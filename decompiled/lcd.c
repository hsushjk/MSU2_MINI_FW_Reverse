/* MSU2_MINI firmware 1.1 -- reconstructed C, module "lcd".
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
 * lcd_cs_dc   @ 0x2668
 * LCD chip select and D/C
 * Signature: undefined FUN_ram_00002668()
 * ------------------------------------------------------------------ */
void lcd_cs_dc(void)

{
  undefined4 uVar1;
  
  uVar1 = __riscv_save_3();
  GPIO_WriteBit(GPIOA->CFGLR,0x200,0);
  GPIO_WriteBit(GPIOC->CFGLR,8,0);
  GPIO_WriteBit(GPIOA->CFGLR,0x40,0);
  spi_xfer_byte(uVar1);
  GPIO_WriteBit(GPIOA->CFGLR,0x200,1);
  GPIO_WriteBit(GPIOC->CFGLR,8,1);
  return;
}

/* ------------------------------------------------------------------
 * lcd_write_cmd   @ 0x26BE
 * command byte
 * Signature: undefined FUN_ram_000026be()
 * ------------------------------------------------------------------ */
void lcd_write_cmd(void)

{
  undefined4 uVar1;
  
  uVar1 = __riscv_save_3();
  GPIO_WriteBit(GPIOA->CFGLR,0x200,0);
  GPIO_WriteBit(GPIOC->CFGLR,8,0);
  GPIO_WriteBit(GPIOA->CFGLR,0x40,1);
  spi_xfer_byte(uVar1);
  GPIO_WriteBit(GPIOA->CFGLR,0x200,1);
  GPIO_WriteBit(GPIOC->CFGLR,8,1);
  return;
}

/* ------------------------------------------------------------------
 * lcd_write_data8   @ 0x2714
 * data byte
 * Signature: undefined FUN_ram_00002714()
 * ------------------------------------------------------------------ */
void lcd_write_data8(void)

{
  uint uVar1;
  
  uVar1 = __riscv_save_3();
  GPIO_WriteBit(GPIOA->CFGLR,0x200,0);
  GPIO_WriteBit(GPIOC->CFGLR,8,0);
  GPIO_WriteBit(GPIOA->CFGLR,0x40,1);
  spi_xfer_byte(uVar1 >> 8 & 0xff);
  spi_xfer_byte(uVar1 & 0xff);
  GPIO_WriteBit(GPIOA->CFGLR,0x200,1);
  GPIO_WriteBit(GPIOC->CFGLR,8,1);
  return;
}

/* ------------------------------------------------------------------
 * lcd_init   @ 0x2778
 * ST7735S init sequence
 * Signature: undefined FUN_ram_00002778()
 * ------------------------------------------------------------------ */
void lcd_init(void)

{
  undefined4 uVar1;
  
  __riscv_save_3();
  Delay_Ms(10);
  GPIO_WriteBit(GPIOA->CFGLR,1,0);
  Delay_Ms(10);
  GPIO_WriteBit(GPIOA->CFGLR,1,1);
  Delay_Ms(10);
  lcd_cs_dc(0x11);
  Delay_Ms(10);
  lcd_cs_dc(0xb1);
  lcd_write_cmd(5);
  lcd_write_cmd(0x3c);
  lcd_write_cmd(0x3c);
  lcd_cs_dc(0xb2);
  lcd_write_cmd(5);
  lcd_write_cmd(0x3c);
  lcd_write_cmd(0x3c);
  lcd_cs_dc(0xb3);
  lcd_write_cmd(5);
  lcd_write_cmd(0x3c);
  lcd_write_cmd(0x3c);
  lcd_write_cmd(5);
  lcd_write_cmd(0x3c);
  lcd_write_cmd(0x3c);
  lcd_cs_dc(0xb4);
  lcd_write_cmd(3);
  lcd_cs_dc(0xc0);
  lcd_write_cmd(0xab);
  lcd_write_cmd(0xb);
  lcd_write_cmd(4);
  lcd_cs_dc(0xc1);
  lcd_write_cmd(0xc5);
  lcd_cs_dc(0xc2);
  lcd_write_cmd(0xd);
  lcd_write_cmd(0);
  lcd_cs_dc(0xc3);
  lcd_write_cmd(0x8d);
  lcd_write_cmd(0x6a);
  lcd_cs_dc(0xc4);
  lcd_write_cmd(0x8d);
  lcd_write_cmd(0xee);
  lcd_cs_dc(0xc5);
  lcd_write_cmd(0xf);
  lcd_cs_dc(0xe0);
  lcd_write_cmd(7);
  lcd_write_cmd(0xe);
  lcd_write_cmd(8);
  lcd_write_cmd(7);
  lcd_write_cmd(0x10);
  lcd_write_cmd(7);
  lcd_write_cmd(2);
  lcd_write_cmd(7);
  lcd_write_cmd(9);
  lcd_write_cmd(0xf);
  lcd_write_cmd(0x25);
  lcd_write_cmd(0x36);
  lcd_write_cmd(0);
  lcd_write_cmd(8);
  lcd_write_cmd(4);
  lcd_write_cmd(0x10);
  lcd_cs_dc(0xe1);
  lcd_write_cmd(10);
  lcd_write_cmd(0xd);
  lcd_write_cmd(8);
  lcd_write_cmd(7);
  lcd_write_cmd(0xf);
  lcd_write_cmd(7);
  lcd_write_cmd(2);
  lcd_write_cmd(7);
  lcd_write_cmd(9);
  lcd_write_cmd(0xf);
  lcd_write_cmd(0x25);
  lcd_write_cmd(0x35);
  lcd_write_cmd(0);
  lcd_write_cmd(9);
  lcd_write_cmd(4);
  lcd_write_cmd(0x10);
  lcd_cs_dc(0xfc);
  lcd_write_cmd(0x80);
  lcd_cs_dc(0x3a);
  lcd_write_cmd(5);
  lcd_cs_dc(0x36);
  uVar1 = 0x78;
  if (g_display_rotation != '\0') {
    if (g_display_rotation != '\x01') goto LAB_ram_000029c8;
    uVar1 = 0xa8;
  }
  lcd_write_cmd(uVar1);
LAB_ram_000029c8:
  lcd_cs_dc(0x29);
  lcd_cs_dc(0x2a);
  lcd_write_cmd(0);
  lcd_write_cmd(0x1a);
  lcd_write_cmd(0);
  lcd_write_cmd(0x69);
  lcd_cs_dc(0x2b);
  lcd_write_cmd(0);
  lcd_write_cmd(1);
  lcd_write_cmd(0);
  lcd_write_cmd(0xa0);
  lcd_cs_dc(0x2c);
  return;
}

/* ------------------------------------------------------------------
 * lcd_set_window   @ 0x2A20
 * CASET/RASET/RAMWR
 * Signature: undefined FUN_ram_00002a20()
 * ------------------------------------------------------------------ */
void lcd_set_window(void)

{
  uint uVar1;
  uint extraout_a2;
  int extraout_a3;
  undefined8 uVar2;
  
  uVar2 = __riscv_save_8();
  uVar1 = (int)((ulonglong)uVar2 >> 0x20) + 0x18;
  lcd_cs_dc(0x2a);
  lcd_write_cmd((uint)uVar2 >> 8 & 0xff);
  lcd_write_cmd((uint)uVar2 & 0xff);
  lcd_write_cmd(extraout_a2 >> 8 & 0xff);
  lcd_write_cmd(extraout_a2 & 0xff);
  lcd_cs_dc(0x2b);
  lcd_write_cmd(uVar1 >> 8 & 0xff);
  lcd_write_cmd(uVar1 & 0xff);
  lcd_write_cmd(extraout_a3 + 0x18U >> 8 & 0xff);
  lcd_write_cmd(extraout_a3 + 0x18U & 0xff);
  lcd_cs_dc(0x2c);
  return;
}

/* ------------------------------------------------------------------
 * lcd_blit_colour   @ 0x2A9C
 * RGB565 rectangle from flash page
 * Signature: undefined FUN_ram_00002a9c()
 * ------------------------------------------------------------------ */
void lcd_blit_colour(void)

{
  uint uVar1;
  int iVar2;
  int extraout_a2;
  int extraout_a3;
  undefined2 extraout_a4;
  undefined1 *puVar3;
  int iVar4;
  undefined8 uVar5;
  undefined4 uStack_c;
  undefined4 uStack_8;
  undefined4 uStack_4;
  
  uVar5 = __riscv_save_8();
  iVar4 = (int)((ulonglong)uVar5 >> 0x20);
  uStack_c = 0x40;
  uStack_4 = 0x10;
  uStack_8 = 1;
  GPIO_Init(GPIOA->CFGLR,&uStack_c);
  lcd_set_window((int)uVar5,iVar4,(int)uVar5 + extraout_a2 + -1,iVar4 + extraout_a3 + -1);
  puVar3 = &g_param_records;
  do {
    *puVar3 = (char)((ushort)extraout_a4 >> 8);
    puVar3[1] = (char)extraout_a4;
    puVar3 = puVar3 + 2;
  } while (puVar3 != (undefined1 *)0x2000022c);
  GPIO_WriteBit(GPIOA->CFGLR,0x200,0);
  GPIO_WriteBit(GPIOC->CFGLR,8,0);
  GPIO_WriteBit(GPIOA->CFGLR,0x40,1);
  for (iVar4 = 0; iVar4 != extraout_a2 * extraout_a3 >> 7; iVar4 = iVar4 + 1) {
    DMA_Cmd(&DMA1_CH3->CFGR,0);
    DMA1_CH3->MADDR = &g_param_records;
    DMA1_CH3->CNTR = 0x100;
    DMA_Cmd(&DMA1_CH3->CFGR,1);
    do {
      iVar2 = DMA_GetFlagStatus(0x200);
    } while (iVar2 == 0);
    DMA_ClearFlag(0x200);
  }
  uVar1 = extraout_a2 * extraout_a3 * 2 & 0xff;
  if (uVar1 != 0) {
    DMA_Cmd(&DMA1_CH3->CFGR,0);
    DMA1_CH3->MADDR = &g_param_records;
    DMA1_CH3->CNTR = uVar1;
    DMA_Cmd(&DMA1_CH3->CFGR,1);
    do {
      iVar4 = DMA_GetFlagStatus(0x200);
    } while (iVar4 == 0);
    DMA_ClearFlag(0x200);
  }
  GPIO_WriteBit(GPIOA->CFGLR,0x200,1);
  GPIO_WriteBit(GPIOC->CFGLR,8,1);
  return;
}

/* ------------------------------------------------------------------
 * adc_init   @ 0x2BCC
 * ADC1 channel setup
 * Signature: undefined FUN_ram_00002bcc()
 * ------------------------------------------------------------------ */
void adc_init(void)

{
  undefined4 uStack_2c;
  undefined4 uStack_28;
  undefined4 uStack_24;
  undefined4 uStack_20;
  undefined4 uStack_1c;
  undefined4 uStack_18;
  undefined4 uStack_14;
  undefined4 uStack_10;
  undefined1 uStack_c;
  
  __riscv_save_3();
  uStack_2c = 0;
  uStack_28 = 0;
  uStack_24 = 0;
  memset(&uStack_20,0,0x20);
  RCC_APB2PeriphClockCmd(8,1);
  RCC_APB2PeriphClockCmd(0x200,1);
  uStack_2c = 2;
  uStack_24 = 0;
  GPIO_Init(GPIOB->CFGLR,&uStack_2c);
  sub_3D66(ADC1->STATR,0x25);
  uStack_14 = 0xe0000;
  uStack_c = 1;
  uStack_20 = 0;
  uStack_1c = 0;
  uStack_18 = 0;
  uStack_10 = 0;
  ADC_Init(ADC1->STATR,&uStack_20);
  ADC_Cmd(ADC1->STATR,1);
  ADC1->CTLR1 = ADC1->CTLR1 | 0x1000000;
  return;
}

/* ------------------------------------------------------------------
 * adc_read   @ 0x2C58
 * single conversion on a channel, averaged
 * Signature: undefined FUN_ram_00002c58()
 * ------------------------------------------------------------------ */
uint adc_read(void)

{
  undefined4 uVar1;
  int iVar2;
  
  uVar1 = __riscv_save_3();
  ADC_GetConversionValue(ADC1->STATR,uVar1,1,7);
  ADC1->IDATAR1 = 0x70;
  ADC1->RDATAR = 0x40;
  do {
    iVar2 = sub_3D5C(ADC1->STATR,2);
  } while (iVar2 == 0);
  return ADC1->RDATAR & 0xffff;
}
