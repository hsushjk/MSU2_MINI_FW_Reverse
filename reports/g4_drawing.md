# MSU2_MINI (CH32X033F8P6) — drawing pipeline, re-verified from dump7 / listing7

Scope: 0x2A20 0x2A9C 0x2BCC 0x2C58 0x2C9E 0x2CF4 0x2D48 0x2DA2 0x2E42 0x2E80 0x2EA4 0x305A 0x30C6 0x3148
0x3180 0x31C0 0x33B8 0x367C 0x3698 0x36B8 0x37D4, with 0x2060/0x20F8/0x1F0A/0x2668/0x26BE/0x2714 and the
GPIO/DMA helpers 0x3E1E/0x3E80/0x3FFC/0x3E56/0x3E6A/0x3E78 as dependencies. gp = 0x20000880.

## 0. Module summary (17 lines)

21 functions over one skeleton: `GPIO_Init(PA6)=Out_PP` → `FUN_2A20` window → optional flash read →
`PA9=0, PC3=0` → `PA6=1` → poke CH3 `MADDR`/`CNTR` → enable → **spin on TCIF3, no timeout** → deassert.
CH3 (SPI1_TX 0x40020030) also serves the TX side of every flash read; CH2 (RX 0x4002001C) receives.
Both are configured once by main (0x2C9E/0x2CF4); only 0x37D4 re-inits CH3. The transfer unit is one
256-byte slab = 128 RGB565 pixels; nothing is double-buffered, clipped or range-checked.
One 262-byte scratch area is reused under five hard-coded bases: 0x2000012C, +6 = 0x20000132,
0x20000114/+6 = 0x2000011A, 0x200001D2, 0x20000130.
Dispatcher FUN_250 class 2 holds x/y/w/h/fg/bg in gp-0x7CC/0x7C8/0x7CA/0x7C6/0x7CE/0x7D0 and picks one
of 12 primitives: colour blit 0x2EA4, mono blit 0x31C0, 32×64 glyph 0x367C→0x31C0, 16×16 GB2312
0x36B8, mono-over-photo 0x33B8, 32×64 MIX 0x3698→0x33B8, 16×16 over photo 0x37D4, raw stream
0x305A/0x30C6/0x3148, UI key helper 0x2778, solid rect 0x2A9C; class 4 builds a 256-byte tile (0x2E80).
The vendor API's 8th parameter (**BG_Page**) is never supplied: the two MIX primitives receive the
background *colour* in the page slot (D8).
**0x2BCC and 0x2C58 are not drawing**: ADC1 init (GPIOB pin 2 analogue, 0x40012400, ADCON CR1/CFGR/
SAMPTR) and one ADC read (`return RDATAR & 0xFFFF` after polling STATR bit 1).

## 1. Verified geometry / stride table

| # | quantity | verified value | evidence (listing7) |
|---|---|---|---|
| Q1 | CASET | cmd 0x2A + x0>>8, x0, x1>>8, x1 — column offset **0** | 0x2A26, 0x2A38–0x2A5C |
| Q1 | RASET | cmd 0x2B + bytes of **y0+0x18, y1+0x18** — row origin **+24**, no x shift | `addi s0,a3,0x18` 0x2A2A, `addi s1,a1,0x18` 0x2A30, bytes 0x2A68–0x2A88 |
| Q1 | RAMWR | cmd 0x2C, no data byte | 0x2A90 |
| Q1 | +24 applied by | 0x2A9C 0x2EA4 0x305A 0x31C0 0x33B8 0x36B8 0x37D4 — all 7 call sites of 0x2A20 | 0x2AD8, 0x2EF0, 0x3094, 0x3204, 0x33FC, 0x3742, 0x3828 |
| Q1 | +24 NOT applied | **0x2DA2 only** — inline `0,x,0,(x+7)&0xff` / `0,y,0,(y+7)&0xff` | 0x2DB4–0x2DF8 (no `addi …,0x18`) |
| Q1 | stride 0x140 | 0x33B8 slab **and** tail ⇒ 320 B/row = **160 px** | `li s10,0x140` 0x3460, `addi s7,s7,-0x140` 0x3464, `mul a1,s0,s10` 0x346C |
| Q1 | stride 0x1E0 | 0x37D4 ⇒ 480 B/row = **240 px** | `li a6,0x1e0` 0x386A, `mul a7,s5,a6` 0x3878 |
| Q1 | panel width | CASET is 0..159 (main draws 160 wide); the two background strides disagree 160 vs 240 ⇒ one path shears 80 px/row | main 0x39B4 lines 48/49 |
| Q2 | mono source | flash → **0x20000114**, 16 B per 128-px slab; bit pointer **0x2000011A** (buffer+6) | 0x3302, 0x3454, `addi a1,s6,0x132` 0x3312 |
| Q2 | pixels/byte | **8, MSB first** (`0x80>>bit`; `*p<0` then `*p<<=1`) | 0x2E20, 0x3180 |
| Q2 | expansion split | **80 px → 0x20000132**, **48 px → 0x200001D2** (= 16 B, exact) | `li a4,0x50` 0x3316, `li a4,0x30` 0x3382 |
| Q2 | DMA window / CPU overlap | CH3 reads **0x20000132–0x20000231** (MADDR 0x3368, CNTR 0x100 0x336E, enable **0x3372**); CPU writes **0x200001D2–0x20000231** at **0x3386**, after enable ⇒ same buffer | as cited |
| Q3 | bytes/px, order | **2 B, big-endian, no swap** (`*p=c>>8; p[1]=c`) | 0x2AE2–0x2AF8, 0x3180, 0x2714 |
| Q3 | source flow | page<<8 → DMA (0x0B, 256 B) into 0x2000012C → CH3 streams **0x20000132** (=dst+6) 0x100 B, residual `(w*h*2)&0xFF`; chunk count `((w*h*2) mod 2^32)>>8`, unsigned | 0x2060 0x2098–0x20B4; 0x2ED4/0x2EF4/0x2EF6/0x2EFA; 0x2F06/0x2F0E/0x2F34 |
| Q4 | glyph source | **flash table 0x5848** (`lui 0x6`, `-0x7b8`), 8 B/glyph, direct `lbu` — no DMA, no page | 0x2E00–0x2E0A, 0x2E1A |
| Q4 | advance / wrap | x+=8; wrap when **x+8 > 0xE8** ⇒ 30 cols / **240-px line**; y=(y+10)&0xFF, x=0 (10-px pitch) | `li s5,0xe8` 0x2E50, `bgeu s5,s0` 0x2E70, `c.addi s1,0xa` 0x2E74 |
| Q4 | 160×80? | 240 > 160 ⇒ cols 20–29 off-panel. main: image y=4 → rows 28–95, text y=96 with no +24 → rows 96–103; contiguous **iff** the visible area is rows 24–103 = exactly 80 rows. Geometry fits a 160×80 panel with text on absolute rows; the wrap width does not | main lines 49, 51–53 |
| — | 16×16 GB2312 | 32 B at `((b0-0xA1)*0x5E + b1 + 0x11CF)*0x20` (== `+0x1270` with `b1-0xA1`), polled 0x03 reader | 0x36B8; 0x37D4 C line 64 |
| — | 32×64 glyph | one flash page per glyph: `base+index` (0x367C, unmasked) / `(base+index)&0xFFFF` (0x3698) | 0x367C, 0x3698 |

## 2. Ranked defects

**D1 [Q5] 0x30C6 — host length used raw, wait unbounded.** `CNTR = a1` 0x30FE (16-bit, packet p[3..4]),
source 0x20000130 (256-B tile), enable 0x3110, poll 0x3114–0x311C. **len=0: CNTR=0 starts no transfer,
TCIF3 never rises, the loop spins forever with PA9/PC3 held low (0x30DC/0x30E8)** — reset required.
len up to 65535 also dumps ~64 KB of SRAM to the panel.

**D2 [Q5] 0x3148 — unconditional TCIF3 poll 0x314C–0x3154.** A lone "end stream" (op 9) spins on a flag
that 0x30C6 already cleared (0x3122) ⇒ permanent hang, panel left in the previous CS state.

**D3 [Q5] 0x2A9C — 32-bit `mul`, arithmetic `srai`, `!=` loop test.** `mul s0,s0,s1` 0x2AFC,
`srai s4,s0,0x7` 0x2B2C, `for (i=0; i != (w*h)>>7; i++)` with a 256-B burst + busy-wait each pass.
0x100×0x100 = **512 slabs = 131072 B** into a 160×80 window; `w*h >= 0x80000000` (e.g. 0xFFFF²) gives a
**negative** bound that `i` can never equal ⇒ effective infinite loop. w,h,colour unclamped.

**D4 [Q3] 0x2EA4 — unbounded page walk.** page runs `a4 … a4+((w*h*2)>>8)`, unsigned, never checked
against the 4096-page device; A23 comes straight from the page, so 0xFFFF² walks ~0x7FFF pages with
address wrap and streams garbage indefinitely. Same class: 0x31C0 `page*256+n*16` (0x3208–0x3218),
0x367C `page = base+index`.

**D5 [Q2] 0x31C0 — CPU writes the DMA source after enable.** 0x3372 enables CH3; 0x3386 then expands
48 px into **0x200001D2–0x20000231**, the last 96 B of the range CH3 is reading (0x20000132+0x100).
No HT flag, no double buffer: the right 96 px of every 128-px slab race the SPI. This is main's first
drawing call (line 49, 160×68 mono page 0xEEC), so the boot screen depends on it.

**D6 [Q5] 0x20F8 — 5-byte handshake, then unsynchronised read-ahead.** The spin `c.lw a1,0x20(a0)` /
`bltu s2,a1` (0x2188/0x218A) only proves the 5 header bytes left; the compose then reads
`write_ptr+5/+6` (`c.lbu a0,0x3(s0)`, `0x4(s0)` at 0x21F4–0x21FC) with no further check ⇒ CPU must
outrun SPI or it composites stale bytes. `CH2.CNTR = count+5` (0x2150–0x2156) with count ≤ 0x100 makes
the RX write **261 B at 0x2000012C–0x20000230, 5 B past the tile**. The `0x40020034 = 0x40020020`
store in the spin loop **is real** (0x2154) and harmless only because NDTR ignores writes while EN=1.

**D7 [Q3] payload phase contradiction.** 0x2060 writes its header at **dst+1…dst+4** (0x209A–0x20A8)
with `CH3.CNTR = len+5` (0x20AC), so a 0x0B read puts the first data byte at **dst+5**; the consumers
read from **dst+6** (0x2F06 → 0x20000132; 0x31C0/0x33B8 → 0x2000011A) while 0x20F8 uses **+5**.
Both cannot be right: either every image is half a pixel off, or the flash images carry a 1-byte pad.
Needs one dump of an image page (0xEEC00); the only +5 user is the unreachable MIX path (D8).

**D8 [Q3] BG_Page never supplied.** Op 4 → `FUN_33B8(x,y,w,h,page,fg,bg)` and op 6 → `FUN_37D4(x,y,p+3,
fg,bg)` get **gp-0x7D0 (the bg RGB565 colour)** in the page slot; 0x33B8 does `slli s7,a6,8` (0x3416),
0x37D4 `*0x100` on a4. Default bg 0xFFFF ⇒ background read from 0xFFFF00, 16× past the 1 MB part.

**D9 [Q1] the two background strides disagree** — 0x140 (160 px) in 0x33B8 (0x3460/0x346C) vs 0x1E0
(240 px) in 0x37D4 (0x386A/0x3878); both literals, neither a parameter. On a 160-px panel one path is
sheared 80 px/row. 240 agrees with 0x2E42's wrap, 160 agrees with CASET: two panel widths in one module.

**D10 [Q4] 0x2DA2 — no +24, 8-bit coordinates, 240-px wrap.** Y sent raw (0x2DE4/0x2DF0) while every
other primitive adds 24, so text and graphics at equal y are 24 rows apart; `x+7`/`y+7` are `andi 0xff`
(0x2DCE/0x2DF4) with hard-coded 0 high bytes (0x2DBA/0x2DC4/0x2DDE/0x2DEA) ⇒ x=250 gives x1=1<x0 and a
backwards window. 30 columns / 240 px puts 10 columns off-panel. No clipping in the module at all.

**D11 0x3180 — destructive 16-bit counter.** `*pcVar2 <<= 1` destroys the caller's bit source and the
counter is `(i+1)&0xFFFF` vs a 32-bit `param_5`, so any count > 0xFFFF loops forever (latent: callers
pass ≤ 0x80).

**D12 0x31C0 tail — one byte over-read.** `FUN_2060(…, (rem>>3)+1)` (C line 59) fetches a byte of the
*next* flash page for any `rem` not a multiple of 8; the final 48-px expansion consumes it.

**D13 0x2E80 — tile expander, not a memcpy.** Contiguous `src[0..3]` (0x2E84–0x2E9A) written 64× with
`dst += 4`; the source pointer never advances, so only 2-pixel patterns are possible (fills from packet
p[2..5]) and it writes 0x100 B at 0x20000130, i.e. 4 B into the slab at 0x2000012C. The old
"reads pkt[2], pkt[6], pkt[0xE]" was a dump6 immediate artefact.

**D14 architectural.** Every primitive re-arms CH3 by direct pokes; only 0x37D4 calls the init. Eight
independent blocking TCIF3 spins (0x20D0, 0x2BB6, 0x3114, 0x314C, 0x338A, 0x20F8 tail, 0x2EA4, 0x2A9C)
have no timeout and neither TEIF nor TCIF2 is ever checked in the drawing path, so a stuck SPI/flash is
indistinguishable from a slow one: the device hangs instead of reporting.