# G6 — formatted-I/O engine + small library (0x4598–0x56C4, tables 0x5D54–0x5E08)

Data: **dump7 / listing7.txt / q2sites.txt** (corrected). Every operand size below is taken from
q2sites (Q2 `lhu`/`sh` are 16-bit, offsets even 0..62) — the old `dump6`/`listing6` numbers are void.

## Module summary
- newlib-nano stdio core re-targeted by WCH: `printf`→`_vfprintf_r`→per-byte `putc`; the only live
  backend is `__swrite`→`_write_r`→USART2 polled write. 37 functions in dump7/index.tsv, 0x4598–0x56C4.
- **Q1 — calling convention.** `0x4598` prologue `c.addi16sp sp,-0x40` spills a1→0x24(sp), a2→0x28,
  a3→0x2c, a4→0x30, a5→0x34, a6→0x38, a7→0x3c; at `0x45C2 c.addi4spn a3,sp,0x24` and `0x45CA
  c.jal 0x493E` with a0=`*(gp-0x7ec)` (_impure_ptr/_GLOBAL_REENT), a1=`*(reent+8)` (_stdout),
  a2=fmt, **a3=&va_list**. So the va_list is a plain `int*` into printf's own register-save area and
  it **is** the 4th argument of `_vfprintf_r(ptr, FILE*, fmt, int *ap)` — dump7 keeps it
  (`FUN_ram_0000493e(int,undefined*,byte*,int*)`); the old "4th arg lost" claim is refuted.
- `_vfprintf_r` keeps per-conversion state in a 0xA0 frame at sp+0x10: +0x00 flags, +0x04 precision
  (reset -1), +0x08 min_digits, +0x0C width, **+0x10 len (never initialised — absent from the reset
  block 0x49DC-0x49E8)**, +0x14 running count, +0x18 conversion char (`sb a1,0x28(sp)` @0x4B4A),
  +0x19 pad ' ', +0x1A '0', +0x43 sign, +0x45 'x'/'X'. Digits are built backwards from +0x43.
- Literal runs → `0x48FC(ptr,FILE*,buf,n)`; one conversion → `0x4CD6` (render) + `0x4BCA` (emit pad,
  sign/0x prefix, digits), then the left-justify pad loop at the end of `0x4CD6` (`flags & 2`).
- **Q2 — sink chain** (see table) ends in a per-byte, unbounded spin on USART2 **STATR bit 6 (TC,
  0x40)** with no timeout; `0x447E` always returns `len`, so `_write_r` (0x5112) never reports an error.
- Support set: `_malloc_r` 0x4802 / `_free_r` 0x5546 / `_sbrk_r` 0x4F78 / lock-unlock 0x5542,0x5544;
  reent+FILE glue 0x468C `__sinit`, 0x46F8 `__sfp`, 0x4646 `__sfmoreglue` (0x68-byte FILE,
  blocks of 4), 0x4798 `_fwalk_reent`, 0x463C `__cleanup` = `_fwalk_reent(reent,_fflush_r)`;
  `memchr` 0x552A; **all other syscalls are ENOSYS stubs**: 0x5684 close, 0x5694 fstat, 0x56A4 isatty
  (returns 0), 0x56B4 lseek, 0x56C4 read — each `errno=0x58` + `-1`.
- Tables: 0x5D54/0x5D74/0x5D94 = three 32-byte fallback `__sf[]` FILEs remapped to reent+0xC/+4/+8;
  0x5DB4 `"#-0+ "` (flags 0x1,0x2,0x4,0x8,0x10), 0x5DBC `"hlL"` (→0x40,0x80,0x100), 0x5DC0
  `"efgEFG"`, 0x5DC8 `"0123456789ABCDEF"`, 0x5DDC `"0123456789abcdef"`, 0x5DF0-0x5E07 = 3×{1..8}
  (not referenced from this module).

## Specifiers (dispatch: `c.lbu a3,0x0(a1)` @0x4CEA — rendered offset 0, real +0x18)
| spec | impl | arg | behaviour / evidence |
|---|---|---|---|
| `%d %i` | yes | 4B | `h` → `slli 0x10;srai 0x10` @0x4DF2/0x4DF4; `l`/`L` ignored; '-' @0x4D4A |
| `%u` | yes | 4B | shares `%o` path 0x4D7E; **`%hu` prints all 32 bits** (`andi 0x40` + `beqz` @0x4DF8 skips the only deref) |
| `%o` | yes | 4B | `%#o` prepends '0' from a **stale len** compare @0x4E52/0x4E54 |
| `%x %X` | yes | 4B | `h` → `&0xffff` @0x4EEE/0x4EF0; `#` sets 0x20 @0x4EB6; 'X' stored @0x4E92 |
| `%p` | yes | 4B | forces 0x20 + 'x'; NULL value clears 0x20 @0x4EC0 (no "0x" printed) |
| `%c %C` | yes | 4B | low byte → state+0x42, len=1 @0x4DD2 |
| `%s` | yes | 4B | `len = memchr(s,0,prec)-s`, else `len = prec` (0x4CD6:183-191); no NULL guard |
| `%n` | yes | 4B ptr | `c.sw a1,0x0(a5)` @0x4F1A (4 bytes) |
| `%hn` | yes | 4B ptr | **`sh a1,0x0(a5)` @0x4F28, Q2 `0xA38E` = 2 bytes** — writes 2 bytes, old 1-byte claim wrong |
| `%f %e %g %E %F %G` | **no** | 8B aligned, or 4B if `L` | 0x4B52→0x4B80/0x4B68; **no output at all**, then `count += s4` @0x4B6C-0x4B70 with s4 = previous conversion's width (garbage if first) |
| `%a %A %b %B %m %z %j %t` | **no** | 4B **not** consumed | falls to 0x4D1A/0x4D1E: echoes the letter, len=1 (the `%` disappears) |
| `%%` | yes | none | same 0x4D1A echo path |
| trailing `%` | bug | none | `beq a3,zero,0x4F2A` @0x4D0E sets len=0; format pointer already past the NUL (0x4B46) → scan continues OOB |

## Sink chain (each hop verified)
| addr | name | evidence |
|---|---|---|
| 0x4598 | `printf` | spills a1..a7 @0x24..0x3C, va_list=sp+0x24, `jal 0x493E` @0x45CA |
| 0x493E | `_vfprintf_r` | `c.addi16sp sp,-0xa0`; `__swsetup_r` guard 0x4978-0x4980 (`lhu _flags`, `andi 0x8`) → `jal 0x5148` @0x4A66; emit 0x48FC @0x49C0 |
| 0x48FC | emit n bytes | per-byte loop, `jal 0x48D6` |
| 0x48D6 | `__sputc_r` | `--fp->_w` (+8) <0 → 0x5054; else `*fp->_p++ = c` |
| 0x5054 | `__swbuf_r` | `__sinit` guard 0x5066, sentinel remap 0x5070, `_w = _lbfsize` @0x507E/0x5080 (`c.lw 0x18`→`c.sw 0x8`), `_flags` `lhu` @0x5082/0x50C2; flushes via 0x5148 / 0x53A6 |
| 0x5148 | `__swsetup_r` | reads `_flags` `lhu` 0x5196/0x51DE…, calls `__smakebuf_r` 0x5498 |
| 0x5498 | `__smakebuf_r` | `_malloc_r(0x4802)` + `__swhatbuf_r(0x5442)` + `_isatty_r(0x5654)` |
| 0x53A6 | `_fflush_r` | `_bf._base==0`→0, sentinel remap, `_flags==0`→0, `jal 0x5276` |
| 0x5276 | `__sflush_r` | `lhu a5,0xc(a1)` @0x5276, then `(*fp->_write)(ptr,cookie,p,n)` = `(*(fp+0x28))` |
| 0x4FD6 | `__swrite` | `lhu _flags` @0x4FD6 (Q2 0x25DE), `__SAPP`→`_lseek_r`, tail 0x501C |
| 0x5112 | `_write_r` | `jal 0x447E` @0x5128, errno `*(gp-0x7f0+…)` |
| 0x447E | USART2 writer *(below this module's range; the sink terminus)* | `0x449A li a1,0x40; 0x449E a0=0x40004400; jal 0x4372; 0x44A4 beqz →0x449A` (no timeout), then `lb a1,0(a5)` + `jal 0x436A` |
| 0x4372 | `(lhu 0x0(a0) & mask)!=0` | Q2 site `ram:00004372 0x210a lhu a0,0x0(a0)` (Ghidra body: `(param_1&param_2)!=0` — load dropped) |
| 0x436A | `sh a1,0x4(a0)` masked `&0x1FF` | Q2 site `ram:0000436e 0xa14e sh a1,0x4(a0)` (Ghidra body: empty) |

USART2 = 0x40004400; poll **STATR (0x40004400) bit 6 = TC (mask 0x40)**, write **DATAR
(0x40004404)** as a 16-bit `sh` of `byte & 0x1FF`. Per byte, no timeout, no length cap beyond `n`.

## Ranked defects
1. **`%f %e %g %E %F %G` print nothing and poison the return value** — 0x4B52 (memchr "efgEFG")
   → 0x4B62 (`andi 0x100`, the `L` test) → 0x4B68 (`+4`) or 0x4B80/0x4B82 (`(ap+7)&~7`, `+8`) →
   0x4B6C `c.lwsp a5,0x24(sp); add a5,s4` → loop back to 0x49A6.
   `s4` is only live on the render path, so `printf("x=%f",1.5)` prints `x=` and returns a value
   derived from the previous conversion (or from an uninitialised register if `%f` is first).
2. **Trailing `%` reads the format string past its NUL** — 0x4D0E `beq a3,zero,0x4F2A` sets len=0,
   but the conversion char was fetched at 0x4B3A and the pointer advanced to NUL+1 at 0x4B46; the
   literal scan (0x49D0/0x49D2) then continues in unmapped/unrelated flash and emits it.
3. **Unbounded width/precision → effectively infinite loop** — digits accumulate with
   `mul a5,a5,s6; add a5,a4` and **no overflow check** (width loop 0x4AE4/0x4AEC, precision loop
   0x4B74/0x4B7C). `%2000000000d` left/zero-pads 2e9 bytes through 0x5054; `%.*d` with a huge
   precision does the same via 0x4BCA:51-60 (`iVar1 = width-total`, loop `iVar6 != iVar1`).
4. **`%s` length comes from `memchr(s,0,prec)` and NULL is not special-cased** (0x4CD6:187). On this
   part 0x00000000 is flash, so `printf("%s",NULL)` prints image bytes up to the first NUL byte
   instead of faulting. If no NUL exists within `prec` bytes, `len` falls back to `prec` itself
   (0x4CD6:189-193) — so a precision larger than the remaining image makes 0x48FC emit unmapped
   flash for `prec` bytes (unbounded OOB read, no bounds check anywhere in the emit path).
5. **`%hn` writes 2 bytes** (0x4F28 `sh`, Q2 0xA38E) — correct per newlib, but it is a *halfword*
   store into the caller's `short*`; the *old* report's "1-byte store" was a Q0/Q2 artifact.
6. **`%#o` decides on a stale `len`** — 0x4E50 `c.lw a4,0x4(s0)` (precision) vs 0x4E52
   `c.lw a5,0x10(s0)` = state+0x10, which is written only afterwards at 0x4E66 and never initialised
   (see summary). First-conversion `%#o` therefore uses stack garbage.
7. **`%hu` and `%ho` silently print all 32 bits** — 0x4DF8 `andi 0x40` + `beqz` jumps over the only
   deref instead of truncating to 16 (x/X *do* mask, 0x4EEE/0x4EF0; l/L are no-ops, which is correct
   for ILP32 where long is 32 bits).
8. **`_write_r` (0x5112) can never fail and never blocks on length** — 0x447E returns `len`
   unconditionally (0x4494 `c.mv a0,s2`), so the errno path at 0x5132-0x513E is dead and a UART that
   stops clocking hangs the CPU in 0x44A4 forever.
9. **`%a %A %b %B` and any unknown conversion are echoed as text** (0x4D1A/0x4D1E) and the argument is
   *not* consumed → silent va_list desync for `%b`/`%a`.
10. **`%n`/`%hn` store through an unvalidated pointer** taken from the format (0x4F1A `c.sw a1,0x0(a5)`,
    0x4F28 `sh a1,0x0(a5)`); no NULL/range check anywhere in 0x4CD6's 'n' arm (0x4F04-0x4F2A), and the
    'h' form still consumes a full 4-byte va_arg slot before storing only 2 bytes.
11. Length modifiers `hh/ll/z/j/t` do not exist: 0x4B1A `memchr("hlL",c,3)` takes exactly one char,
    `printf("%llx",1)` prints `lx` and consumes the argument as if `%x`.
12. `%.*` is **correct** (0x4AAE '.', 0x4AB8 `fmt[1]=='*'`, 0x4AC4 `addi s0,2` + one va_arg,
    negative → -1) — the old "va_list desync" finding is refuted.

Undecoded: 0x4FD6..0x501D and 0x504E..0x5053 are `db` in listing7 (Ghidra never disassembled them).
0x504E = `lh a1,0xe(a1)` + 2-byte tail transfer = `__sclose(fp)=_close_r(ptr,fp->_file)`.
0x4FD6 reads `_flags` (Q2 `lhu`, same halfword 0x25DE as the verified site at 0x5276), tests
`__SAPP` (0x4FE4 `andi 0x100`), calls `_lseek_r`..`_lseek(,0,SEEK_END)` (0x4FF6 `c.li a3,2`), masks
`_flags &= ~__SOFF` (0x4FFE-0x5004, `c.lui 0x1f;-1` = 0x1EFFF), then sets a0=reent, a1=`lh 0xe`=_file,
a2=buf, a3=n and tail-jumps at 0x501C (`dd a8`). That register setup matches `_write_r(ptr,fd,buf,n)`
exactly and 0x5112 is the only function of that shape and has no other caller → the target is
**_write_r 0x5112 (inferred; the 2-byte jump immediate is not decodable from listing7)**.