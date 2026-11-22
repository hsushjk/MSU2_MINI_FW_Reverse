# g1_usb — USB FS device stack re-verified against dump7 / listing7 / q2sites.txt

## Module summary
- USBFS device @0x40023400 (CH32X033, 20 KB SRAM 0x20000000..0x20004FFF); gp = 0x20000880.
- Code: sub_E94 (0xE94) device init, FUN_F56 (0xF56) connect/disconnect, vec_B8 (0x1110) IRQ13 handler, sub_FD2 (0xFD2) endpoint send, sub_C3A (0xC3A) ring reset, FUN_44E6 (0x44E6) byte-wise memcpy, sub_DEA (0xDEA) commit line coding, sub_43B2 (0x43B2) busy-wait delay.
- ISR = one flat dispatcher on INT_FG(0x406)/INT_ST(0x407): INT_FG bit1 endpoint events, bit0 bus reset, bit2 suspend; INT_ST&0x30==0x30 SETUP, ==0x20 IN (0x20 EP0 / 0x21 EP1 / 0x23 EP3 / 0x24 EP4), ==0 EP0-OUT if bit6, 2 EP2-OUT, 4 EP4-OUT. Epilogue writes INT_FG = 2 (write-1-clear).
- Data in: EP2 OUT, one DMA slot per packet into a 16 x 64 B ring at gp+0x364 (0x20000BE4), metadata 0x20000FE4..0x20001011, drained by main loop FUN_ram_0000059a (0x59A).
- Data out: only EP3 is ever loaded (6 B from gp-0x7BC; callers 0x2A8/0x820/0x998). EP1 and EP4 are enabled but never loaded; EP2 IN cannot exist (TX bit clear).
- Control: 12-entry standard table at 0x5818 (bRequest 0..11), class dispatcher 0x1512 (CDC ACM + HID), and a default-accept vendor path with no handler.
- Descriptors: device 0x5CE0, config 0x5C74 (107 B, CDC-ACM IAD + HID, 3 interfaces, 5 endpoints), HID class desc 0x5CC8, report desc 0x5CF4, strings 0x5D18/0x5D28/0x5D3C.
- Verdict: works as a vendor data pipe. The HID half of the composite device is inert, the CDC control interface is decorative, and two host-reachable paths (SET_REPORT accumulator, vendor IN) give an unprivileged host unchecked memory access. Ranked list in §5.

## 1. Endpoint model (Q1)
All writes in sub_E94, 0xE9C-0xEEA. Bit meaning from the firmware's own CTRL_H values (0xCA = ACK/ACK, 0xCF = STALL/STALL, bit6 T_TOG, bit7 R_TOG): bits1:0 T_RES, bits3:2 R_RES.
- 0xE9C `sb 0x4C,0x40C` -> UEP4_1_MOD = 0x4C: EP4 nibble 0xC (bit2 TX-enable, bit3 double-buffer), EP1 nibble 0x4 (TX).
- 0xEA4 `sb 0x48,0x40D` -> UEP2_3_MOD = 0x48: EP2 nibble 0x8 (double-buffer only, no TX), EP3 nibble 0x4 (TX).
- UEP567_MOD 0x40E is never written in the image (only read at 0x1046) -> EP5/6/7 disabled.
- DMA: UEP0 0x410 = 0x2000106C (64 B; EP4's in-buffer is the implicit +0x40 = 0x200010AC), UEP1 0x414 = 0x2000112C, UEP2 0x418 = 0x20000BE4, UEP3 0x41C = 0x2000116C.
- CTRL_H init: UEP0 0x422 = 2, UEP1 0x426 = 2, UEP3 0x42E = 2, UEP4 0x432 = 2, UEP2 0x42A = 0; TX_LEN 0x424/0x42C = 0. Connect: BASE_CTRL 0x400 = 0x29, UDEV_CTRL 0x401 = 0x81, INT_EN = 7, DEV_ADDR = 0.

| desc EP | declared | hardware | actually used |
|---|---|---|---|
| 0x81 | IN intr 64 | EP1 TX enabled | never (no CDC serial-state notification) |
| 0x02 | OUT bulk 64 | EP2 RX, double-buffered | yes, main data pipe |
| 0x83 | IN bulk 64 | EP3 TX | yes, 6-byte reports |
| 0x84 | IN intr 64 | EP4 TX + DBUF | never loaded -> zero-length INs only |
| 0x04 | OUT intr 64 | EP4 RX not enabled | handler 0x1454 unreachable |

Mismatches (0x5C74 vs hardware): (i) 0x04 declared OUT but EP4 is configured TX-only, so the HID OUT endpoint does not exist; (ii) 0x84 declared but UEP4_TX_LEN (0x40023430) is only ever written 0 (0x1472), so HID IN can only return zero-length packets; (iii) 0x81 is enabled but never transmitted on, so the IAD's notification endpoint is dead; (iv) 0x02 is declared bulk OUT but implemented as a double-buffered 16-slot FIFO with no NAK back-pressure (D4). Consistent: wTotalLength 0x6B = 107 = 0x5C74..0x5CDE, EP0 max packet 64 = 64-byte buffer, IAD union 0->1, bMaxPower 0x32 = 100 mA, bmAttributes 0x80.

## 2. Data path in (Q2) — EP2 OUT, INT_ST&0x3F == 2 -> 0x1388
- 0x138A/0x138E: UEP2_CTRL_H ^= 0x80 (R_TOG toggle), stored back — re-arms the receiver.
- 0x1394: len = RX_LEN (u16 @0x40023408), masked to 16 bits at 0x139A/0x139C. No clamp to 64; the value is trusted as delivered by the core.
- 0x13A6-0x13B2: that 16-bit len is stored at gp+0x76A+2*P (P = producer, u16 at gp+0x764/0x765).
- 0x13C8-0x13D2: producer = (P+1) & 0xFFFF written back to gp+0x764/0x765.
- 0x13D8-0x13E4: UEP2_DMA (0x40023418) = gp+0x364 + 64*(P+1), the next slot.
- 0x13EE-0x1400: if (P+1) > 0xF -> UEP2_DMA = gp+0x364 (wrap) and the u16 at gp+0x76C is zeroed.
- 0x1402-0x141C: pending counter (u16 at gp+0x768/0x769) incremented; 0x1426-0x144E: if counter > 13 -> UEP2_CTRL_H = (CTRL_H & ~0xD) | 0x8 and gp+0x79D = 1.
- Payload: the core DMAs the packet to whatever UEP2_DMA pointed at before it, i.e. slot P of 16 x 64 B = 0x400 at 0x20000BE4..0x20000FE3. 16 x 64 B geometry confirmed.
- Consumer (FUN_ram_0000059a, C lines 39-101): C = u16 at gp+0x766; len = *(u8*)(gp+0x76A+2*C) (low byte only); payload slot gp+0x364+64*C; on consume it zeroes the u16 at gp+0x772+2*C, increments the two bytes at gp+0x766/0x767, wraps at 16 (clearing the u16 at gp+0x76E) and decrements gp+0x768.
- gp+0x764 (0x20000FE4) = **producer index**. Not 0x766 — the task hint and the old report are wrong.
- gp+0x766 (0x20000FE6) = consumer index, written only by the main loop. gp+0x768 (0x20000FE8) = pending-packet counter.
- gp+0x76A (0x20000FEA) = per-slot length table, 16-bit entries, indexed 0..15. gp+0x775 is referenced by no instruction; it is byte 3 of the 16-word array at gp+0x772.
- gp+0x79D (0x2000101D) = flag set to 1 at 0x144E ("queue >= 14"), never read. The live SET_REPORT accumulator is a different u16 at gp-0x79C = 0x200000E4.

## 3. Data path out (Q3) — sub_FD2 @0xFD2, u32 sub_FD2(u32 ep, u32 src, u16 len, u32 mode)
- `if (ep-1 > 6) return 1`, then `if (busy[gp-0x794+ep]) return 1` (busy = 8 bytes at 0x200000EC, set here, cleared only by the matching IN-completion handler: 0x124C EP0, 0x12E6 EP1, 0x1306 EP3, 0x122C EP4).
- Nibble select 0x1008-0x1050: ep1/ep4 -> UEP4_1_MOD (high nibble iff ep==1), ep2/ep3 -> UEP2_3_MOD (high iff ep==3), ep5/ep6 -> UEP567_MOD (high iff ep==6), else UEPX_MOD 0x40F.
- 0x1052: `if ((nib & 4) == 0) return 0` — success reported for an endpoint with TX disabled.
- Registers: CTRL_H = 0x40023422 + 4*ep; DMA = *(u32*)(0x40023410 + 4*ep); TX_LEN = 0x40023420 + 4*ep.
- Double-buffer branch (nib&8): offset 0x40; if ((nib&1) && (CTRL_H&0x40)==0) offset = 0xC0 else 0x80; destination = DMA + 0x20000000 + offset. Single-buffer branch: if ((nib&1) && (CTRL_H&0x40)) -> +0x40, else if (mode==0) UEPn_DMA = src & 0xFFFF, else +0. Then memcpy(dst,src,len) via FUN_44E6.
- 0x10CE: UEPn_TX_LEN = len; 0x10D0 `*CTRL_H &= 0xFC`; 0x10D4 busy[ep] = 1; return 0.
- Callers 0x2A8, 0x820, 0x998 — all ep=3, src=gp-0x7BC, len=6, mode=1. No caller uses ep 1,2,4,5,6,7.
- Success without transmitting? Yes: (a) the nib&4 test returns 0 with nothing done; (b) it only fills the buffer and TX_LEN — transmission happens on the next IN token, so a host that never polls gets nothing while the call reports success; (c) for ep=4 the DMA expression 0x40023410+16 = 0x40023420 is UEP0_TX_LEN (no UEP4_DMA exists), so an ep=4 call would derive its destination from EP0's TX length — latent, unreachable today.
- Nothing ever sends on EP2 IN (TX bit clear) or EP4 IN (TX_LEN never non-zero); EP3 IN is the only sender.

## 4. Control path (Q4)
- SETUP 0x14AA: UEP0_CTRL_H = 0xCA; the 8 bytes have already been DMA'd into the EP0 buffer at UEP0_DMA = gp+0x7EC = 0x2000106C. The handler copies [0] -> gp-0x784 (0x200000FC, bmRequestType), [1] -> gp-0x78A (0x200000F6, bRequest), [2:3] -> gp-0x782 (0x200000FE, wValue), [4:5] -> gp-0x788 (0x200000F8, wIndex), [6:7] -> gp-0x786 (0x200000FA, wLength).
- Split 0x14E2-0x150E on bmRequestType&0x60: 0 -> standard 0x1676, 0x20 -> class 0x1512, 0x40 -> vendor 0x1660.
- Standard 0x1676: bRequest > 0xB -> STALL (0x1864, CTRL_H = 0xCF); else jump via 0x5818: 0 GET_STATUS 0x1940, 1 CLEAR_FEATURE 0x180A, 2/4/7 STALL, 3 SET_FEATURE 0x1896, 5 SET_ADDRESS 0x1798, 6 GET_DESCRIPTOR 0x1690, 8 GET_CONFIGURATION 0x17E2, 9 SET_CONFIGURATION 0x17F8, 10 GET_INTERFACE 0x193A, 11 SET_INTERFACE 0x17A2 (so SET_DESCRIPTOR is a STALL).
- SET_ADDRESS stores wValue&0xFF at gp-0x79A (0x200000E6); DEV_ADDR is written only in the EP0-IN completion at 0x12CE as (DEV_ADDR & 0x80) | value — correct, after the status stage.
- SET_CONFIGURATION stores wValue&0xFF at gp-0x799 (0x200000E7) and unconditionally sets gp-0x798 = 1; SET_INTERFACE shares the generic responder 0x17A2 and validates nothing; GET_INTERFACE returns 0.
- CLEAR/SET_FEATURE: device recipient only wValue=1, and SET_FEATURE(remote wakeup) always STALLs because bmAttributes at 0x5C7B = 0x80 has bit5 clear; endpoint recipient wValue=0 maps wIndex 0x81->UEP1_CTRL_H, 0x02->UEP2_CTRL_H (&0xC), 0x83->UEP3_CTRL_H, 0x84/0x04->UEP4_CTRL_H; else STALL.
- GET_STATUS: device and endpoint recipients only (interface -> STALL); reply byte 1 when the halt field is 3/0xC, 2 when gp-0x797 bit0 (remote wakeup) is set. gp-0x797 bit1 = suspended.
- GET_DESCRIPTOR: wValue>>8 = 1 device 0x5CE0 (18 B), 2 config 0x5C74 (len = b[3:2] = 0x6B), 3 string (0 -> RAM gp-0x7D8, 1 -> 0x5D18 "wch.cn" 0x0E, 2 -> 0x5D28 "CH32x035" 0x12, 3 -> 0x5D3C "0123456789" 0x16), 0x21 -> 0x5CC8 (9 B), 0x22 -> 0x5CF4 (0x22 B); any other type -> STALL. Reply cursor = u32 pointer at gp-0x780, advanced by the bytes sent.
- Class 0x1512: 0x01 GET_REPORT (STALL unless wLength==0, then a 0-byte reply — never returns a report), 0x02 GET_IDLE (gp-0x78C), 0x03 GET_PROTOCOL (gp-0x78B), 0x09 SET_REPORT, 0x0A SET_IDLE (gp-0x78C = wValue>>8), 0x0B SET_PROTOCOL (gp-0x78B = wValue&0xFF), 0x20 SET_LINE_CODING, 0x21 GET_LINE_CODING, 0x22/0x23 silent no-ops, >0x23 STALL.
- CDC line coding: the 7 data bytes are copied from the EP0 buffer to gp+0x79E..0x7A4 at 0x118C-0x11BA, but only in the EP0-OUT completion (0x1164) after the host sends the data stage; gp+0x7A5 then gets 0x1E (0x11C0) and is immediately overwritten from gp+0x76B (0x11C4-0x11CA), then sub_DEA programs the UART. gp+0x7A5 is never read, so that aliasing is harmless. GET_LINE_CODING points the cursor at gp+0x79E (0x15AE).
- Vendor: no vendor request exists. 0x1660 computes 0 for any request with bmRequestType bit6 set (0xFF otherwise, unreachable) and jumps into the generic responder 0x1566, which copies min(wLength,0x40) bytes from the pointer at gp-0x780 into the EP0 buffer and sends it.

## 5. Defects, ranked
D1. **Host-controlled unbounded write, SET_REPORT.** acc = u16 at gp-0x79C (0x200000E4), zeroed at 0x1544; each OUT data packet does memcpy(gp+0x7AC + acc, EP0buf, RX_LEN) at 0x134A/0x1350 and acc += RX_LEN at 0x1368, with no bound and no guard beyond 16-bit truncation. Writes are 64-byte chunks from 0x2000102C wrapping mod 65536, so anything in 0x2000xxxx (EP2 ring at 0x20000BE4, stack, descriptor cursor) can be overwritten, and acc > 0x3FD3 leaves SRAM (0x20005000) entirely.
D2. **Vendor requests are accepted and answered through an uninitialised pointer.** 0x1660 -> 0x1566; nothing ever writes gp-0x780 (0x20000100) before the first vendor request, so a vendor IN copies up to 64 bytes from address 0x00000000 (flash) and advances the cursor by n per request. Nothing is ever STALLed; a host can walk flash/RAM 64 bytes at a time.
D3. **Length metadata aliases a second array.** The ISR stores the 16-bit length at gp+0x76A+2*P for P = 0..15 (0x13A6-0x13B2), but sub_C3A initialises only four length words (gp+0x76A/0x76C/0x76E/0x770) followed by a separate 16-word array at gp+0x772..0x791. Slots 4..15 keep their length inside that array, and the consumer's clear at gp+0x772+2*C (0x9A6-0x9AA) zeroes the length word of slot C+4. Impact appears once more than 3 packets are queued.
D4. **No producer/consumer overrun guard.** The ISR never compares the producer (gp+0x764) with the consumer (gp+0x766). The only back-pressure, counter > 13 at 0x1426-0x144A, rewrites UEP2_CTRL_H to (CTRL_H & ~0xD) | 0x8 — R_RES stays ACK — and sets a flag nobody reads, so a host may keep sending and overwrite unconsumed slots; the counter is a plain u16 that wraps.
D5. **HID endpoint 0x84 can only send zero-length packets.** UEP4_TX_LEN is written only at 0x1472 with 0 and UEP4_CTRL_H is initialised to 2 (T_RES = ACK) at 0xEDA, so EP4 IN stays armed and answers IN tokens with ZLPs; 0x122C only re-arms.
D6. **EP4 OUT handler is dead code.** 0x1454 (INT_ST bit2) complements RX_LEN bytes from gp+0x82C into gp+0x86C, which nothing reads — and it cannot run because UEP4_1_MOD = 0x4C enables EP4 TX, not RX.
D7. **EP1 (declared 0x81 IN) is TX-enabled at 0xE9C but never loaded** (no sub_FD2 call with ep=1), so no serial-state notification is ever sent; EP1 also answers IN tokens with ZLPs.
D8. **String descriptor 0 (LANGID) comes from never-written RAM.** 0x1770/0x1776 read gp-0x7D8 (0x200000A8), which no instruction writes, so GET_DESCRIPTOR(STRING,0) returns a 0-byte descriptor.
D9. **wLength is zeroed for every class/vendor OUT data stage** at 0x117E, so the "more data expected" test at 0x11D0 always takes the status branch: multi-packet control OUT transfers (SET_LINE_CODING or SET_REPORT with wLength > 64) get a premature zero-length status IN.
D10. **SET_CONTROL_LINE_STATE (0x22) / SEND_BREAK (0x23) are silent no-ops**, and the ACM descriptor claims bmCapabilities 0x02 while the implemented requests are the line-coding group it does not advertise.
D11. **Compliance gaps**: GET_STATUS(interface) STALLs (0x1940 handles recipients 0 and 2 only); SET_CONFIGURATION(0) is not honoured (gp-0x798 always set to 1, never cleared); SET_INTERFACE accepts any wValue/wIndex; GET_REPORT never returns data.
D12. **sub_FD2 can report success without transmitting** (0x1052 TX-bit test, buffer-only preparation); with ep=4 it would compute its DMA from UEP0_TX_LEN (0x40023420) because UEP4 has no DMA register.
UNKNOWN: core semantics of T_RES == 00 (sub_FD2 leaves it at 0x10D0 while the completion handlers write 10) — if 00 is NAK rather than "send", EP3 could never transmit, which the device's observable behaviour contradicts.