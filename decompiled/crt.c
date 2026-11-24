/* MSU2_MINI firmware 1.1 -- reconstructed C, module "crt".
 * Target: WCH CH32X033F8P6 (RISC-V RV32IMAC + WCH byte load/store extensions), 48 MHz, 64 KB flash (62 KB usable),
 * 20 KB SRAM at 0x20000000, 1 MB SPI NOR (P25D80), 0.96" 160x80 ST7735S panel.
 *
 * Reconstructed from the vendor image (24 224 bytes, flash 0x0000-0x5E9F) with Ghidra 12.2
 * (RISCV:LE:32:default) plus a patched riscv.rvc.sinc. The WCH core re-uses the RVC c.fld/c.fsd
 * encodings for byte loads/stores: quadrant 0 is decoded exactly (c.lbu/c.sb), quadrant 2
 * (c.lbu.sp/c.sb.sp, 16-bit lhu/sh) cannot be expressed in Sleigh 12.2 and is left as an
 * operand-free marker; those sites are listed per function below and in data/q2sites.txt.
 * Bodies are the decompiler output with symbol and register names substituted; nothing was
 * "cleaned up", so the original structure is intact. Symbols named sub_XXXX were not
 * identified; str_XXXX are C strings in flash. "extraout_x" values are incoming argument
 * registers that Ghidra lost track of across the -msave-restore prologue helper.
 */
#include "registers.h"
#include "strings.h"

/* ------------------------------------------------------------------
 * reset_entry   @ 0x0000
 * jump to _start
 * Signature: undefined thunk_FUN_ram_00003bb4()
 * ------------------------------------------------------------------ */
void thunk__start(void)

{
  undefined4 *puVar1;
  undefined4 *puVar2;
  
  gp = 0x20000880;
  puVar1 = &DAT_ram_00005df0;
  puVar2 = (undefined4 *)&DAT_ram_20000000;
  do {
    *puVar2 = *puVar1;
    puVar1 = puVar1 + 1;
    puVar2 = puVar2 + 1;
  } while (puVar2 < &g_bg_page);
  puVar1 = (undefined4 *)&g_bg_page;
  do {
    *puVar1 = 0;
    puVar1 = puVar1 + 1;
  } while (puVar1 < (undefined4 *)0x200011b0);
  _DAT_csreg_0bc0 = 0x1f;
  _DAT_csreg_0804 = 3;
  _mstatus = 0x88;
  _mtvec = 7;
  SystemInit();
  mepc = main;
  return;
}
