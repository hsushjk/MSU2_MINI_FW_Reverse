/* MSU2_MINI firmware 1.1 -- reconstructed C, module "boot".
 * Target: WCH CH32X033F8P6 (RISC-V RV32IMC + WCH byte load/store extensions), 48 MHz, 64 KB flash,
 * 20 KB SRAM at 0x20000000, 1 MB SPI NOR (P25D80), 0.96" 160x80 ST7735S panel.
 *
 * Reconstructed from the vendor image (24 224 bytes, flash 0x0000-0x5E9F) with Ghidra 12.2
 * (RISCV:LE:32:default) plus a patched riscv.rvc.sinc. The WCH core re-uses the RVC c.fld/c.fsd
 * encodings for byte loads/stores: quadrant 0 is decoded exactly (c.lbu/c.sb), quadrant 2
 * (c.lbu.sp/c.sb.sp, 16-bit lhu/sh) cannot be expressed in Sleigh 12.2 and is left as an
 * operand-free marker; those sites are listed per function below and in work/q2sites.txt.
 * Bodies are the decompiler output with symbol and register names substituted; nothing was
 * "cleaned up", so the original structure is intact. Symbols named sub_XXXX were not
 * identified; str_XXXX are C strings in flash. "extraout_x" values are incoming argument
 * registers that Ghidra lost track of across the -msave-restore prologue helper.
 */
#include "registers.h"
#include "strings.h"

/* ------------------------------------------------------------------
 * SystemInit   @ 0x3B32
 * HSION + flash wait state
 * Signature: undefined FUN_ram_00003b32()
 * ------------------------------------------------------------------ */
void SystemInit(void)

{
  RCC->CTLR = RCC->CTLR | 1;
  FLASH_R->ACTLR = FLASH_R->ACTLR & 0xfffffffc | 2;
  RCC->CFGR0 = RCC->CFGR0 & 0xf8ffff0f;
  return;
}

/* ------------------------------------------------------------------
 * SystemCoreClockUpdate   @ 0x3B74
 * Signature: undefined FUN_ram_00003b74()
 * ------------------------------------------------------------------ */
void SystemCoreClockUpdate(void)

{
  uint uVar1;
  
  uVar1 = (uint)*(byte *)((RCC->CFGR0 >> 4 & 0xf) + 0x20000000);
  SystemCoreClock = 48000000 >> (uVar1 & 0x1f);
  if ((RCC->CFGR0 & 0x80) == 0) {
    if (uVar1 == 0) {
      SystemCoreClock = 0xffffffff;
    }
    else {
      SystemCoreClock = 48000000 / uVar1;
    }
  }
  return;
}

/* ------------------------------------------------------------------
 * _start   @ 0x3BB4
 * crt0: gp/sp, .data/.bss, CSRs, mret into main
 * Signature: undefined FUN_ram_00003bb4()
 * ------------------------------------------------------------------ */
void _start(void)

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

/* ------------------------------------------------------------------
 * ADC_Init   @ 0x3C3E
 * Signature: undefined FUN_ram_00003c3e()
 * ------------------------------------------------------------------ */
void ADC_Init(int param_1,uint *param_2)

{
  *(uint *)(param_1 + 4) =
       *param_2 | param_2[6] | param_2[7] | param_2[1] << 8 | *(uint *)(param_1 + 4) & 0xe0f0feff;
  *(uint *)(param_1 + 8) =
       param_2[4] | param_2[3] | param_2[2] << 1 | *(uint *)(param_1 + 8) & 0xfff1f7fd;
  *(uint *)(param_1 + 0x2c) =
       ((byte)param_2[1] - 1 & 0xff) << 0x14 | *(uint *)(param_1 + 0x2c) & 0xff0fffff;
  return;
}

/* ------------------------------------------------------------------
 * ADC_Cmd   @ 0x3C94
 * Signature: undefined FUN_ram_00003c94()
 * ------------------------------------------------------------------ */
void ADC_Cmd(int param_1,int param_2)

{
  uint uVar1;
  
  if (param_2 == 0) {
    uVar1 = *(uint *)(param_1 + 8) & 0xfffffffe;
  }
  else {
    uVar1 = *(uint *)(param_1 + 8) | 1;
  }
  *(uint *)(param_1 + 8) = uVar1;
  return;
}

/* ------------------------------------------------------------------
 * ADC_GetConversionValue   @ 0x3CA4
 * Signature: undefined FUN_ram_00003ca4()
 * ------------------------------------------------------------------ */
void ADC_GetConversionValue(int param_1,uint param_2,uint param_3,int param_4)

{
  uint uVar1;
  
  if (param_2 < 10) {
    *(uint *)(param_1 + 0x10) =
         param_4 << (param_2 * 3 & 0x1f) | ~(7 << (param_2 * 3 & 0x1f)) & *(uint *)(param_1 + 0x10);
  }
  else {
    uVar1 = (param_2 - 10) * 3;
    *(uint *)(param_1 + 0xc) =
         param_4 << (uVar1 & 0x1f) | ~(7 << (uVar1 & 0x1f)) & *(uint *)(param_1 + 0xc);
  }
  if (param_3 < 7) {
    uVar1 = (param_3 - 1) * 5;
    *(uint *)(param_1 + 0x34) =
         param_2 << (uVar1 & 0x1f) | ~(0x1f << (uVar1 & 0x1f)) & *(uint *)(param_1 + 0x34);
    return;
  }
  if (param_3 < 0xd) {
    uVar1 = (param_3 - 7) * 5;
    *(uint *)(param_1 + 0x30) =
         param_2 << (uVar1 & 0x1f) | ~(0x1f << (uVar1 & 0x1f)) & *(uint *)(param_1 + 0x30);
    return;
  }
  uVar1 = (param_3 - 0xd) * 5;
  *(uint *)(param_1 + 0x2c) =
       param_2 << (uVar1 & 0x1f) | ~(0x1f << (uVar1 & 0x1f)) & *(uint *)(param_1 + 0x2c);
  return;
}

/* ------------------------------------------------------------------
 * sub_3D5C   @ 0x3D5C
 * Signature: undefined FUN_ram_00003d5c()
 * ------------------------------------------------------------------ */
bool sub_3D5C(uint *param_1,uint param_2)

{
  return (*param_1 & param_2) != 0;
}

/* ------------------------------------------------------------------
 * sub_3D66   @ 0x3D66
 * Signature: undefined FUN_ram_00003d66()
 * ------------------------------------------------------------------ */
void sub_3D66(int param_1,uint param_2)

{
  *(uint *)(param_1 + 0x50) = param_2 | *(uint *)(param_1 + 0x50) & 0xfffffe00;
  return;
}

/* ------------------------------------------------------------------
 * GetChipID   @ 0x3D72
 * unique ID register
 * Signature: undefined FUN_ram_00003d72()
 * ------------------------------------------------------------------ */
undefined4 GetChipID(void)

{
  return _DAT_ram_1ffff704;
}
