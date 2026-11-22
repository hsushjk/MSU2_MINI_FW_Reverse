# G2 (re-verified) — boot chain, clock tree, TIM3 tick, USART2/DMA, USB endpoint transport

Data: `dump7/funcs`, `listing7.txt`, `q2sites.txt`, raw `image.bin` (0x5EA0 B). Every number below was
re-read from listing7/image.bin, not from `reports/g2_plumbing.md` (dump6/listing6: 4x-wrong compressed
immediates, `lbu/sb` for quadrant-2 `lhu/sh`). gp = 0x20000880.

## Module summary

1. crt0 (0x3BB4) is stock WCH `handle_reset`: gp/sp setup, 176-byte .data copy from 0x5DF0, 4352-byte
   .bss clear, four CSRs, SystemInit, then `mepc = 0x39B4 (main); mret` — main runs with MIE on.
2. mtvec = 7 = base 4 + mode 3 (WCH vectored), so IRQ n vectors to `4+4n`: USB IRQ45 → 0xB8, TIM3
   IRQ54 → 0xDC. Both slots hold the expected handler; unused slots hold 2-byte `c.j self` hangs.
3. Clock: HSI only, 48 MHz. SystemInit ends with CFGR0 = its reset value 0x00000000 (HPRE=0, no
   SW/PLL/PPRE write), so HCLK = PCLK1 = PCLK2 = 48 MHz; FLASH = 2 wait states. No PLL, no HSE, no
   HSIRDY poll. SystemCoreClock = 48e6 from .data; `SystemCoreClockUpdate` (0x3B74) is never called.
4. TIM3: PSC = 47 (hidden `0x827C` store at 0x0AF6, which Ghidra drops), ARR = 999 → 1 kHz / 1 ms.
   ISR 0x0E4C increments a **u16** at 0x200000CC (`lhu/addi/sh`, single writer, non-atomic).
5. USART2 (0x40004400) is configured twice: a TX-only 8N1 console (0x441E, PA2/AF_PP, BRR 0x1A1) and
   a full re-init 0x0B34 (PA2 AF_PP, PA3 IPU, PA15 Out_PP) reachable only from the USB ISR path.
6. The UART DMA path is **inert, not dangerous**: both channels get `CNTR = 0` because 0x0CC8 never
   stores BufferSize, so no byte ever moves and the 2048-byte RX ring is never filled.
7. 0x0FD2 is a USB EP sender (all registers 0x400234xx), not a UART write; callers use EP3 IN.
8. 0x100/0x108/0x116/0x130 are `-msave-restore` stubs that return through t0 (`82 82`); they are the
   sole reason the decompiler prints `extraout_a2/a3/a4` and `undefined8 uVar1`.
9. Real defects are: the dead DMA path, UART reconfiguration inside the USB ISR, a 16-bit ms counter
   with a magnitude-only timeout, an even-parity path that leaves 8-bit words, and silent fault hangs.

## Verified facts

| addr | verified behaviour (evidence) |
|---|---|
| 0x3BB4 | `auipc gp,.+.; addi -0x334` → gp 0x20000880; `auipc sp,0x20001; addi 0x444` → sp 0x20005000 |
| 0x3BC4 | .data: src 0x5DF0 (`auipc a0,0x2; addi 0x22c`), dst 0x20000000, end gp-0x7D0 = 0x200000B0 → 0xB0 = **176 B**, 44 words (`bgeu` 0x3BD8-0x3BE8) |
| 0x3BEC | .bss: 0x200000B0 … 0x200011B0 (`auipc a1,0x1FFFD; addi 0x5C0`) = **0x1100 = 4352 B**, word loop |
| 0x3C06 | `csrw 0x0BC0, 0x1F` (WCH core-config CSR; bit meanings not derivable from image) |
| 0x3C0C | `csrw 0x0804, 3` = INTSYSCR: INE\|HPE → **interrupt nesting enabled** |
| 0x3C12 | `csrw mstatus, 0x88` (MPIE\|MIE, MPP=0) |
| 0x3C1A | `auipc t0,0xFFFFC` + `addi 0x3EA` wraps to 4, `ori 3` → **mtvec = 7** (base 4, mode 3) |
| 0x3C2A | `jal ra,0x3B32` SystemInit; 0x3C2E `auipc+addi -0x27A` → mepc 0x39B4; 0x3C3A `mret` (never returns) |
| 0x0..0x1C | vec[0] = `j 0x3BB4`; [1]=[2]=0; [3]=0x0E48; [4]=0x0E4A; [6]=0x3BB2 — 0x0E48/0x0E4A/0x3BB2 are `01 a0` = `c.j self` |
| 0xB8 / 0xDC | 0xB8 = 0x1110 (USB IRQ45), 0xDC = 0x0E4C (TIM3 IRQ54) — confirms `4+4n` |
| 0x3B32 | CTLR |= 1; CFGR0 |= 0x50 (0x3B48); CFGR0 &= 0xF8FFFF5F (0x3B52); FLASH_ACTLR = (x&~3)\|2 (0x3B5C); CFGR0 &= 0xFFFFFF0F (0x3B6C); redundant rewrite 0x3B6E. **Final CFGR0 = 0x00000000** |
| 0x20000080 | SystemCoreClock = 0x02DC6C00 (48e6) from .data @0x5E70; 0x20000090 = 0x200011B0 heap ptr |
| 0x3B74 | SystemCoreClockUpdate: `48000000 >> tbl[(CFGR0>>4)&0xF]` or `/tbl[...]` per CFGR0 bit7; tbl @0x20000000 = `01..08 01..08` — **0 callers in image → dead code** |
| 0x4064 | RCC_GetClocksFreq: out[0] = 48e6, out[1]=out[2]=out[3] = same value (PPRE1/PPRE2 ignored); caller 0x42B2 picks out[3] for USART1 (0x42F8), out[2] otherwise (0x433E) |
| 0x0ABC | TIM_DeInit(0x40000400); PSC = SystemCoreClock/1e6 − 1 = **47** (store at 0x0AF6, `0x827C`, dropped by Ghidra); ARR = **999**; UIE; PFIC ISER 0xE000E104 |= 0x400000 (bit22 = IRQ54); CEN |
| 0x0E4C | TIM_GetITStatus/ClearITFlag (0x428E/0x42A6) then `lhu a5,-0x7B4(gp); addi; sh` → u16 counter at **0x200000CC**; `mret` |
| 0x07AC | only reader of the tick: `lhu a4,-0x7B4; a5=0x1387(4999); bgeu → timeout`; no other consumer exists |
| 0x0584 / 0x0608 | `sh zero,-0x7B4` (tick reset) + clears 0x200000BE/0x200000BF/0x200000CE and 0x200000D2 |
| 0x441E | console: APB1 |= 0x20000 (USART2), APB2 |= 4 (GPIOA), **GPIO_Init(GPIOA, pin 4 = PA2, mode 0x18 AF_PP)**, `{baud=a0, 8N1, Mode = 0x0008 = TE only}`, USART_Cmd |
| 0x42B2 | USART_Init BRR: `a5 = PCLK*25; a4 = 4*baud; divu; /100, %100; frac = (frac*16+50)/100; BRR = mant<<4\|frac` → 115200 @48 MHz = 0x1A1 (417, −0.08 %) |
| 0x0B34 | pins by *mask*: **PA2 AF_PP** (pin 4, mode 0x18), **PA3 IPU** (pin 8, 0x48), **PA15 Out_PP** (0x8000, 0x10); USART2 CTLR1(+0x0C) &= ~0xC; stop code 1 → 0x3000 (1.5), 2 → 0x2000 (2), else 0; parity 1 → 0x600 odd **+ WordLength 0x1000 (9 bit)**, 2 → 0x400 even (word stays 8 bit), else 0; Mode 0xC; CTLR1 &= ~0x40; CTLR1 \|= 0x2000. No AFIO write anywhere |
| 0x0CC8 | memset 0x2C; `PADDR = 0x40004404`; a0==0 → DMA_DeInit(0x40020080)=CH7, DIR 0x10, MINC, PL 0x1000; else DMA_DeInit(0x4002006C)=CH6, DIR 0, CIRC 0x20, MINC, PL 0x1000. **BufferSize (+0xC) never stored → CNTR = 0**; a2 (0x800) discarded |
| 0x0D74 | DMAR off; CH6/CH7 disable; 0x0B34(baud, a2, a3); 0x0CC8 ×2 (0x0DBE, 0x0DD0, both CNTR=0); DMAR on; 0x0C3A |
| 0x0DEA | called only from 0x11CE (CDC class request handler); baud = gp+0x79E..0x7A1 LE, stop 0x7A2, parity 0x7A3 → 0x0D74; UEP2_DMA = 0x20000BE4; UEP2_CTRL_H &= 0xFFF3 |
| 0x0C3A | zeroes 0x20000FE4 block: 5 halfwords + 0x1E byte at +7, 16×u16 loop at +0xE..+0x2D, 0x200000DC/0x200000E0 |
| 0x0FD2 | **USB**: range check ep−1 ≤ 6 → ep 1..7; busy flag gp−0x794+ep; MOD byte 0x4002340C/0D/0E/0F; `UEPn_DMA = 0x40023410+4ep`, `UEPn_TX_LEN = 0x40023420+4ep`, `UEPn_CTRL_H = 0x40023422+4ep`; only ep 1..4 hit real registers; callers (0x2A8, 0x820, 0x998) all pass **3** |
| 0x0E94 | MOD 0x4C/0x48; DMA ptrs 0x2000106C/0x2000112C/0x20000BE4/0x2000116C; CTRL_H = 2/0/2/2; 7 busy flags cleared |
| 0x0F14 | GPIOC clock (APB2 bit4); PC16 (0x10000) floating, PC17 (0x20000) IPU |
| 0x0F56 | AFIO+0x18 = (x & ~0xCF) \| (a1 ? 0xCC : 0x88); attach: 0x0F14, BASE_CTRL 0, 0x0E94, DEV_ADDR 0, BASE_CTRL 0x29, INT_FG 0xFF, UDEV_CTRL 0x81, INT_EN 7, ISER 0x2000 (IRQ45); detach: 6, delay, 0, ICER |
| 0x0A90 / 0x0E7C | APB2 GPIOA+APB1 USART2/TIM3+AHB DMA1 / APB2 AFIO+AHB USBFS (helpers 0x40C0/0x40DE/0x40A2 → RCC+0x18/+0x1C/+0x14) |
| 0x0208 | byte lookup: a0 ≤ 0xFF → 0x200002E4[a0]; 0x100..0x1FF → 0x5718[a0−0x100]; else 0 |
| 0x0238 | 6-byte copy loop (i = 0..5), no alignment, no overlap handling |
| 0x0100/108/116/130 | save stubs: addi16sp −0x40 (or −0x10 at 0x130), save s-regs, `sub sp,sp,t1`, **`82 82` = c.jr t0**. 0x13C/0x140/0x14A/0x154 restorer entries end `ret`. Listing prints `db 0x82 / db 0x80` for 0x8282 — a listing rendering bug |
| 0x3C3E/3C94/3CA4 | ADC_Init / ADC_Cmd / channel-config: masks 0xE0F0FEFF (+4 CTLR1), 0xFFF1F7FD (+8 CTLR2), 0xFF0FFFFF (+0x2C RSQR1); RSQR1 length = `(u8)init[4]−1 << 20` (0x3C84); SAMPTR (+0x10) ch<10 offset 3·ch, (+0x0C) ch≥10 offset 3·(ch−10); called once each (0x2C34, 0x2C3E, 0x2C58 chain) |

## Defect list (ranked)

1. **0x0CC8 — both DMA channels get CNTR = 0** (BufferSize at struct +0xC is never stored; callers pass
   0x800). RX ring 0x200003E4 is never written, CH7 is never loaded: the whole "UART over CDC" feature
   is dead code that still rides the USB ISR. The old report's "2048 B overflow into the 64-byte slot
   array" is impossible — no transfer ever starts.
2. **0x0DEA→0x0D74→0x0B34 — UART+GPIO+DMA reconfiguration inside IRQ45**: four GPIO_Init RMWs of
   CFGLR/CFGHR, a USART_Init divide, two DMA_DeInit/Init pairs, and a 0x0C3A block clear, all from the
   USB interrupt; any 0x0FD2/0x0250 work can interleave.
3. **0x0B34 — parity code 2 leaves an 8-bit word** (0x0C2E path skips the `WordLength = 0x1000` store at
   0x0BEA) while code 1 is odd+9-bit; the two parity modes frame differently. Stop code 1 → 1.5 stop
   bits (0x3000) is only meaningful with 5-bit words.
4. **0x0E4C/0x07AC — 16-bit ms counter** compared by magnitude (`a5 = 0x1387`), so the "5 s timeout"
   fires early after a wrap (65.536 s) and the ISR's `lhu/addi/sh` is not atomic; `sh zero` at 0x0584/
   0x0608 can race the ISR (≤1 lost tick). Only one consumer exists.
5. **0x3B32 — dead-store SystemInit**: three RMWs of CFGR0 to end at its reset value (the `|= 0x50` at
   0x3B48 is wiped at 0x3B6C), PLL bits cleared by code that never sets them, no HSIRDY poll, no PPRE
   programming, FLASH latency a bare literal.
6. **0x3B74 never called; 0x4064 ignores PPRE1/PPRE2** and both read a 1..8 divisor table from .data
   (0x20000000/0x20000010) instead of the documented AHBPrescTable encoding; `divu` by a zero entry
   would yield 0xFFFFFFFF. SystemCoreClock is a hand-maintained constant.
7. **0x0FD2 — endpoint range 1..7 but only 1..4 map to real registers** (5..7 alias TX_LEN/CTRL_H of
   1..3); return 0 conflates "sent" with "MOD bit clear, nothing sent" (0x1058); `len` unchecked
   against 64; the zero-copy path still writes TX_LEN from a2; buffer = UEPn_DMA + 0x20000000.
8. **0x0E94/0x0F56 — dead EP1 config** (UEP1_* programmed, never used), fused attach/detach with three
   `BASE_CTRL = 0` writes, an undocumented AFIO+0x18 write whose value depends on the reset source, and
   no USBFS reset pulse.
9. **0x3FEE — split BSHR write**: low half → GPIOA+0x10 (BSHR), high half → GPIOA+0x20, an offset that
   is not in the CH32 GPIO map (`BCR` is +0x14); called as 0x3FEE(GPIOA, 4) from 0x0B58 to set PA2.
10. **0x3C3E — `((u8)init[4] − 1) << 20`** (0x3C84-0x3C8E, masked to 8 bits) unguarded: a sequence length
    of 0 writes 255 into RSQR1.L[7:0]; three non-atomic RMWs of CTLR1/CTLR2/RSQR1; 0x3CA4 never bounds
    the channel, so ch ≥ 18 shifts a 3-bit field into reserved bits.
11. **0x0E48/0x0E4A/0x3BB2 are `c.j self`** and slots 0x04/0x08 are literal 0: any fault or stray
    interrupt hangs silently with no mcause/mepc capture.
12. **0x0A90 duplicates clocks already enabled by 0x441E** (GPIOA, USART2); 0x0208 has two magic windows
    with 0 used both as data and as the error value.

## Corrections vs the old report / open items

- Pins were read as *numbers*: the field is a **mask**. 0x0B34 configures PA2/PA3/PA15, not PA4/PA8/
  PA15; the console also uses PA2. "Drives the wrong pin" (old defect 4) is void.
- `SystemCoreClockUpdate` (0x3B74) is never called — old report said it runs after reset.
- The "100 ≤ counter" tick consumer does not exist in the image; only 0x07AC (4999) reads the tick.
- Old defect 1 (RX overflow) is impossible; old defect 2's "0x0C3A zeroes the endpoint busy flags" is
  wrong (it clears 0x20000FE4..0x20001025, the busy flags are 0x200000EC..0x200000F2).
- Confirmed unchanged: mtvec 7/base 4, tick u16 @0x200000CC, PSC 47 hidden at 0x0AF6, CNTR = 0,
  0xFD2 register math, 0x3FEE's stray +0x20 store, ADC masks.
- UNKNOWN: meaning of CSR 0x0BC0 bits; whether UEPn_DMA keeps only 15:0; what GPIOA+0x20 is on this die;
  the AFIO+0x18 bit functions; DMA1 CH6/CH7 hardware mapping (inferred from PADDR/DIR only).