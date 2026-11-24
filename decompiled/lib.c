/* MSU2_MINI firmware 1.1 -- reconstructed C, module "lib".
 * Target: WCH CH32X033F8P6 (RISC-V RV32IMC + WCH byte load/store extensions), 48 MHz, 62 KB usable flash (64 KB nominal),
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
 * __riscv_save_13   @ 0x0100
 * -msave-restore prologue helper
 * Signature: undefined FUN_ram_00000100()
 * ------------------------------------------------------------------ */
void __riscv_save_13(void)

{
  return;
}

/* ------------------------------------------------------------------
 * __riscv_save_12   @ 0x0108
 * -msave-restore prologue helper
 * Signature: undefined FUN_ram_00000108()
 * ------------------------------------------------------------------ */
void __riscv_save_12(void)

{
  return;
}

/* ------------------------------------------------------------------
 * __riscv_save_8   @ 0x0116
 * -msave-restore prologue helper
 * Signature: undefined FUN_ram_00000116()
 * ------------------------------------------------------------------ */
void __riscv_save_8(void)

{
  return;
}

/* ------------------------------------------------------------------
 * __riscv_save_3   @ 0x0130
 * -msave-restore prologue helper
 * Signature: undefined FUN_ram_00000130()
 * ------------------------------------------------------------------ */
void __riscv_save_3(void)

{
  return;
}

/* ------------------------------------------------------------------
 * __riscv_restore_1   @ 0x013C
 * -msave-restore epilogue helper
 * Signature: undefined FUN_ram_0000013c()
 * ------------------------------------------------------------------ */
void __riscv_restore_1(void)

{
  return;
}

/* ------------------------------------------------------------------
 * memset   @ 0x0160
 * byte fill
 * Signature: undefined FUN_ram_00000160()
 * ------------------------------------------------------------------ */
void memset(uint *param_1,uint param_2,uint param_3)

{
  uint uVar1;
  uint extraout_t1;
  uint uVar2;
  uint extraout_a1;
  int extraout_a2;
  uint uVar3;
  uint *puVar4;
  int extraout_a4;
  int extraout_a5;
  
  uVar1 = 0xf;
  if (0xf < param_3) {
    if (((uint)param_1 & 0xf) != 0) {
      (*(code *)(((uint)param_1 & 0xf) * 4 + 0x196))();
      param_1 = (uint *)(extraout_a4 - (extraout_a5 + -0x10));
      param_3 = extraout_a2 + extraout_a5 + -0x10;
      uVar1 = extraout_t1;
      param_2 = extraout_a1;
      if (param_3 <= extraout_t1) goto LAB_ram_0000018a;
    }
    uVar2 = 0;
    if (param_2 != 0) {
      uVar2 = param_2 & 0xff | (param_2 & 0xff) << 8;
      uVar2 = uVar2 | uVar2 << 0x10;
    }
    uVar3 = param_3 & 0xfffffff0;
    param_3 = param_3 & 0xf;
    puVar4 = (uint *)(uVar3 + (int)param_1);
    do {
      *param_1 = uVar2;
      param_1[1] = uVar2;
      param_1[2] = uVar2;
      param_1[3] = uVar2;
      param_1 = param_1 + 4;
    } while (param_1 < puVar4);
    if (param_3 == 0) {
      return;
    }
  }
LAB_ram_0000018a:
                    /* WARNING: Could not recover jumptable at 0x00000196. Too many branches */
                    /* WARNING: Treating indirect jump as call */
  (*(code *)((uVar1 - param_3) * 4 + 0x19a))();
  return;
}

/* ------------------------------------------------------------------
 * lut_get   @ 0x0208
 * 8-bit lookup table accessor
 * Signature: undefined FUN_ram_00000208()
 * ------------------------------------------------------------------ */
undefined1 lut_get(uint param_1)

{
  undefined1 uVar1;
  
  if (param_1 < 0x100) {
    uVar1 = *(undefined1 *)(gp + -0x59c + param_1);
  }
  else {
    uVar1 = 0;
    if ((param_1 - 0x100 & 0xffff) < 0x100) {
      uVar1 = *(undefined1 *)(param_1 + 0x5618);
    }
  }
  return uVar1;
}

/* ------------------------------------------------------------------
 * memcpy6   @ 0x0238
 * fixed 6-byte copy
 * Signature: undefined FUN_ram_00000238()
 * ------------------------------------------------------------------ */
void memcpy6(int param_1,int param_2)

{
  undefined1 *puVar1;
  undefined1 *puVar2;
  int iVar3;
  
  iVar3 = 0;
  do {
    puVar1 = (undefined1 *)(param_2 + iVar3);
    puVar2 = (undefined1 *)(param_1 + iVar3);
    iVar3 = iVar3 + 1;
    *puVar2 = *puVar1;
  } while (iVar3 != 6);
  return;
}
