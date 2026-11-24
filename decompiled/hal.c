/* MSU2_MINI firmware 1.1 -- reconstructed C, module "hal".
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
 * DMA_DeInit   @ 0x3D7C
 * Signature: undefined FUN_ram_00003d7c()
 * ------------------------------------------------------------------ */
void DMA_DeInit(uint *param_1)

{
  uint uVar1;
  
  *param_1 = *param_1 & 0xfffe;
  *param_1 = 0;
  param_1[1] = 0;
  param_1[2] = 0;
  param_1[3] = 0;
  if (param_1 == (uint *)DMA1_CH1->CFGR) {
    DMA1->INTFCR = DMA1->INTFCR | 0xf;
  }
  else if (param_1 == (uint *)&DMA1_CH2->CFGR) {
    DMA1->INTFCR = DMA1->INTFCR | 0xf0;
  }
  else {
    if (param_1 == (uint *)&DMA1_CH3->CFGR) {
      uVar1 = 0xf00;
    }
    else if (param_1 == (uint *)DMA1_CH4->CFGR) {
      uVar1 = 0xf000;
    }
    else if (param_1 == (uint *)DMA1_CH5->CFGR) {
      uVar1 = 0xf0000;
    }
    else if (param_1 == (uint *)DMA1_CH6->CFGR) {
      uVar1 = 0xf00000;
    }
    else if (param_1 == (uint *)DMA1_CH7->CFGR) {
      uVar1 = 0xf000000;
    }
    else {
      if (param_1 != (uint *)DMA1_CH8->CFGR) {
        return;
      }
      uVar1 = 0xf0000000;
    }
    DMA1->INTFCR = DMA1->INTFCR | uVar1;
  }
  return;
}

/* ------------------------------------------------------------------
 * DMA_Init   @ 0x3E1E
 * per-channel CFGR/CNTR/PADDR/MADDR
 * Signature: undefined FUN_ram_00003e1e()
 * ------------------------------------------------------------------ */
void DMA_Init(uint *param_1,uint *param_2)

{
  *param_1 = param_2[2] | param_2[8] | param_2[4] | param_2[5] | param_2[6] | param_2[7] |
             param_2[9] | param_2[10] | *param_1 & 0xffff800f;
  param_1[1] = param_2[3];
  param_1[2] = *param_2;
  param_1[3] = param_2[1];
  return;
}

/* ------------------------------------------------------------------
 * DMA_Cmd   @ 0x3E56
 * Signature: undefined FUN_ram_00003e56()
 * ------------------------------------------------------------------ */
void DMA_Cmd(uint *param_1,int param_2)

{
  uint uVar1;
  
  if (param_2 == 0) {
    uVar1 = *param_1 & 0xfffe;
  }
  else {
    uVar1 = *param_1 | 1;
  }
  *param_1 = uVar1;
  return;
}

/* ------------------------------------------------------------------
 * DMA_GetFlagStatus   @ 0x3E6A
 * Signature: undefined FUN_ram_00003e6a()
 * ------------------------------------------------------------------ */
bool DMA_GetFlagStatus(uint param_1)

{
  return (param_1 & DMA1->INTFR) != 0;
}

/* ------------------------------------------------------------------
 * DMA_ClearFlag   @ 0x3E78
 * Signature: undefined FUN_ram_00003e78()
 * ------------------------------------------------------------------ */
void DMA_ClearFlag(undefined4 param_1)

{
  DMA1->INTFCR = param_1;
  return;
}

/* ------------------------------------------------------------------
 * GPIO_Init   @ 0x3E80
 * Signature: undefined FUN_ram_00003e80()
 * ------------------------------------------------------------------ */
void GPIO_Init(uint *param_1,uint *param_2)

{
  uint uVar1;
  uint uVar2;
  uint uVar3;
  uint uVar4;
  uint uVar5;
  uint uVar6;
  uint uVar7;
  
  uVar4 = param_2[2];
  uVar6 = uVar4 & 0xf;
  if ((uVar4 & 0x10) != 0) {
    uVar6 = uVar6 | param_2[1];
  }
  uVar3 = *param_2;
  uVar5 = 0;
  if ((uVar3 & 0xff) != 0) {
    uVar5 = *param_1;
    uVar1 = 0;
    do {
      uVar7 = 1 << (uVar1 & 0x1f);
      if (uVar7 == (uVar3 & uVar7)) {
        uVar5 = uVar6 << (uVar1 << 2 & 0x1f) | uVar5 & ~(0xf << (uVar1 << 2 & 0x1f));
        if (uVar4 == 0x28) {
          param_1[5] = uVar7;
        }
        else if (uVar4 == 0x48) {
          param_1[4] = uVar7;
        }
      }
      uVar1 = uVar1 + 1;
    } while (uVar1 != 8);
    *param_1 = uVar5;
  }
  if ((uVar3 & 0xff00) != 0) {
    if (param_1 == (uint *)GPIOA->CFGLR) {
      uVar5 = *(uint *)(gp + -0x7fc);
    }
    else if (param_1 == (uint *)GPIOB->CFGLR) {
      uVar5 = *(uint *)(gp + -0x7f8);
    }
    else if (param_1 == (uint *)GPIOC->CFGLR) {
      uVar5 = *(uint *)(gp + -0x7f4);
    }
    uVar1 = 8;
    do {
      uVar7 = 1 << (uVar1 & 0x1f);
      if (uVar7 == (uVar3 & uVar7)) {
        uVar2 = uVar1 * 4 - 0x20;
        uVar5 = ~(0xf << (uVar2 & 0x1f)) & uVar5 | uVar6 << (uVar2 & 0x1f);
        if (uVar4 == 0x28) {
          param_1[5] = uVar7;
        }
        else if (uVar4 == 0x48) {
          param_1[4] = uVar7;
        }
      }
      uVar1 = uVar1 + 1;
    } while (uVar1 != 0x10);
    param_1[1] = uVar5;
    if (param_1 == (uint *)GPIOA->CFGLR) {
      *(uint *)(gp + -0x7fc) = uVar5;
    }
    else if (param_1 == (uint *)GPIOB->CFGLR) {
      *(uint *)(gp + -0x7f8) = uVar5;
    }
    else if (param_1 == (uint *)GPIOC->CFGLR) {
      *(uint *)(gp + -0x7f4) = uVar5;
    }
  }
  if (0xffff < uVar3) {
    uVar5 = param_1[7];
    uVar1 = 0;
    do {
      uVar7 = 1 << (uVar1 + 0x10 & 0x1f);
      if (uVar7 == (uVar3 & uVar7)) {
        uVar5 = uVar6 << (uVar1 << 2 & 0x1f) | ~(0xf << (uVar1 << 2 & 0x1f)) & uVar5;
        if (uVar4 == 0x28) {
          param_1[5] = uVar7;
        }
        else if (uVar4 == 0x48) {
          param_1[8] = 1 << (uVar1 & 0x1f);
        }
      }
      uVar1 = uVar1 + 1;
    } while (uVar1 != 8);
    param_1[7] = uVar5;
  }
  return;
}

/* ------------------------------------------------------------------
 * GPIO_SetBits   @ 0x3FEE
 * Signature: undefined FUN_ram_00003fee()
 * ------------------------------------------------------------------ */
void GPIO_SetBits(int param_1,uint param_2)

{
  *(uint *)(param_1 + 0x10) = param_2 & 0xffff;
  *(uint *)(param_1 + 0x20) = param_2 >> 0x10;
  return;
}

/* ------------------------------------------------------------------
 * GPIO_WriteBit   @ 0x3FFC
 * Signature: undefined FUN_ram_00003ffc()
 * ------------------------------------------------------------------ */
void GPIO_WriteBit(int param_1,uint param_2,int param_3)

{
  if (param_3 == 0) {
    *(uint *)(param_1 + 0x14) = param_2;
    return;
  }
  *(uint *)(param_1 + 0x10) = param_2 & 0xffff;
  *(uint *)(param_1 + 0x20) = param_2 >> 0x10;
  return;
}

/* ------------------------------------------------------------------
 * write_once_flag   @ 0x4010
 * write-only global at gp-0x77C
 * Signature: undefined FUN_ram_00004010()
 * ------------------------------------------------------------------ */
void write_once_flag(undefined4 param_1)

{
  *(undefined4 *)(gp + -0x77c) = param_1;
  return;
}

/* ------------------------------------------------------------------
 * PWR_SetPLS   @ 0x4016
 * Signature: undefined FUN_ram_00004016()
 * ------------------------------------------------------------------ */
void PWR_SetPLS(uint param_1)

{
  AWU->CSR = param_1 | AWU->CSR & 0xffffff9f;
  return;
}

/* ------------------------------------------------------------------
 * PWR_GetStatus   @ 0x4026
 * Signature: undefined FUN_ram_00004026()
 * ------------------------------------------------------------------ */
bool PWR_GetStatus(uint param_1)

{
  return (param_1 & AWU->WR) != 0;
}

/* ------------------------------------------------------------------
 * pvd_probe   @ 0x4034
 * brown-out/PVD strapping probe
 * Signature: undefined FUN_ram_00004034()
 * ------------------------------------------------------------------ */
bool pvd_probe(void)

{
  int iVar1;
  
  __riscv_save_3();
  Delay_Init();
  RCC_APB1PeriphClockCmd(0x10000000,1);
  PWR_SetPLS(0x60);
  delay_cycles(10);
  iVar1 = PWR_GetStatus(4);
  PWR_SetPLS(0);
  return iVar1 != 0;
}

/* ------------------------------------------------------------------
 * RCC_GetClocksFreq   @ 0x4064
 * wrong clock model
 * Signature: undefined FUN_ram_00004064()
 * ------------------------------------------------------------------ */
void RCC_GetClocksFreq(undefined4 *param_1)

{
  uint uVar1;
  uint uVar2;
  
  *param_1 = 48000000;
  uVar1 = (uint)*(byte *)((RCC->CFGR0 >> 4 & 0xf) + 0x20000010);
  uVar2 = 48000000 >> (uVar1 & 0x1f);
  if ((RCC->CFGR0 & 0x80) == 0) {
    if (uVar1 == 0) {
      uVar2 = 0xffffffff;
    }
    else {
      uVar2 = 48000000 / uVar1;
    }
  }
  param_1[1] = uVar2;
  param_1[2] = uVar2;
  param_1[3] = uVar2;
  return;
}

/* ------------------------------------------------------------------
 * RCC_AHBPeriphClockCmd   @ 0x40A2
 * Signature: undefined FUN_ram_000040a2()
 * ------------------------------------------------------------------ */
void RCC_AHBPeriphClockCmd(uint param_1,int param_2)

{
  if (param_2 == 0) {
    RCC->AHBPCENR = ~param_1 & RCC->AHBPCENR;
    return;
  }
  RCC->AHBPCENR = param_1 | RCC->AHBPCENR;
  return;
}

/* ------------------------------------------------------------------
 * RCC_APB2PeriphClockCmd   @ 0x40C0
 * Signature: undefined FUN_ram_000040c0()
 * ------------------------------------------------------------------ */
void RCC_APB2PeriphClockCmd(uint param_1,int param_2)

{
  if (param_2 == 0) {
    RCC->APB2PCENR = ~param_1 & RCC->APB2PCENR;
    return;
  }
  RCC->APB2PCENR = param_1 | RCC->APB2PCENR;
  return;
}

/* ------------------------------------------------------------------
 * RCC_APB1PeriphClockCmd   @ 0x40DE
 * Signature: undefined FUN_ram_000040de()
 * ------------------------------------------------------------------ */
void RCC_APB1PeriphClockCmd(uint param_1,int param_2)

{
  if (param_2 == 0) {
    RCC->APB1PCENR = ~param_1 & RCC->APB1PCENR;
    return;
  }
  RCC->APB1PCENR = param_1 | RCC->APB1PCENR;
  return;
}

/* ------------------------------------------------------------------
 * RCC_APB2PeriphResetCmd   @ 0x40FC
 * Signature: undefined FUN_ram_000040fc()
 * ------------------------------------------------------------------ */
void RCC_APB2PeriphResetCmd(uint param_1,int param_2)

{
  if (param_2 == 0) {
    RCC->APB2PRSTR = ~param_1 & RCC->APB2PRSTR;
    return;
  }
  RCC->APB2PRSTR = param_1 | RCC->APB2PRSTR;
  return;
}

/* ------------------------------------------------------------------
 * RCC_APB1PeriphResetCmd   @ 0x411A
 * Signature: undefined FUN_ram_0000411a()
 * ------------------------------------------------------------------ */
void RCC_APB1PeriphResetCmd(uint param_1,int param_2)

{
  if (param_2 == 0) {
    RCC->APB1PRSTR = ~param_1 & RCC->APB1PRSTR;
    return;
  }
  RCC->APB1PRSTR = param_1 | RCC->APB1PRSTR;
  return;
}

/* ------------------------------------------------------------------
 * SPI1_CTLR1_merge   @ 0x4138
 * Signature: undefined FUN_ram_00004138()
 *
 * WCH 16-bit load/store sites (quadrant-2 custom opcodes, invisible to the decompiler):
 *   0x4138  lhu a4,0x0(a0)
 *   0x4140  lhu a3,0x2(a1)
 *   0x4144  lhu a5,0x0(a1)
 *   0x4148  lhu a3,0x4(a1)
 *   0x414C  lhu a3,0x6(a1)
 *   0x4150  lhu a3,0x8(a1)
 *   0x4154  lhu a3,0xa(a1)
 *   0x4158  lhu a3,0xc(a1)
 *   0x415C  lhu a3,0xe(a1)
 *   0x4162  sh a5,0x0(a0)
 *   0x4164  lhu a5,0x10(a1)
 *   0x4166  sh a5,0x10(a0)
 * ------------------------------------------------------------------ */
void SPI1_CTLR1_merge(void)

{
  return;
}

/* ------------------------------------------------------------------
 * SPI1_Cmd   @ 0x416A
 * Signature: undefined FUN_ram_0000416a()
 *
 * WCH 16-bit load/store sites (quadrant-2 custom opcodes, invisible to the decompiler):
 *   0x416A  lhu a5,0x0(a0)
 *   0x4172  sh a5,0x0(a0)
 * ------------------------------------------------------------------ */
void SPI1_Cmd(void)

{
  return;
}

/* ------------------------------------------------------------------
 * SPI1_CTLR1_write   @ 0x4184
 * Signature: undefined FUN_ram_00004184()
 *
 * WCH 16-bit load/store sites (quadrant-2 custom opcodes, invisible to the decompiler):
 *   0x4184  lhu a5,0x4(a0)
 *   0x418A  sh a1,0x4(a0)
 * ------------------------------------------------------------------ */
void SPI1_CTLR1_write(void)

{
  return;
}

/* ------------------------------------------------------------------
 * SPI1_SendData   @ 0x4196
 * Signature: undefined FUN_ram_00004196()
 *
 * WCH 16-bit load/store sites (quadrant-2 custom opcodes, invisible to the decompiler):
 *   0x4196  sh a1,0xc(a0)
 * ------------------------------------------------------------------ */
void SPI1_SendData(void)

{
  return;
}

/* ------------------------------------------------------------------
 * SPI1_ReceiveData   @ 0x419A
 * Signature: undefined FUN_ram_0000419a()
 *
 * WCH 16-bit load/store sites (quadrant-2 custom opcodes, invisible to the decompiler):
 *   0x419A  lhu a0,0xc(a0)
 * ------------------------------------------------------------------ */
void SPI1_ReceiveData(void)

{
  return;
}

/* ------------------------------------------------------------------
 * SPI1_GetFlagStatus   @ 0x419E
 * Signature: undefined FUN_ram_0000419e()
 *
 * WCH 16-bit load/store sites (quadrant-2 custom opcodes, invisible to the decompiler):
 *   0x419E  lhu a0,0x8(a0)
 * ------------------------------------------------------------------ */
bool SPI1_GetFlagStatus(uint param_1,uint param_2)

{
  return (param_1 & param_2) != 0;
}

/* ------------------------------------------------------------------
 * sub_41A8   @ 0x41A8
 * Signature: undefined FUN_ram_000041a8()
 * ------------------------------------------------------------------ */
void sub_41A8(void)

{
  int iVar1;
  undefined4 uVar2;
  
  iVar1 = __riscv_save_3();
  if (iVar1 == TIM1->CTLR1) {
    RCC_APB2PeriphResetCmd(0x800,1);
    RCC_APB2PeriphResetCmd(0x800,0);
  }
  else {
    if (iVar1 == TIM2->CTLR1) {
      RCC_APB1PeriphResetCmd(1,1);
      uVar2 = 1;
    }
    else {
      if (iVar1 != TIM3->CTLR1) {
        return;
      }
      RCC_APB1PeriphResetCmd(2,1);
      uVar2 = 2;
    }
    RCC_APB1PeriphResetCmd(uVar2,0);
  }
  return;
}

/* ------------------------------------------------------------------
 * SPI1_I2SCFGR?   @ 0x4202
 * Signature: undefined FUN_ram_00004202()
 *
 * WCH 16-bit load/store sites (quadrant-2 custom opcodes, invisible to the decompiler):
 *   0x4202  lhu a5,0x0(a0)
 *   0x421C  lhu a4,0x2(a1)
 *   0x4224  lhu a4,0x6(a1)
 *   0x4230  sh a5,0x0(a0)
 *   0x4232  lhu a5,0x4(a1)
 *   0x4234  sh a5,0x2c(a0)
 *   0x4236  lhu a5,0x0(a1)
 *   0x4238  sh a5,0x28(a0)
 *   0x4250  sh a5,0x30(a0)
 *   0x4254  sh a5,0x14(a0)
 * ------------------------------------------------------------------ */
void SPI1_I2SCFGR?(void)

{
  return;
}

/* ------------------------------------------------------------------
 * SPI1_GetFlagStatus2?   @ 0x4258
 * Signature: undefined FUN_ram_00004258()
 *
 * WCH 16-bit load/store sites (quadrant-2 custom opcodes, invisible to the decompiler):
 *   0x4258  lhu a5,0x0(a0)
 *   0x4260  sh a5,0x0(a0)
 * ------------------------------------------------------------------ */
void SPI1_GetFlagStatus2?(void)

{
  return;
}

/* ------------------------------------------------------------------
 * spi_clear_bit   @ 0x4270
 * Signature: undefined FUN_ram_00004270()
 *
 * WCH 16-bit load/store sites (quadrant-2 custom opcodes, invisible to the decompiler):
 *   0x4270  lhu a5,0xc(a0)
 *   0x4276  sh a1,0xc(a0)
 * ------------------------------------------------------------------ */
void spi_clear_bit(void)

{
  return;
}

/* ------------------------------------------------------------------
 * spi_set_bit?   @ 0x4282
 * Signature: undefined FUN_ram_00004282()
 *
 * WCH 16-bit load/store sites (quadrant-2 custom opcodes, invisible to the decompiler):
 *   0x428A  sh a1,0x10(a0)
 * ------------------------------------------------------------------ */
void spi_set_bit?(void)

{
  return;
}

/* ------------------------------------------------------------------
 * void_428e   @ 0x428E
 * Signature: undefined FUN_ram_0000428e()
 *
 * WCH 16-bit load/store sites (quadrant-2 custom opcodes, invisible to the decompiler):
 *   0x428E  lhu a5,0x10(a0)
 *   0x4290  lhu a0,0xc(a0)
 * ------------------------------------------------------------------ */
bool void_428e(uint param_1,uint param_2)

{
  uint in_a5;
  
  if ((in_a5 & param_2) == 0) {
    return false;
  }
  return (param_1 & 0xffff & param_2) != 0;
}

/* ------------------------------------------------------------------
 * void_42a6   @ 0x42A6
 * Signature: undefined FUN_ram_000042a6()
 *
 * WCH 16-bit load/store sites (quadrant-2 custom opcodes, invisible to the decompiler):
 *   0x42AE  sh a1,0x10(a0)
 * ------------------------------------------------------------------ */
void void_42a6(void)

{
  return;
}

/* ------------------------------------------------------------------
 * USART_Init   @ 0x42B2
 * baud divisor from a broken PCLK1
 * Signature: undefined FUN_ram_000042b2()
 *
 * WCH 16-bit load/store sites (quadrant-2 custom opcodes, invisible to the decompiler):
 *   0x42B6  lhu a3,0x10(a0)
 *   0x42BE  lhu a3,0x6(a1)
 *   0x42C0  lhu a4,0xc(a1)
 *   0x42C6  sh a5,0x10(a0)
 *   0x42C8  lhu a3,0xc(a0)
 *   0x42D2  lhu a3,0x4(a1)
 *   0x42DA  lhu a3,0x8(a1)
 *   0x42DE  lhu a3,0xa(a1)
 *   0x42E2  sh a5,0xc(a0)
 *   0x42E4  lhu a5,0x14(a0)
 *   0x42F0  sh a5,0x14(a0)
 *   0x4336  sh a5,0x8(s0)
 * ------------------------------------------------------------------ */
void USART_Init(void)

{
  undefined1 auStack_10 [16];
  
  __riscv_save_3();
  RCC_GetClocksFreq(auStack_10);
  return;
}

/* ------------------------------------------------------------------
 * USART_Cmd   @ 0x4342
 * Signature: undefined FUN_ram_00004342()
 *
 * WCH 16-bit load/store sites (quadrant-2 custom opcodes, invisible to the decompiler):
 *   0x4344  lhu a5,0xc(a0)
 *   0x434A  sh a5,0xc(a0)
 *   0x434E  lhu a4,0xc(a0)
 * ------------------------------------------------------------------ */
void USART_Cmd(void)

{
  return;
}

/* ------------------------------------------------------------------
 * USART_SendData8?   @ 0x4358
 * Signature: undefined FUN_ram_00004358()
 *
 * WCH 16-bit load/store sites (quadrant-2 custom opcodes, invisible to the decompiler):
 *   0x4358  lhu a5,0x14(a0)
 *   0x435E  sh a1,0x14(a0)
 * ------------------------------------------------------------------ */
void USART_SendData8?(void)

{
  return;
}

/* ------------------------------------------------------------------
 * USART_SendData   @ 0x436A
 * Signature: undefined FUN_ram_0000436a()
 *
 * WCH 16-bit load/store sites (quadrant-2 custom opcodes, invisible to the decompiler):
 *   0x436E  sh a1,0x4(a0)
 * ------------------------------------------------------------------ */
void USART_SendData(void)

{
  return;
}

/* ------------------------------------------------------------------
 * USART_GetFlagStatus   @ 0x4372
 * Signature: undefined FUN_ram_00004372()
 *
 * WCH 16-bit load/store sites (quadrant-2 custom opcodes, invisible to the decompiler):
 *   0x4372  lhu a0,0x0(a0)
 * ------------------------------------------------------------------ */
bool USART_GetFlagStatus(uint param_1,uint param_2)

{
  return (param_1 & param_2) != 0;
}

/* ------------------------------------------------------------------
 * USART_ClearFlag   @ 0x437C
 * Signature: undefined FUN_ram_0000437c()
 *
 * WCH 16-bit load/store sites (quadrant-2 custom opcodes, invisible to the decompiler):
 *   0x4384  sh a1,0x0(a0)
 * ------------------------------------------------------------------ */
void USART_ClearFlag(void)

{
  return;
}

/* ------------------------------------------------------------------
 * Delay_Init   @ 0x4388
 * loop calibration from SystemCoreClock
 * Signature: undefined FUN_ram_00004388()
 * ------------------------------------------------------------------ */
void Delay_Init(void)

{
  uint uVar1;
  
  uVar1 = SystemCoreClock / 8000000;
  *(char *)(gp + -0x776) = (char)uVar1;
  *(ushort *)(gp + -0x778) = ((ushort)uVar1 & 0xff) * 1000;
  return;
}

/* ------------------------------------------------------------------
 * delay_cycles   @ 0x43B2
 * Signature: undefined FUN_ram_000043b2()
 * ------------------------------------------------------------------ */
void delay_cycles(void)

{
                    /* WARNING: Do nothing block with infinite loop */
  do {
  } while( true );
}

/* ------------------------------------------------------------------
 * Delay_Ms   @ 0x43E8
 * Signature: undefined FUN_ram_000043e8()
 * ------------------------------------------------------------------ */
void Delay_Ms(void)

{
                    /* WARNING: Do nothing block with infinite loop */
  do {
  } while( true );
}

/* ------------------------------------------------------------------
 * USART2_Init   @ 0x441E
 * 115200 8N1
 * Signature: undefined FUN_ram_0000441e()
 * ------------------------------------------------------------------ */
void USART2_Init(void)

{
  undefined4 uVar1;
  undefined4 uStack_1c;
  undefined4 uStack_18;
  undefined4 uStack_14;
  undefined4 uStack_10;
  undefined4 uStack_c;
  undefined4 uStack_8;
  undefined2 uStack_4;
  
  uVar1 = __riscv_save_3();
  RCC_APB1PeriphClockCmd(0x20000,1);
  RCC_APB2PeriphClockCmd(4,1);
  uStack_1c = 4;
  uStack_18 = 1;
  uStack_14 = 0x18;
  GPIO_Init(GPIOA->CFGLR,&uStack_1c);
  uStack_8 = 0x80000;
  uStack_c = 0;
  uStack_4 = 0;
  uStack_10 = uVar1;
  USART_Init(USART2->STATR,&uStack_10);
  USART_Cmd(USART2->STATR,1);
  return;
}

/* ------------------------------------------------------------------
 * usart_poll_write   @ 0x447E
 * blocking TXE poll per byte
 * Signature: undefined FUN_ram_0000447e()
 * ------------------------------------------------------------------ */
int usart_poll_write(void)

{
  int iVar1;
  int iVar2;
  int extraout_a1;
  int extraout_a2;
  
  __riscv_save_8();
  for (iVar1 = 0; iVar1 < extraout_a2; iVar1 = iVar1 + 1) {
    do {
      iVar2 = USART_GetFlagStatus(USART2->STATR,0x40);
    } while (iVar2 == 0);
    USART_SendData(USART2->STATR,(short)*(char *)(extraout_a1 + iVar1));
  }
  return extraout_a2;
}

/* ------------------------------------------------------------------
 * DMA_M2M_Copy   @ 0x44BC
 * Signature: undefined FUN_ram_000044bc()
 * ------------------------------------------------------------------ */
uint DMA_M2M_Copy(int param_1)

{
  uint uVar1;
  uint uVar2;
  
  uVar2 = *(uint *)(gp + -0x7f0);
  uVar1 = param_1 + uVar2;
  if ((uVar1 < 0x200011b0) || (0x20004800 < uVar1)) {
    uVar2 = 0xffffffff;
  }
  else {
    *(uint *)(gp + -0x7f0) = uVar1;
  }
  return uVar2;
}
