# g5 -- WCH-style HAL re-verified against dump7 / listing7 (CH32X033, fw V1.1)

Evidence: `listing7.txt` (every instruction re-read), `q2sites.txt` (16-bit quadrant-2: off = 8*(b12:b10) + 2*(b6:b5)), `dump7\funcs\*.c`, `ch32x035.h`, `symbols.tsv`, and `image.bin` for the RAM initial image.

## 1. Module summary

- WCH StdPeriph re-implemented as small base-pointer helpers: a0 = peripheral base, a1 = config struct or bit mask. Only 0x4202 (TIM1 0x40012C00 / TIM2 0x40000000), 0x42B2 (USART1 0x40013800) and 0x3E80 (GPIOA 0x40010800 / GPIOB 0x40010C00 / GPIOC 0x40011000) compare a0 against literals; 0x4064, the RCC enables/resets, PWR, 0x3E6A and SysTick use absolute bases; the rest inherit the caller's literal (`lui 0x40013` = SPI1 0x40013000, `lui 0x40004`+0x400 = USART2 0x40004400).
- The 18 small accessors are all 16-bit `lhu`/`sh` read-modify-write helpers -- the old "8-bit, offset unknown" reading is obsolete. Only 0x4138, 0x4202, 0x42B2, 0x3E80 and 0x4064 are real functions; the rest are 6-14 instructions.
- Layout: DMA 3D7C/3E1E/3E56/3E6A/3E78 / GPIO 3E80/3FEE/3FFC / PWR 4010/4016/4026/4034 / RCC 4064/40A2/40C0/40DE/40FC/411A / SPI1 4138/416A/4184/4196/419A/419E / TIM 4202/4258/4270/4282/428E/42A6 / USART 42B2/4342/4358/436A/4372/437C / Delay 4388/43B2/43E8 / USART2 441E/447E / allocator 44BC/44E6.
- Boot context that the HAL depends on (`_start` 0x3BB4): gp = 0x20000880, sp = 0x20005000; .data 0x20000000-0x200000B0 is copied from flash 0x5DF0 (0x3BD8-0x3BE8) and .bss 0x200000B0-0x200011B0 is zeroed (0x3BFC-0x3C02). SystemInit = 0x3B32, SystemCoreClockUpdate = 0x3B74 (stores into 0x20000080), main = 0x39B4.
- Functionally the module is close to the vendor library and **most of the old report's defects do not survive**: memcpy has no overrun, the allocator window matches the linker layout, and the BSXR pull writes are the documented encoding. The real residue is listed in S7.

## 2. Accessor table (all 16-bit; base proven from the identity test or the caller's literal)

| fn | peripheral (proof) | register / offset | size | R/W | idiom |
|---|---|---|---|---|---|
| 0x4138 | SPI1 (`lui 0x40013` @0x1CD0, 0x1E06) | CTLR1 0x00; CRCR 0x10 | 16 | R/W | SPI_Init: CTLR1=(CTLR1&0x3040)\|OR(desc.hw[0..7])=0x0307; CRCR=desc.hw[8] |
| 0x416A | SPI1 (same) | CTLR1 0x00 bit6 | 16 | R/W | SPI_Cmd (SPE): `ori 0x40` / `andi -0x41` |
| 0x4184 | SPI1 (same) | **CTLR2 0x04** | 16 | R/W | set/clear CTLR2; calls 0x1CDC/0x1CE8 -> TXDMAEN\|RXDMAEN |
| 0x4196 | SPI1 (`lui a0,0x40013` @0x1B1C) | DATAR 0x0C | 16 | W | SendData |
| 0x419A | SPI1 (same) | DATAR 0x0C | 16 | R | ReceiveData (named so in symbols.tsv) |
| 0x419E | SPI1 (same) | STATR 0x08 | 16 | R | GetFlagStatus -> bool; 0x1B10 polls 0x02 TXE / 0x01 RXNE |
| 0x4202 | TIMx via `a0==0x40012C00 \|\| a0==0x40000000` | CTLR1 0x00, PSC 0x28, ATRLR 0x2C, RPTCR 0x30 (T1/T2 only), SWEVGR 0x14 | 16 | W | TimeBaseInit: clears CEN/DIR/CMS + CKD, sets PSC/ARR, then UG |
| 0x4258 | TIMx | CTLR1 0x00 bit0 | 16 | R/W | TIM_Cmd (CEN) |
| 0x4270 | TIMx | INTENR 0x0C | 16 | R/W | TIM_ITConfig |
| 0x4282 | TIMx | INTFR 0x10 | 16 | W | ClearITPendingBit: writes `~mask` |
| 0x428E | TIMx | INTFR 0x10 + INTENR 0x0C | 16 | R | GetITStatus = (INTFR&m) && (INTENR&m) |
| 0x42A6 | TIMx | INTFR 0x10 | 16 | W | byte-identical duplicate of 0x4282 |
| 0x42B2 | USART1 0x40013800 else PCLK1 branch (0x4302 `bne`) | BRR 0x08, CTLR1 0x0C, CTLR2 0x10, CTLR3 0x14 | 16 | R/W | USART_Init (S5) |
| 0x4342 | USARTx | CTLR1 0x0C bit13 | 16 | R/W | USART_Cmd (UE); disable `&0x3DFFF` |
| 0x4358 | USART2 (caller 0x0D94 `lui 0x40004`+0x400) | CTLR3 0x14 bit6 | 16 | R/W | clear DMAR (called with 0x40) |
| 0x436A | USARTx | DATAR 0x04 | 16 | W | SendData, `a1 & 0x1FF` |
| 0x4372 | USARTx | STATR 0x00 | 16 | R | GetFlagStatus |
| 0x437C | USARTx | STATR 0x00 | 16 | W | ClearFlag: writes `~mask` (0x0B34 clears 0x40 TC) |

Other range functions (32-bit unless noted):

| fn | peripheral | register / mechanism | R/W | note |
|---|---|---|---|---|
| 0x3D7C | DMA1, ch = a0 | CFGR+0, CNTR+4, PADDR+8, MADDR+C; INTFCR 0x40020004 | W | DeInit: `CFGR &= 0xFFFE` (EN off), zero the 4 words, then per-channel INTFCR RMW |
| 0x3E1E | DMA1, ch = a0 | CFGR/CNTR/PADDR/MADDR | R/W | Init; CFGR = new fields \| (old & 0x3800F), i.e. vendor `CFGR_MASK` |
| 0x3E56/6A/78 | DMA1 | CFGR bit0 (R/W) / INTFR 0x00 (R) / INTFCR 0x04 (W) | -- | Cmd / flag test / plain W1C clear |
| 0x3E80 | GPIOA/B/C | CFGLR 0x00, CFGHR 0x04, CFGXR 0x1C; BSHR 0x10, BCR 0x14, BSXR 0x20 | R/W | GPIO_Init (S3) |
| 0x3FEE/3FFC | GPIOx | BSHR = pins&0xFFFF, BSXR 0x20 = pins>>16; BCR 0x14 when a2=0 | W | SetBits / SetOrResetBits |
| 0x4010/16/26 | RAM 0x20000104 / PWR CTLR 0x00 / PWR CSR 0x04 | W / R/W / R | -- | flag setter (no reader) / PLS=(old&~0x60)\|a0 / flag test |
| 0x4034 / 0x4064 | PWR+RCC / RCC | BOD probe / CFGR0 0x04 read, out[0..3]={SYSCLK,HCLK,PCLK1,PCLK2} | R/W / R | S4 / S6 |
| 0x40A2/40C0/40DE / 40FC/411A | RCC 0x40021000 | AHBPCENR 0x14, APB2PCENR 0x18, APB1PCENR 0x1C / APB2PRSTR 0x0C, APB1PRSTR 0x10 | R/W | enable `\|= mask`, disable `&= ~mask` |
| 0x4388 / 43B2/43E8 | RAM / SysTick 0xE000F000 | p_us gp-0x776, p_ms gp-0x778 / SR 0x04, CMPLR 0x10, CMPHR 0x14, CTLR 0x00 | R/W | Delay_Init / Delay_Us/Ms (S4) |
| 0x441E / 447E | USART2 + GPIOA + RCC | cfg struct / STATR/DATAR via 0x4372/0x436A | W / R/W | init (S5) / polled putchar (polls TC 0x40) |
| 0x44BC / 44E6 | RAM | heap ptr 0x20000090 / -- | R/W | bump allocator 0x200011B0..0x20004800 / memcpy |

## 3. GPIO_Init 0x3E80 -- CFGLR / CFGHR / CFGXR arithmetic

- desc a1 = {[0] pin mask -> a3 (0x3E92), [1] speed (0x3E8C), [2] mode -> a4 (0x3E80)}; a6 = mode&0xF (0x3E86), and if mode&0x10 then a6 |= speed (0x3E8E), i.e. CNF<<2 | MODE; nibble = a6 << (pin*4).
- Pins 0-7 (if a3&0xFF): `c.lw a5,0x0(a0)` = **CFGLR read live** (0x3E9C), nibble replaced, `c.sw a5,0x0(a0)` back (0x3EDE). Pull-down (mode 0x28/N1) -> `sw a7,0x14(a0)` BCR (0x3ED4); pull-up (0x48/P1) -> `sw a7,0x10(a0)` BSHR (0x3FA2).
- Pins 8-15 (if a3&0xFF00, guard 0x3EE6): a5 comes from a **RAM shadow, CFGHR is never read** -- 0x3EF6 `lw a5,-0x7fc(gp)` (A), 0x3FB0 `-0x7f8(gp)` (B), 0x3FBA `-0x7f4(gp)` (C) = 0x20000084/88/8C, written back at 0x3F4A/0x3FD2/0x3FDC; `c.sw a5,0x4(a0)` -> CFGHR (0x3F40). Pull bits as above, via `sw t1,0x14(a0)` (0x3F32) / `sw t1,0x10(a0)` (0x3FC4).
- Pins 16-23 (only if a3 >= 0x10000, 0x3F4E `c.lui a5,0x10`): `c.lw a2,0x1c(a0)` = **CFGXR read live** (0x3F54), stored back at 0x3F9A. Pull select uses bit 1<<(16+n): IPD -> BCR (0x3F92), IPU -> BSXR (0x3FE6 `sll`, 0x3FEA `c.sw a1,0x20(a0)`). That is GPIO_BSXR_BS16..BS23 and the same OUTDR-bit idiom as 0x3FEE/0x3FFC -- **not** a wrong register, and BCR/BSHR for the low pins is the same idiom (old report item 6 is wrong).
- The three shadows are seeded to **0x44444444** in .data (flash 0x5E74/0x5E78/0x5E7C), i.e. the CFGHR reset value, and this scan found no CFGHR write outside 0x3E80 -- so boot is consistent. It remains a duplicated-state hazard rather than a proven corruption: any writer that bypasses 0x3E80 desynchronises the cache silently, and nothing ever re-reads CFGHR to resynchronise.
- Fall-through bug: if a0 is none of the three bases the compare chain reaches 0x3EFA without loading a shadow, so a5 still holds the CFGLR word read at 0x3E9C (or the 0 from 0x3E94) and 0x3F40 writes that into CFGHR of the given base.

## 4. Delay 0x4388 / 0x43B2 / 0x43E8 and the brown-out probe 0x4034

- 0x4388: `lw a5,0x80(0x20000000)` = SystemCoreClock, `/8 000 000` (0x7A1200 = `lui 0x7a1; addi 0x200`), `&0xFF`, `sb` gp-0x776; x1000 -> `sh` gp-0x778. At 48 MHz: p_us = 6, p_ms = 6000; the unit is **SysTick ticks per us**, 6 = HCLK/8 (CTLR.STCLK is never set, so the base really is HCLK/8).
- 0x43B2/0x43E8: clear SR bit0; ticks = p_us (lbu 0x43BE) or p_ms (lhu 0x43F4) x a0; CMPLR = ticks, CMPHR = 0; CTLR |= 0x10 (STRE), then |= 0x21 (INIT|STE); poll SR bit0 at 0x43DE with **no timeout**; CTLR &= ~1. Unbounded and non-reentrant (CMPLR is global), but no ISR calls it (0x43B2 <- 0x0FBE, 0x1A38, 0x404C; 0x43E8 <- 0x277E/0x2794/0x27A6/0x27B2), so the race is latent.
- 0x4034: 0x4388 again (0x4038) -> 0x40DE(0x10000000) PWR clock -> 0x4016(0x60) PLS=0b11 -> Delay_Us(10) -> 0x4026(0x04) PVDO -> 0x4016(0) PLS=0 -> returns PVDO. PWR_CTLR bit4 (PVDE) is never set and this header defines no PVDE, so the detector is only level-selected.

## 5. USART init 0x441E and the baud divisor

- 0x40DE(0x20000,1) USART2 clock; 0x40C0(4,1) GPIOA; 0x3E80(0x40010800,{4,1,0x18}) = PA2 AF push-pull (0x443A-0x444E); cfg at sp+0x10 = {baud=a0, 0x00000000 (8 data bits, 1 stop), 0x00080000 (parity 0x0000 | Mode 0x0008 = TE), 0x0000}; 0x42B2(0x40004400,&cfg) at 0x446A; 0x4342(0x40004400,1) at 0x4474. main 0x39C2 (`c.lui a0,0x1c; addi 0x200` = 0x1C200 = 115200) -> **115200, 8N1, TX only**.
- 0x42B2 fills {SYSCLK,HCLK,PCLK1,PCLK2} from 0x4064 into sp+0x10..0x1C; for a0 != 0x40013800 it jumps to 0x433E and takes **PCLK1** (`c.lwsp a5,0x18(sp)`) -- that block is the live path, not dead code.
- BRR: q = PCLK1*25/(baud*4) = 100/USARTDIV (0x4308-0x4314); int = q/100, frac = ((q%100)*16+50)/100; stores `int<<4 | (frac & 0xF)` at +0x08. At 48 MHz / 115200: q = 1 200 000 000/460 800 = 2604 -> int 26, frac (4*16+50)/100 = 1 -> BRR = 0x1A1 = 26.0625 -> 48e6/(16/26.0625) = 115 107 Bd (-0.08 %): **matches 115200**. No zero check on baud (`divu` at 0x4314).

## 6. RCC_GetClocksFreq 0x4064

`*out = 48 000 000` hard-coded (0x4064 `lui 0x2dc7; addi -0x400` = 0x2DC6C00); t = byte at 0x20000010[(CFGR0>>4)&0xF] (0x407A-0x4084 `c.lbu`); HCLK = 48e6>>t if CFGR0 bit7 (HPRE) else 48e6/t (`divu` 0x4096, no zero guard); PCLK1 = PCLK2 = HCLK -- PPRE1/PPRE2 are never read. It is a duplicate of SystemCoreClockUpdate 0x3B74 (same table, same shift/divide, storing to 0x20000080). Correct here only because SystemInit 0x3B32 leaves HPRE = /1 (`andi a4,a4,-0xF1` at 0x3B68) and PPRE at /1.

## 7. Ranked defects

1. **0x3D7C (med)** -- the per-channel flag clear is a read-modify-write of INTFCR 0x40020004 (`c.lw` 0x3DA2/0x3DB4/0x3DC4/0x3DD8/0x3DE6/0x3DF6/0x3E06/0x3E16, `ori` 0x3DA4/0x3DB6/0x3DCC, shared `c.sw` 0x3DA8), masks 0xF,0xF0,0xF00 ... 0xF0000000. INTFCR is write-1-to-clear: if it reads back the flag image, the OR carries other channels' pending bits into the write and clears them too. 0x3E78 avoids this with a plain `sw`; nothing in the image can settle INTFCR's read behaviour, so this is an idiom violation with a conditional side effect.
2. **0x3E80 (med)** -- CFGHR is cached in RAM (0x20000084/88/8C) while CFGLR and CFGXR are read live. Seeded to 0x44444444 so boot is right and no other writer exists today, but the cache is never resynchronised from CFGHR, so any future direct CFGHR write (or a second init path) silently re-writes stale config.
3. **0x3E80 (low-med)** -- unrecognised base -> CFGHR receives the CFGLR word (fall-through at 0x3EFA). Latent: every known caller passes GPIOA/B/C.
4. **0x4138 (med)** -- SPI_Init ORs eight halfwords of descriptor into CTLR1 but writes CRCR from `desc.hw[8]`, i.e. the word at sp+0x1c, which neither caller initialises (0x1CB0-0x1CCE and 0x1DE6-0x1E04 write only sp+0xc..sp+0x18). The `li a5,7` before 0x1CCC suggests 7, but 0x1CCC is one of the three undecoded `c.wch100` insns, so SPI1 CRCR receives either 7 or stack rubbish; inert only because CRCEN (bit 13 of CTLR1, preserved by the 0x3040 mask) is never set.
5. **0x4064 / 0x42B2 (low-med)** -- the clock tree is hard-coded (SYSCLK 48 MHz, PCLK1 = PCLK2 = HCLK, PPRE1/PPRE2 ignored) and duplicated in 0x3B74; any prescaler change silently mis-reports the clocks and therefore the baud divisor. No zero guard on the HPRE table value (`divu` 0x4096) or on baud (`divu` 0x4314).
6. **0x4034 (low)** -- PLS=0b11 without PVDE; PWR clock left enabled; PLS cleared to 0 instead of restored to its previous value; Delay_Init re-run (recomputes gp-0x776/778).
7. **0x43B2/0x43E8 (low)** -- unbounded poll with no timeout, global SysTick reprogramming without save/restore, and CTLR is left stopped (`&= ~1`) rather than restored.
8. **0x3E1E (info)** -- keeps CFGR bits 0-3 (EN, TCIE, HTIE, TEIE) across a re-init, exactly like the vendor `CFGR_MASK` idiom; a re-init of a running channel restarts CNTR/PADDR/MADDR while EN stays 1.
9. **Dead code (info)** -- 0x42A6 duplicates 0x4282 byte for byte; 0x447E's only caller 0x5128 (FUN_5112) is unreferenced; 0x0CC8 (USART2 RX DMA, ch7 normal / ch5 circular) has no caller; 0x4010's global 0x20000104 has four inline stores (0x06FA/0x0A2E/0x0B20/0x0FA4) and no reader.
10. **Not defects (checked and cleared)** -- 0x44E6 is a correct newlib-style memcpy (aligned fast path, 9-word unroll, `p < (end & ~3)` word tail, byte tail; no overrun, and its callers 0x10FE/0x12A4/0x1350/0x158C/0x15F6/0x1716 are all in range). 0x44BC's window is linker-consistent: the heap pointer at 0x20000090 is initialised to 0x200011B0 (= _end, the .bss limit) and the upper bound 0x20004800 = sp 0x20005000 - 0x800, i.e. a 2 KB stack reserve; caller 0x4F78 does test the 0xFFFFFFFF failure. 0x447E polling TC before each byte is slow but correct.

## 8. Corrections vs reports\g5_hal.md (stale)

- 0x4184 is CTLR2 (0x04) -> TXDMAEN/RXDMAEN, not CTLR1 -> CPOL/MSTR; 0x42A6 and 0x4282 are the same code (INTFR 0x10); 0x4202's last store (+0x14) is SWEVGR.UG, not an interrupt enable; 0x433E is the live PCLK1 branch.
- The 18 accessors are 16-bit `lhu`/`sh` with proven offsets (old: 8-bit, unknown offsets); 0x3E80's BSXR/BCR/BSHR pull writes are correct, not a wrong register.
- Withdrawn after re-check: the memcpy overrun (0x44E6), the "unrelated" heap window (0x44BC -- it matches _end and sp-0x800), the "allocator failure ignored" claim (0x4F78 checks), and the "CFGHR shadow starts zeroed" implication (it is seeded 0x44444444).
- Confirmed in the old report: 0x4064 has no zero check and ignores the APB prescalers; 0x4010's flag has no reader; 0x3D7C's INTFCR access is a read-modify-write of a W1C register.