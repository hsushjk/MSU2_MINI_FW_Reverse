# G3 — SPI flash (P25D80) driver + LCD bring-up

Functions: 0x1A66, 0x1A86, 0x1B10, 0x1B66, 0x1BC6, 0x1CFC, 0x1E32, 0x1E6C, 0x1E7A, 0x1EA8, 0x1F0A,
0x1F80, 0x1FF6, 0x2060, 0x20F8, 0x2202, 0x2306, 0x23BE, 0x2412, 0x2466, 0x24BA, 0x250E,
0x2668/0x26BE/0x2714, 0x2778. Sources: listing7.txt, q2sites.txt (16-bit lhu/sh operands), calls.tsv.

## Module summary

- **SPI1 = 0x40013000**, configured twice with an identical 9-field `SPI_InitTypeDef` (0x1BC6 flash,
  0x1CFC LCD): Direction 0x0000 (2-line full duplex), Mode 0x0104 (MSTR|SSI), DataSize 0x0000
  (8-bit), CPOL 0x0002 (high), CPHA 0x0001 (2Edge) ⇒ **mode 3**, NSS 0x0200 (software, SSM=1),
  BR 0x0000 ⇒ **fPCLK/2**, MSB first. `FUN_4138` ORs those fields into CTLR1 (keeps 0x3040) and
  writes CRCR; `FUN_416A` sets SPE last; `FUN_4184(·,2,1)+(·,1,1)` ⇒ CTLR2 = 0x03 (TX|RX DMAEN).
  **SSOE never set**, CTLR2 never written by SPI_Init itself.
- **Clock 24 MHz.** `FUN_4388` computes 1 ms = 0x1770 SysTick ticks from 8e6 ⇒ HCLK = PCLK2 =
  48 MHz (no PLL/HSE) ⇒ SCK = 24 MHz. `FUN_43E8(n)` is a real ms delay (`delay(10)` = 10 ms),
  `FUN_43B2` the µs twin.
- **Pins.** PA5 = SCK, PA7 = MOSI (both AF_PP), PB7 = flash /CS (Out_PP, idle high), PC3 = LCD /CS,
  PA0 = LCD RESET, PA9 is toggled in every LCD transaction but has no board load,
  **PA6 = flash MISO *and* LCD D/C**.
- **PA6 ownership changes only by re-running `GPIO_Init`**: input (mode 4) at 0x1BC6, 0x1F80
  (0x1F9E), 0x2202, 0x2306, 0x250E (0x2512), and per flash burst in 0x2EA4 (0x2F26), 0x31C0
  (0x321A/0x3230/0x32F8), 0x33B8 (0x343C/0x34B4), 0x36B8 (0x36DE), 0x37D4 (0x385C); output
  (mode 0x10) at 0x1CFC, 0x2EA4 (0x2ECC), 0x31C0 (0x31EC/0x3264/0x331E), 0x33B8
  (0x33D8/0x3552/0x35F0), 0x36B8 entry, 0x37D4 (0x381A). Boot order (main 0x39B4): 0x1BC6 →
  0x1EA8 → 0x1A66 → 0x1B66 → 0x1FF6 → 0x1A86 → **0x1CFC (PA6 out)** → 0x2778.
  `0x1EA8/0x1B66/0x1FF6/0x1F0A/0x2060/0x20F8` never touch PA6; **`0x2778` never touches it either.**
- Every command is built at **0x2000012C** (`gp-0x754`) and shifted by the blocking engine 0x1B10
  (poll STATR&2 ≤201, DATAR, poll STATR&1 ≤201, read DATAR; **returns 0 silently on timeout**).
  Erase/program add **DMA1 CH3** (0x40020030, TX from SPI1_DATAR) with CH2 (0x4002001C, RX) armed
  alongside; only **TCIF3 (0x200)** is ever awaited.
- Addresses are 24-bit with an implied zero low byte: `cmd, page>>8, page&0xFF, 0x00` ⇒ addr =
  `page<<8`. No 4-byte addressing, no 0x9F/0xAB read, no size or ID validation (0x90's result at
  `gp-0x5BC+4`, 0x3A6E, is never compared).
- LCD bit-banged through SPI1, /CS per byte: 0x2668 = command, 0x26BE = data, 0x2714 = 2 data bytes.

## Command table (page = 8-bit address unit; addr = page<<8; CS = PB7 unless noted)

| # | Function | CMD | Header bytes | Dummy/payload | Data path (CNTR) | Notes |
|---|----------|-----|--------------|---------------|------------------|-------|
| 1 | 0x1EA8 | 0x90 Read MF/DevID | `90 00 00 00` | read 2 (MF, DEVID) | polled | returns `(b0<<8)\|b1`; stored, never checked |
| 2 | 0x1E32 / 0x1E6C | 0x05 RDSR | `05` | read 1 | polled | `do{…}while(sr&1)` — **no timeout** |
| 3 | 0x1E7A | 0x06 WREN | `06` | — | polled | WEL never read back |
| 4 | 0x1B66 | 0x4B Read UID | `4B FF FF FF FF` | read 16 | polled | → `gp-0x59C`+4..+0x13, clobbers chip UID |
| 5 | 0x1FF6 | 0x48 Read Security | `48 00 10 00` | 1 dummy + read 60 | polled | sec. reg 1 @0x001000 → +0x1E..+0x59 |
| 6 | 0x1F0A(dst,addr,len) | 0x03 Read | `03 A2 A1 A0` | 1 per byte | polled | arbitrary address, blocking |
| 7 | 0x1F80 | 0x03 Read 1B | `03 A2 A1 A0` | 1 | polled | host class3/sub0; PA6→input |
| 8 | 0x2060(buf,addr,len) | 0x0B Fast Read | `0B A2 A1 A0` | 1 dummy + len | **CH3 = len+5** (0x20B0) / **CH2 = len+6** (0x20B4) | **counts disagree**; TCIF3 only |
| 9 | 0x20F8(buf,addr,len,·) | 0x0B Fast Read | `0B A2 A1 A0` | 1 dummy + len | **CH3 = CH2 = len+5** (0x2150/54/56) | 1bpp→RGB565 in place |
| 10 | 0x2202(page,end) | 0x81 then 0x02 | `81 A2 A1 00` / `02 A2 A1 00` | 256 data | erase polled; prog **CH3 = 0x104** | src fixed 0x20000130 |
| 11 | 0x2306 | 0x02 Page Program | `02 A2 A1 00` | 256 data | **CH3 = 0x104** | no erase |
| 12 | 0x23BE(page) | 0x81 Page Erase | `81 A2 A1 00` | — | polled | `li a0,0x81` @0x23DE (Puya-only opcode) |
| 13 | 0x2412(page) | 0x20 Sector Erase | `20 A2 A1 00` | — | polled | 4 KB; `li a0,0x20` @0x2432 |
| 14 | 0x2466(page) | 0x52 Block Erase | `52 A2 A1 00` | — | polled | 32 KB; `li a0,0x52` @0x2486 |
| 15 | 0x24BA(page) | 0xD8 Block Erase | `D8 A2 A1 00` | — | polled | 64 KB; `li a0,0xd8` @0x24DA |

DMA: CH2 = 0x4002001C, CH3 = 0x40020030 (CFGR +0, CNTR +4, PADDR +8, MADDR +0xC). 0x2060 sets CH3
MADDR = buf+1 (0x2080), CH2 MADDR = buf (0x2082), clears both EN bits with mask 0xFFFE (0x206C-7A).

## 0x250E erase planner — what it really erases

`(P, N)` = (start page, count) from host class3/sub2. `s4 = P+N` is set **once** (0x2558); `page` is
recomputed as `(s4-N)&0xFFFF` at 0x255A/0x256C/0x2586. `s2 = ((P&0x7F)>>4)` (+1 if `P&0xF`) `&7`;
`s3 = ((P>>7)&1) xor (s2!=0)`.
1. Head 0x2562→0x25E8: `while (P&0xF && N) { N-=1; 0x23BE(page); }` — P…16-page boundary.
2. Sector loop 0x25F8: `while (N>=0x10 && s2) { 0x2412(page); N-=0x10; s2=(s2+1)&7; }` — runs exactly
   `8-s2` times, so `page` ends 0x80 (32 KB) aligned.
3. Optional 32 KB 0x2610: `if (s3 && N>0x7F) { 0x2466(page); N-=0x80; page+=0x80; }`.
4. 64 KB loop 0x2586/0x2630: `while (N>0xFF) { 0x24BA(page); N-=0x100; page+=0x100; }`.
5. Tail 0x2592: optional 0x2466 for `t>=0x80`, then `(t&0x7F)>>4` × 0x2412 (loop 0x2640) and
   `t&0xF` × 0x23BE (loop 0x2654).

⇒ For **P+N ≤ 0xFFFF the erased set is exactly [P, P+N)**: every erase is issued on a properly
aligned page, **nothing outside the range is erased**, no gaps. The old "walks past the end" claim is
false. Real faults: 16-bit wrap (D8) and PA6 left as input (D1).

## 0x1A86 licence check

Record `R = gp-0x59C` (0x200002E4): `R[0..0x0B]` = chip die UID 0x1FFFF7E8..F3 (0x1A66), `R[4..0x13]`
= flash UID from 0x4B (0x1B66) — **overlapping** — `R[0x1E..0x59]` = 60 security bytes (0x1FF6),
`R[0x14..0x1D]` never written (BSS zeros). It then writes `R[4]=0x59`, `R[5]=0x58` (0x1A8E/0x1A94),
overwriting two of its own inputs. 15 iterations from 0x1AA6: `b1=R[2k+6]; b2=R[2k+7];
R[2k+6]=~b1; R[2k+7]=b2^0x51;` then compare `((~b1&0xFF)<<8)|(b2^0x51)` with the 16-bit product
`R[k]*R[k+0xF]`. All match → same scramble, `R[7]=1` (0x1B0A), return 1. Mismatch (0x1AD0) → 30×
`{R[j+0x1E]=0; R[j]^=0xA5;}`, `R[0x1F]=0`, return 0. Main 0x3A82→0x3A8C stores the result in
`gp-0x7B1`; it is read **once** (0x3ACA) and only suppresses three text draws at y=0x60 (0x5C58
"MSN isn't activated."). **Nothing functional is gated.**

## Ranked defects

1. **PA6 not restored — LCD re-init runs with D/C floating.** 0x1F80 (0x1F9E), 0x2202, 0x2306, 0x250E
   (0x2512) set PA6 input and never restore; **0x2778 never programs PA6 at all**. Host class3/sub4 →
   0x2778 after any sub0-3 leaves D/C undriven. Only 0x2EA4/0x31C0/0x33B8/0x37D4 repair it.
2. **0x2060 count mismatch + consumer offset.** CH3 = len+5 (0x20B0) but CH2 = len+6 (0x20B4): CH2
   never completes, is never disabled, TCIF2 never awaited (only 0x200). Data lands at **buf+5**, 0x2EA4
   reads from **buf+6** (0x2F7C/0x2F84/0x2F86) ⇒ every fast-read row is one byte late, stale tail.
3. **0x20F8 overwrites the RX DMA buffer while it still fills** (guard `CH2.CNTR <= len` = 5 bytes in,
   both CNTR = len+5) and destroys the source bitmap (`slli a0,1; sb`), writing at buf-2/buf-1.
4. **SCK 24 MHz vs ST7735S max ≈15.15 MHz** (BR=000, 48 MHz PCLK2): 1.58× the 66 ns serial cycle.
5. **Reset/init timing.** 0x11 SLPOUT then `delay(10)` = 10 ms vs 120 ms (0x27A0-AC); no delay after
   0x29 DISPON (0x29C8); RESX low 10 ms, 10 ms to 0x11. `delay()` itself is sound.
6. **MADCTL 0x36 sent with no parameter when orientation ∉ {0,1}**: 0x29B0 `lbu`, 0x29B4 `li 0x78`,
   0x29B8 `beqz`, 0x29BC `bne` → 0x29C8 skips 0x26BE, so the panel keeps reset MADCTL. Host sub4
   passes p[3] unvalidated.
7. **No timeout anywhere.** 0x1E6C spins on `sr&1` forever; 0x1B10 returns 0 after 201 polls and every
   caller uses it as data. An absent/hung flash hangs the MCU in 0x1EA8/0x1E7A/0x1E32; no error path.
8. **Erase planner 16-bit wrap.** Page arithmetic masked at 0x255E/0x256C/0x258A/0x25A4; P/N are
   unvalidated host u16s ⇒ P+N > 0xFFFF wraps and the loops erase **near page 0**.
9. **Programs always 256 bytes and never check WEL**: 0x2202/0x2306 hard-code CNTR = 0x104 and source
   0x20000130 (host length ignored), and 0x1E7A's 0x06 is never verified, so a refused WREN no-ops
   silently and is reported as success.
10. **Licence check is self-referential, destructive, non-gating**: it overwrites its own inputs
    R[4]/R[5], compares against a half-zero table (R[0x14..0x1D] are BSS), wipes the security copy on
    failure, toggles three strings, cannot pass twice, and is not bound to the device ID.
11. **Window inconsistency.** 0x2778 sets X 26..105 / Y 1..160 and issues RAMWR with no data
    (0x29D0-0x2A18); 0x2A20 sets X from 0 and Y+0x18 (0x2A2A/0x2A30). Module-specific, undocumented.
12. **Unproven suspicions:** 0x4B sends address 0xFFFFFFFF instead of 0x000000 (0x1B66); opcode 0x81 is
    Puya-specific; 0xFC 0x80 is not in a ST7735S command table that can be cited; SWRESET/NORON/INV/IDLE-off/
    DISCTRL are never sent; PA9 is toggled in every LCD transaction but has no board load.

Datasheet figures in this report are quoted from memory and were not re-checked against the original
document; MEDIUM/LOW items are leads, not findings.