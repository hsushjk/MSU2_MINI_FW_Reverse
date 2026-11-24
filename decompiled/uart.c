/* MSU2_MINI firmware 1.1 -- reconstructed C, module "uart".
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
 * board_clocks_enable   @ 0x0A90
 * RCC AHB/APB clock enables for the board
 * Signature: undefined FUN_ram_00000a90()
 * ------------------------------------------------------------------ */
undefined4 board_clocks_enable(void)

{
  __riscv_save_3();
  RCC_APB2PeriphClockCmd(4,1);
  RCC_APB1PeriphClockCmd(0x20000,1);
  RCC_APB1PeriphClockCmd(2,1);
  RCC_AHBPeriphClockCmd(1,1);
  return 0;
}

/* ------------------------------------------------------------------
 * tim3_init   @ 0x0ABC
 * TIM3 1 kHz tick setup
 * Signature: undefined FUN_ram_00000abc()
 * ------------------------------------------------------------------ */
void tim3_init(void)

{
  uint auStack_c [2];
  undefined2 uStack_4;
  
  __riscv_save_3();
  auStack_c[0] = 0;
  auStack_c[1] = 0;
  uStack_4 = 0;
  sub_41A8(TIM3->CTLR1);
  auStack_c[0] = auStack_c[0] & 0xffff;
  auStack_c[1] = 999;
  SPI1_I2SCFGR?(TIM3->CTLR1,auStack_c);
  spi_set_bit?(TIM3->CTLR1,1);
  spi_clear_bit(TIM3->CTLR1,1,1);
  NVIC->ISER[1] = 0x400000;
  SPI1_GetFlagStatus2?(TIM3->CTLR1,1);
  return;
}

/* ------------------------------------------------------------------
 * uart2_gpio_init   @ 0x0B34
 * USART2 pins
 * Signature: undefined FUN_ram_00000b34()
 * ------------------------------------------------------------------ */
void uart2_gpio_init(void)

{
  int iVar1;
  int extraout_a2;
  undefined8 uVar2;
  undefined4 uStack_1c;
  undefined4 uStack_18;
  undefined4 uStack_14;
  undefined4 uStack_10;
  undefined4 uStack_c;
  undefined4 uStack_8;
  uint uStack_4;
  
  uVar2 = __riscv_save_8();
  iVar1 = (int)((ulonglong)uVar2 >> 0x20);
  uStack_10 = 0;
  uStack_c = 0;
  uStack_8 = 0;
  uStack_4 = 0;
  uStack_1c = 0;
  uStack_18 = 0;
  uStack_14 = 0;
  GPIO_SetBits(GPIOA->CFGLR,4);
  uStack_1c = 4;
  uStack_18 = 1;
  uStack_14 = 0x10;
  GPIO_Init(GPIOA->CFGLR,&uStack_1c);
  USART2->CTLR1 = USART2->CTLR1 & 0xfff3;
  uStack_1c = 8;
  uStack_14 = 0x48;
  GPIO_Init(GPIOA->CFGLR,&uStack_1c);
  uStack_14 = 0x18;
  uStack_1c = 4;
  uStack_18 = 1;
  GPIO_Init(GPIOA->CFGLR,&uStack_1c);
  uStack_1c = 0x8000;
  uStack_18 = 1;
  uStack_14 = 0x10;
  GPIO_Init(GPIOA->CFGLR,&uStack_1c);
  uStack_c._2_2_ = 0x3000;
  if (iVar1 != 1) {
    if (iVar1 == 2) {
      uStack_c._2_2_ = 0x2000;
    }
    else {
      uStack_c._2_2_ = 0;
    }
  }
  uStack_c._0_2_ = 0;
  if (extraout_a2 == 1) {
    uStack_8._0_2_ = 0x600;
  }
  else {
    if (extraout_a2 != 2) {
      uStack_8._0_2_ = 0;
      goto LAB_ram_0000014a;
    }
    uStack_8._0_2_ = 0x400;
  }
  uStack_c._0_2_ = 0x1000;
LAB_ram_0000014a:
  uStack_8 = CONCAT22(0xc,(undefined2)uStack_8);
  uStack_4 = uStack_4 & 0xffff0000;
  uStack_10 = (int)uVar2;
  USART_Init(USART2->STATR,&uStack_10);
  USART_ClearFlag(USART2->STATR,0x40);
  USART_Cmd(USART2->STATR,1);
  return;
}

/* ------------------------------------------------------------------
 * usb_ep_flags_clear   @ 0x0C3A
 * clear per-endpoint busy flags
 * Signature: undefined FUN_ram_00000c3a()
 * ------------------------------------------------------------------ */
void usb_ep_flags_clear(int param_1)

{
  int iVar1;
  
  g_rxq = 0;
  *(undefined2 *)(gp + 0x766) = 0;
  *(undefined2 *)(gp + 0x768) = 0;
  *(undefined1 *)(gp + 0x76a) = 0;
  *(undefined1 *)(gp + 0x76b) = 0x1e;
  *(undefined2 *)(gp + 0x76c) = 0;
  *(undefined2 *)(gp + 0x76e) = 0;
  *(undefined2 *)(gp + 0x770) = 0;
  iVar1 = 0;
  do {
    *(undefined2 *)(iVar1 * 2 + gp + 0x772) = 0;
    iVar1 = iVar1 + 1;
  } while (iVar1 != 0x10);
  *(undefined1 *)(gp + 0x792) = 0;
  *(undefined2 *)(gp + 0x794) = 0;
  *(undefined2 *)(gp + 0x796) = 0;
  *(undefined1 *)(gp + 0x798) = 0;
  *(undefined2 *)(gp + 0x79a) = 0;
  *(undefined1 *)(gp + 0x79c) = 0;
  *(undefined1 *)(gp + 0x79d) = 0;
  *(undefined4 *)(gp + -0x7a4) = 0;
  *(undefined4 *)(gp + -0x7a0) = 0;
  if (param_1 != 0) {
    *(undefined1 *)(gp + 0x79e) = 0;
    *(undefined1 *)(gp + 0x79f) = 0xc2;
    *(undefined1 *)(gp + 0x7a0) = 1;
    *(undefined1 *)(gp + 0x7a1) = 0;
    *(undefined1 *)(gp + 0x7a2) = 0;
    *(undefined1 *)(gp + 0x7a3) = 0;
    *(undefined1 *)(gp + 0x7a4) = 8;
    *(undefined1 *)(gp + 0x7a5) = 0x1e;
  }
  return;
}

/* ------------------------------------------------------------------
 * uart_dma_rx_setup   @ 0x0CC8
 * DMA1 CH6/CH7 for USART2
 * Signature: undefined FUN_ram_00000cc8()
 * ------------------------------------------------------------------ */
void uart_dma_rx_setup(void)

{
  undefined4 uVar1;
  undefined8 uVar2;
  undefined4 uStack_2c;
  undefined4 uStack_28;
  undefined4 uStack_24;
  undefined4 uStack_1c;
  undefined4 uStack_18;
  undefined4 uStack_14;
  undefined4 uStack_10;
  undefined4 uStack_c;
  undefined4 uStack_8;
  undefined4 uStack_4;
  
  uVar2 = __riscv_save_3();
  uVar1 = (undefined4)((ulonglong)uVar2 >> 0x20);
  memset(&uStack_2c,0,0x2c);
  uStack_28 = uVar1;
  if ((int)uVar2 == 0) {
    DMA_DeInit(DMA1_CH7->CFGR);
    uStack_2c = USART2->DATAR;
    uStack_24 = 0x10;
    uStack_18 = 0x80;
    uStack_1c = 0;
    uStack_14 = 0;
    uStack_10 = 0;
    uStack_c = 0;
    uStack_8 = 0x1000;
    uStack_4 = 0;
    DMA_Init(DMA1_CH7->CFGR,&uStack_2c);
    uVar1 = DMA1_CH7->CFGR;
  }
  else {
    DMA_DeInit(DMA1_CH6->CFGR);
    uStack_2c = USART2->DATAR;
    uStack_18 = 0x80;
    uStack_c = 0x20;
    uStack_24 = 0;
    uStack_1c = 0;
    uStack_14 = 0;
    uStack_10 = 0;
    uStack_8 = 0x1000;
    uStack_4 = 0;
    DMA_Init(DMA1_CH6->CFGR,&uStack_2c);
    uVar1 = DMA1_CH6->CFGR;
  }
  DMA_Cmd(uVar1,1);
  return;
}

/* ------------------------------------------------------------------
 * uart_dma_reconfig   @ 0x0D74
 * Signature: undefined FUN_ram_00000d74()
 * ------------------------------------------------------------------ */
void uart_dma_reconfig(void)

{
  undefined4 extraout_a2;
  undefined4 extraout_a3;
  undefined8 uVar1;
  
  uVar1 = __riscv_save_8();
  USART_SendData8?(USART2->STATR,0x40,0);
  DMA_Cmd(DMA1_CH6->CFGR,0);
  DMA_Cmd(DMA1_CH7->CFGR,0);
  uart2_gpio_init((int)((ulonglong)uVar1 >> 0x20),extraout_a2,extraout_a3);
  uart_dma_rx_setup(0,gp + 0x364,0);
  uart_dma_rx_setup(1,0x200003e4,0x800);
  USART_SendData8?(USART2->STATR,0x40,1);
  usb_ep_flags_clear((int)uVar1);
  return;
}

/* ------------------------------------------------------------------
 * usb_uart_reinit   @ 0x0DEA
 * UART reconfiguration called from the USB ISR
 * Signature: undefined FUN_ram_00000dea()
 * ------------------------------------------------------------------ */
void usb_uart_reinit(void)

{
  __riscv_save_3();
  uart_dma_reconfig(0,(uint)*(byte *)(gp + 0x79f) * 0x100 +
                     (uint)*(byte *)(gp + 0x7a1) * 0x1000000 + (uint)*(byte *)(gp + 0x7a0) * 0x10000
                     + (uint)*(byte *)(gp + 0x79e),*(undefined1 *)(gp + 0x7a2),
                   *(undefined1 *)(gp + 0x7a3));
  USBFSD->UEP2_DMA = gp + 0x364;
  USBFSD->UEP2_CTRL_H = USBFSD->UEP2_CTRL_H & 0xfff3;
  return;
}
