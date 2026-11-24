/* MSU2_MINI firmware 1.1 -- reconstructed C, module "draw".
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
 * spi_dma_tx_init   @ 0x2C9E
 * DMA1 CH3 (TX) -> SPI1
 * Signature: undefined FUN_ram_00002c9e()
 * ------------------------------------------------------------------ */
void spi_dma_tx_init(void)

{
  undefined8 uVar1;
  undefined4 auStack_2c [2];
  undefined4 uStack_24;
  undefined4 uStack_1c;
  undefined4 uStack_18;
  undefined4 uStack_14;
  undefined4 uStack_10;
  undefined4 uStack_c;
  undefined4 uStack_8;
  undefined4 uStack_4;
  
  uVar1 = __riscv_save_3();
  memset(auStack_2c,0,0x2c);
  RCC_AHBPeriphClockCmd(1,1);
  DMA_DeInit((int)uVar1);
  uStack_24 = 0x10;
  uStack_18 = 0x80;
  uStack_8 = 0x2000;
  uStack_1c = 0;
  uStack_14 = 0;
  uStack_10 = 0;
  uStack_c = 0;
  uStack_4 = 0;
  auStack_2c[0] = (int)((ulonglong)uVar1 >> 0x20);
  DMA_Init((int)uVar1,auStack_2c);
  return;
}

/* ------------------------------------------------------------------
 * spi_dma_rx_init   @ 0x2CF4
 * DMA1 CH2 (RX) <- SPI1
 * Signature: undefined FUN_ram_00002cf4()
 * ------------------------------------------------------------------ */
void spi_dma_rx_init(void)

{
  undefined8 uVar1;
  undefined4 auStack_2c [2];
  undefined4 uStack_24;
  undefined4 uStack_1c;
  undefined4 uStack_18;
  undefined4 uStack_14;
  undefined4 uStack_10;
  undefined4 uStack_c;
  undefined4 uStack_8;
  undefined4 uStack_4;
  
  uVar1 = __riscv_save_3();
  memset(auStack_2c,0,0x2c);
  RCC_AHBPeriphClockCmd(1,1);
  DMA_DeInit((int)uVar1);
  uStack_18 = 0x80;
  uStack_8 = 0x3000;
  uStack_24 = 0;
  uStack_1c = 0;
  uStack_14 = 0;
  uStack_10 = 0;
  uStack_c = 0;
  uStack_4 = 0;
  auStack_2c[0] = (int)((ulonglong)uVar1 >> 0x20);
  DMA_Init((int)uVar1,auStack_2c);
  return;
}

/* ------------------------------------------------------------------
 * dma_ring_init   @ 0x2D48
 * DMA1 CH8 memory-to-memory
 * Signature: undefined FUN_ram_00002d48()
 * ------------------------------------------------------------------ */
void dma_ring_init(void)

{
  undefined8 uVar1;
  undefined1 auStack_2c [4];
  undefined4 uStack_28;
  undefined4 uStack_24;
  undefined4 uStack_1c;
  undefined4 uStack_18;
  undefined4 uStack_14;
  undefined4 uStack_10;
  undefined4 uStack_c;
  undefined4 uStack_8;
  undefined4 uStack_4;
  
  uVar1 = __riscv_save_3();
  memset(auStack_2c,0,0x2c);
  RCC_AHBPeriphClockCmd(1,1);
  DMA_DeInit((int)uVar1);
  uStack_1c = 0x40;
  uStack_18 = 0x80;
  uStack_8 = 0x1000;
  uStack_4 = 0x4000;
  uStack_24 = 0;
  uStack_14 = 0;
  uStack_10 = 0;
  uStack_c = 0;
  uStack_28 = (int)((ulonglong)uVar1 >> 0x20);
  DMA_Init((int)uVar1,auStack_2c);
  return;
}

/* ------------------------------------------------------------------
 * lcd_draw_glyph8   @ 0x2DA2
 * 8x8 text primitive
 * Signature: undefined FUN_ram_00002da2()
 * ------------------------------------------------------------------ */
void lcd_draw_glyph8(void)

{
  byte bVar1;
  uint uVar2;
  int iVar3;
  undefined4 uVar4;
  int extraout_a2;
  undefined4 extraout_a3;
  undefined4 extraout_a4;
  undefined8 uVar5;
  
  uVar5 = __riscv_save_12();
  iVar3 = (int)((ulonglong)uVar5 >> 0x20);
  lcd_cs_dc(0x2a);
  lcd_write_cmd(0);
  lcd_write_cmd((int)uVar5);
  lcd_write_cmd(0);
  lcd_write_cmd((int)uVar5 + 7U & 0xff);
  lcd_cs_dc(0x2b);
  lcd_write_cmd(0);
  lcd_write_cmd(iVar3);
  lcd_write_cmd(0);
  lcd_write_cmd(iVar3 + 7U & 0xff);
  lcd_cs_dc(0x2c);
  iVar3 = 0;
  do {
    bVar1 = (logo_glyphs)[iVar3 + extraout_a2 * 8];
    uVar2 = 0;
    do {
      uVar4 = extraout_a3;
      if ((0x80 >> (uVar2 & 0x1f) & (uint)bVar1) == 0) {
        uVar4 = extraout_a4;
      }
      uVar2 = uVar2 + 1;
      lcd_write_data8(uVar4);
    } while (uVar2 != 8);
    iVar3 = iVar3 + 1;
  } while (iVar3 != 8);
  return;
}

/* ------------------------------------------------------------------
 * lcd_draw_text   @ 0x2E42
 * string with wrap
 * Signature: undefined FUN_ram_00002e42()
 * ------------------------------------------------------------------ */
void lcd_draw_text(void)

{
  uint uVar1;
  uint uVar2;
  char *extraout_a2;
  undefined4 extraout_a3;
  undefined4 extraout_a4;
  char *pcVar3;
  undefined8 uVar4;
  
  uVar4 = __riscv_save_8();
  for (pcVar3 = extraout_a2; uVar2 = (uint)((ulonglong)uVar4 >> 0x20), *pcVar3 != 0;
      pcVar3 = pcVar3 + 1) {
    uVar1 = (int)uVar4 + 8U & 0xff;
    lcd_draw_glyph8((int)uVar4,uVar2,(int)*pcVar3,extraout_a3,extraout_a4);
    if (0xe8 < uVar1) {
      uVar2 = uVar2 + 10 & 0xff;
      uVar1 = 0;
    }
    uVar4 = CONCAT44(uVar2,uVar1);
  }
  return;
}

/* ------------------------------------------------------------------
 * lcd_set_tile   @ 0x2E80
 * build the 256-byte pattern tile
 * Signature: undefined FUN_ram_00002e80()
 * ------------------------------------------------------------------ */
void lcd_set_tile(undefined1 *param_1,undefined1 *param_2)

{
  undefined1 *puVar1;
  undefined1 *puVar2;
  
  puVar1 = param_1;
  do {
    puVar2 = puVar1 + 4;
    *puVar1 = *param_2;
    puVar1[1] = param_2[1];
    puVar1[2] = param_2[2];
    puVar1[3] = param_2[3];
    puVar1 = puVar2;
  } while (puVar2 != param_1 + 0x100);
  return;
}

/* ------------------------------------------------------------------
 * lcd_fill_rect   @ 0x2EA4
 * window fill
 * Signature: undefined FUN_ram_00002ea4()
 * ------------------------------------------------------------------ */
void lcd_fill_rect(void)

{
  uint uVar1;
  int iVar2;
  int iVar3;
  int extraout_a2;
  int extraout_a3;
  int extraout_a4;
  int iVar4;
  undefined8 uVar5;
  undefined4 uStack_c;
  undefined4 uStack_8;
  undefined4 uStack_4;
  
  uVar5 = __riscv_save_12();
  iVar2 = (int)((ulonglong)uVar5 >> 0x20);
  uStack_c = 0x40;
  uStack_4 = 0x10;
  uStack_8 = 1;
  GPIO_Init(GPIOA->CFGLR,&uStack_c);
  lcd_set_window((int)uVar5,iVar2,extraout_a2 + -1 + (int)uVar5,extraout_a3 + -1 + iVar2);
  iVar4 = ((extraout_a2 * extraout_a3 & 0x7fffffffU) >> 7) + extraout_a4;
  iVar2 = extraout_a4;
  while (iVar4 != iVar2) {
    uStack_4 = 4;
    GPIO_Init(GPIOA->CFGLR,&uStack_c);
    flash_fast_read(&g_param_records,iVar2 << 8,0x100);
    uStack_4 = 0x10;
    GPIO_Init(GPIOA->CFGLR,&uStack_c);
    GPIO_WriteBit(GPIOA->CFGLR,0x200,0);
    GPIO_WriteBit(GPIOC->CFGLR,8,0);
    GPIO_WriteBit(GPIOA->CFGLR,0x40,1);
    DMA_Cmd(&DMA1_CH3->CFGR,0);
    DMA1_CH3->MADDR = 0x20000132;
    DMA1_CH3->CNTR = 0x100;
    iVar2 = iVar2 + 1;
    DMA_Cmd(&DMA1_CH3->CFGR,1);
    do {
      iVar3 = DMA_GetFlagStatus(0x200);
    } while (iVar3 == 0);
    DMA_ClearFlag(0x200);
    GPIO_WriteBit(GPIOA->CFGLR,0x200,1);
    GPIO_WriteBit(GPIOC->CFGLR,8,1);
  }
  uVar1 = extraout_a2 * extraout_a3 * 2 & 0xff;
  if (uVar1 != 0) {
    uStack_4 = 4;
    GPIO_Init(GPIOA->CFGLR,&uStack_c);
    flash_fast_read(&g_param_records,iVar4 * 0x100,uVar1);
    uStack_4 = 0x10;
    GPIO_Init(GPIOA->CFGLR,&uStack_c);
    GPIO_WriteBit(GPIOA->CFGLR,0x200,0);
    GPIO_WriteBit(GPIOC->CFGLR,8,0);
    GPIO_WriteBit(GPIOA->CFGLR,0x40,1);
    DMA_Cmd(&DMA1_CH3->CFGR,0);
    DMA1_CH3->MADDR = 0x20000132;
    DMA1_CH3->CNTR = uVar1;
    DMA_Cmd(&DMA1_CH3->CFGR,1);
    do {
      iVar2 = DMA_GetFlagStatus(0x200);
    } while (iVar2 == 0);
    DMA_ClearFlag(0x200);
    GPIO_WriteBit(GPIOA->CFGLR,0x200,1);
    GPIO_WriteBit(GPIOC->CFGLR,8,1);
  }
  return;
}

/* ------------------------------------------------------------------
 * lcd_draw_hline   @ 0x305A
 * Signature: undefined FUN_ram_0000305a()
 * ------------------------------------------------------------------ */
void lcd_draw_hline(void)

{
  int iVar1;
  int extraout_a2;
  int extraout_a3;
  undefined8 uVar2;
  undefined4 uStack_c;
  undefined4 uStack_8;
  undefined4 uStack_4;
  
  uVar2 = __riscv_save_3();
  iVar1 = (int)((ulonglong)uVar2 >> 0x20);
  uStack_c = 0x40;
  uStack_4 = 0x10;
  uStack_8 = 1;
  GPIO_Init(GPIOA->CFGLR,&uStack_c);
  lcd_set_window((int)uVar2,iVar1,extraout_a2 + -1 + (int)uVar2,extraout_a3 + -1 + iVar1);
  GPIO_WriteBit(GPIOA->CFGLR,0x40,1);
  GPIO_WriteBit(GPIOA->CFGLR,0x200,0);
  GPIO_WriteBit(GPIOC->CFGLR,8,0);
  return;
}

/* ------------------------------------------------------------------
 * lcd_stream   @ 0x30C6
 * stream a host-length buffer through SPI DMA
 * Signature: undefined FUN_ram_000030c6()
 * ------------------------------------------------------------------ */
void lcd_stream(void)

{
  int iVar1;
  undefined8 uVar2;
  
  uVar2 = __riscv_save_3();
  GPIO_WriteBit(GPIOA->CFGLR,0x200,0);
  GPIO_WriteBit(GPIOC->CFGLR,8,0);
  DMA_Cmd(&DMA1_CH3->CFGR,0);
  DMA1_CH3->CNTR = (int)((ulonglong)uVar2 >> 0x20);
  DMA1_CH3->MADDR = (int)uVar2;
  DMA_ClearFlag(0x200);
  DMA_Cmd(&DMA1_CH3->CFGR,1);
  do {
    iVar1 = DMA_GetFlagStatus(0x200);
  } while (iVar1 == 0);
  DMA_ClearFlag(0x200);
  GPIO_WriteBit(GPIOA->CFGLR,0x200,1);
  GPIO_WriteBit(GPIOC->CFGLR,8,1);
  return;
}

/* ------------------------------------------------------------------
 * lcd_stream_end   @ 0x3148
 * end of stream
 * Signature: undefined FUN_ram_00003148()
 * ------------------------------------------------------------------ */
void lcd_stream_end(void)

{
  int iVar1;
  
  __riscv_save_3();
  do {
    iVar1 = DMA_GetFlagStatus(0x200);
  } while (iVar1 == 0);
  DMA_ClearFlag(0x200);
  GPIO_WriteBit(GPIOA->CFGLR,0x200,1);
  GPIO_WriteBit(GPIOC->CFGLR,8,1);
  return;
}

/* ------------------------------------------------------------------
 * lcd_send_pixels   @ 0x3180
 * raw pixel writer
 * Signature: undefined FUN_ram_00003180()
 * ------------------------------------------------------------------ */
void lcd_send_pixels(int param_1,undefined1 *param_2,undefined4 param_3,undefined4 param_4,
                     uint param_5)

{
  uint uVar1;
  char *pcVar2;
  
  for (uVar1 = 0; uVar1 != param_5; uVar1 = uVar1 + 1 & 0xffff) {
    pcVar2 = (char *)((uVar1 >> 3) + param_1);
    if (*pcVar2 < '\0') {
      *param_2 = (char)((uint)param_3 >> 8);
      param_2[1] = (char)param_3;
    }
    else {
      *param_2 = (char)((uint)param_4 >> 8);
      param_2[1] = (char)param_4;
    }
    *pcVar2 = *pcVar2 << 1;
    param_2 = param_2 + 2;
  }
  return;
}

/* ------------------------------------------------------------------
 * lcd_blit_colour_bg   @ 0x31C0
 * colour blit with background
 * Signature: undefined FUN_ram_000031c0()
 * ------------------------------------------------------------------ */
void lcd_blit_colour_bg(void)

{
  uint uVar1;
  int iVar2;
  int extraout_a2;
  int extraout_a3;
  int extraout_a4;
  undefined4 extraout_a5;
  undefined4 extraout_a6;
  int iVar3;
  undefined8 uVar4;
  undefined4 uStack_c;
  undefined4 uStack_8;
  undefined4 uStack_4;
  
  uVar4 = __riscv_save_13();
  iVar3 = (int)((ulonglong)uVar4 >> 0x20);
  uStack_c = 0x40;
  uStack_4 = 0x10;
  uStack_8 = 1;
  GPIO_Init(GPIOA->CFGLR,&uStack_c);
  lcd_set_window((int)uVar4,iVar3,extraout_a2 + -1 + (int)uVar4,extraout_a3 + -1 + iVar3);
  for (iVar3 = extraout_a4 << 8;
      iVar3 != (extraout_a4 * 0x10 + ((uint)(extraout_a2 * extraout_a3) >> 7)) * 0x10;
      iVar3 = iVar3 + 0x10) {
    uStack_4 = 4;
    GPIO_Init(GPIOA->CFGLR,&uStack_c);
    flash_fast_read(gp + -0x76c,iVar3,0x10);
    lcd_send_pixels(gp + -0x766,0x20000132,extraout_a5,extraout_a6,0x50);
    uStack_4 = 0x10;
    GPIO_Init(GPIOA->CFGLR,&uStack_c);
    GPIO_WriteBit(GPIOA->CFGLR,0x200,0);
    GPIO_WriteBit(GPIOC->CFGLR,8,0);
    GPIO_WriteBit(GPIOA->CFGLR,0x40,1);
    DMA_Cmd(&DMA1_CH3->CFGR,0);
    DMA1_CH3->MADDR = 0x20000132;
    DMA1_CH3->CNTR = 0x100;
    DMA_Cmd(&DMA1_CH3->CFGR,1);
    lcd_send_pixels(gp + -0x75c,gp + -0x6ae,extraout_a5,extraout_a6,0x30);
    do {
      iVar2 = DMA_GetFlagStatus(0x200);
    } while (iVar2 == 0);
    DMA_ClearFlag(0x200);
    GPIO_WriteBit(GPIOA->CFGLR,0x200,1);
    GPIO_WriteBit(GPIOC->CFGLR,8,1);
  }
  uVar1 = extraout_a2 * extraout_a3 & 0x7f;
  if (uVar1 != 0) {
    uStack_4 = 4;
    GPIO_Init(GPIOA->CFGLR,&uStack_c);
    flash_fast_read(gp + -0x76c,iVar3,(uVar1 >> 3) + 1);
    lcd_send_pixels(gp + -0x766,0x20000132,extraout_a5,extraout_a6,uVar1);
    uStack_4 = 0x10;
    GPIO_Init(GPIOA->CFGLR,&uStack_c);
    GPIO_WriteBit(GPIOA->CFGLR,0x200,0);
    GPIO_WriteBit(GPIOC->CFGLR,8,0);
    GPIO_WriteBit(GPIOA->CFGLR,0x40,1);
    DMA_Cmd(&DMA1_CH3->CFGR,0);
    DMA1_CH3->MADDR = 0x20000132;
    DMA1_CH3->CNTR = uVar1 << 1;
    DMA_Cmd(&DMA1_CH3->CFGR,1);
    do {
      iVar3 = DMA_GetFlagStatus(0x200);
    } while (iVar3 == 0);
    DMA_ClearFlag(0x200);
    GPIO_WriteBit(GPIOA->CFGLR,0x200,1);
    GPIO_WriteBit(GPIOC->CFGLR,8,1);
  }
  __riscv_restore_1();
  return;
}

/* ------------------------------------------------------------------
 * lcd_image_with_bg   @ 0x33B8
 * background image + foreground
 * Signature: undefined FUN_ram_000033b8()
 * ------------------------------------------------------------------ */
void lcd_image_with_bg(void)

{
  int iVar1;
  uint uVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  int extraout_a2;
  int extraout_a3;
  int extraout_a4;
  undefined4 extraout_a5;
  int iVar6;
  int extraout_a6;
  int iVar7;
  uint uVar8;
  uint uVar9;
  uint uVar10;
  uint uVar11;
  uint uVar12;
  undefined8 uVar13;
  undefined4 uStack_c;
  undefined4 uStack_8;
  undefined4 uStack_4;
  
  uVar13 = __riscv_save_13();
  iVar1 = (int)((ulonglong)uVar13 >> 0x20);
  iVar3 = (int)uVar13;
  uStack_c = 0x40;
  uVar2 = iVar3 + extraout_a2;
  uStack_4 = 0x10;
  uStack_8 = 1;
  GPIO_Init(GPIOA->CFGLR,&uStack_c);
  lcd_set_window(iVar3,iVar1,uVar2 - 1,extraout_a3 + -1 + iVar1);
  uVar10 = extraout_a2 * extraout_a3 & 0x7f;
  iVar7 = extraout_a4 * 0x100;
  iVar6 = ((uint)(extraout_a2 * extraout_a3) >> 7) * 0x10;
  for (iVar1 = iVar7; iVar1 != iVar6 + iVar7; iVar1 = iVar1 + 0x10) {
    uStack_4 = 4;
    GPIO_Init(GPIOA->CFGLR,&uStack_c);
    flash_fast_read(gp + -0x76c,iVar1,0x10);
    uVar8 = 0;
    do {
      iVar4 = (int)((ulonglong)uVar13 >> 0x20);
      uVar12 = (uint)uVar13;
      if (uVar12 < uVar2) {
        uVar11 = uVar2 - uVar12;
        uVar9 = uVar8 & 0xffff;
        iVar5 = uVar12 * 2 + extraout_a6 * 0x100 + iVar4 * 0x140;
        if (0x80 - uVar8 < uVar11) {
          flash_fast_read_dma(&g_param_records + uVar8 * 2,iVar5,uVar9 * -2 + 0x100 & 0xffff,
                           gp + -0x766,uVar9,extraout_a5);
          iVar5 = iVar4;
          iVar4 = (uVar12 + 0x80) - uVar8;
          break;
        }
        flash_fast_read_dma(&g_param_records + uVar8 * 2,iVar5,(uVar11 & 0x7fff) << 1,gp + -0x766,
                         uVar9,extraout_a5);
        uVar8 = uVar8 + uVar11;
      }
      iVar5 = iVar4 + 1;
      uVar13 = CONCAT44(iVar4 + 1,iVar3);
      iVar4 = iVar3;
    } while (uVar8 < 0x80);
    uVar13 = CONCAT44(iVar5,iVar4);
    uStack_4 = 0x10;
    GPIO_Init(GPIOA->CFGLR,&uStack_c);
    GPIO_WriteBit(GPIOA->CFGLR,0x200,0);
    GPIO_WriteBit(GPIOC->CFGLR,8,0);
    GPIO_WriteBit(GPIOA->CFGLR,0x40,1);
    DMA_Cmd(&DMA1_CH3->CFGR,0);
    DMA1_CH3->MADDR = &g_param_records;
    DMA1_CH3->CNTR = 0x100;
    DMA_Cmd(&DMA1_CH3->CFGR,1);
    do {
      iVar4 = DMA_GetFlagStatus(0x200);
    } while (iVar4 == 0);
    DMA_ClearFlag(0x200);
    GPIO_WriteBit(GPIOA->CFGLR,0x200,1);
    GPIO_WriteBit(GPIOC->CFGLR,8,1);
  }
  if (uVar10 != 0) {
    uStack_4 = 4;
    GPIO_Init(GPIOA->CFGLR,&uStack_c);
    flash_fast_read(gp + -0x76c,iVar7 + iVar6,0x10);
    uVar8 = 0;
    do {
      uVar12 = (uint)uVar13;
      iVar1 = (int)((ulonglong)uVar13 >> 0x20) + 1;
      uVar13 = CONCAT44(iVar1,iVar3);
      if (uVar12 < uVar2) {
        uVar9 = uVar2 - uVar12;
        iVar1 = iVar1 * 0x140 + extraout_a6 * 0x100 + -0x140 + uVar12 * 2;
        if (uVar10 - uVar8 < uVar9) {
          flash_fast_read_dma(&g_param_records + uVar8 * 2,iVar1,(uVar10 - uVar8 & 0x7fff) << 1,
                           gp + -0x766,uVar8 & 0xffff,extraout_a5);
          break;
        }
        flash_fast_read_dma(&g_param_records + uVar8 * 2,iVar1,(uVar9 & 0x7fff) << 1,gp + -0x766,
                         uVar8 & 0xffff,extraout_a5);
        uVar8 = uVar8 + uVar9;
      }
    } while (uVar8 < uVar10);
    uStack_4 = 0x10;
    GPIO_Init(GPIOA->CFGLR,&uStack_c);
    GPIO_WriteBit(GPIOA->CFGLR,0x200,0);
    GPIO_WriteBit(GPIOC->CFGLR,8,0);
    GPIO_WriteBit(GPIOA->CFGLR,0x40,1);
    DMA_Cmd(&DMA1_CH3->CFGR,0);
    DMA1_CH3->MADDR = &g_param_records;
    DMA1_CH3->CNTR = uVar10 << 1;
    DMA_Cmd(&DMA1_CH3->CFGR,1);
    do {
      iVar1 = DMA_GetFlagStatus(0x200);
    } while (iVar1 == 0);
    DMA_ClearFlag(0x200);
    GPIO_WriteBit(GPIOA->CFGLR,0x200,1);
    GPIO_WriteBit(GPIOC->CFGLR,8,1);
  }
  __riscv_restore_1();
  return;
}

/* ------------------------------------------------------------------
 * lcd_mono_image_a   @ 0x367C
 * 1-bit image expansion
 * Signature: undefined FUN_ram_0000367c()
 * ------------------------------------------------------------------ */
void lcd_mono_image_a(void)

{
  int extraout_a2;
  undefined4 extraout_a3;
  undefined4 extraout_a4;
  int extraout_a5;
  undefined8 uVar1;
  
  uVar1 = __riscv_save_3();
  lcd_blit_colour_bg((int)uVar1,(int)((ulonglong)uVar1 >> 0x20),0x20,0x40,extraout_a2 + extraout_a5,
                   extraout_a3,extraout_a4);
  return;
}

/* ------------------------------------------------------------------
 * lcd_mono_image_b   @ 0x3698
 * 1-bit image expansion
 * Signature: undefined FUN_ram_00003698()
 * ------------------------------------------------------------------ */
void lcd_mono_image_b(void)

{
  int extraout_a2;
  undefined4 extraout_a3;
  undefined4 extraout_a4;
  int extraout_a5;
  undefined8 uVar1;
  
  uVar1 = __riscv_save_3();
  lcd_image_with_bg((int)uVar1,(int)((ulonglong)uVar1 >> 0x20),0x20,0x40,
                   extraout_a2 + extraout_a5 & 0xffff,extraout_a3,extraout_a4);
  return;
}

/* ------------------------------------------------------------------
 * lcd_mono_image   @ 0x36B8
 * 1-bit image from a flash page
 * Signature: undefined FUN_ram_000036b8()
 * ------------------------------------------------------------------ */
void lcd_mono_image(void)

{
  uint uVar1;
  uint uVar2;
  uint uVar3;
  int iVar4;
  byte *extraout_a2;
  uint extraout_a3;
  uint extraout_a4;
  undefined8 uVar5;
  undefined4 uStack_c;
  undefined4 uStack_8;
  undefined4 uStack_4;
  
  uVar5 = __riscv_save_12();
  iVar4 = (int)((ulonglong)uVar5 >> 0x20);
  uStack_c = 0x40;
  uStack_4 = 4;
  uStack_8 = 0;
  GPIO_Init(GPIOA->CFGLR,&uStack_c);
  flash_read_data(&g_param_records,
                   ((*extraout_a2 - 0xa1) * 0x5e + (uint)extraout_a2[1] + 0x11cf) * 0x20,0x20);
  uStack_4 = 0x10;
  uStack_8 = 1;
  GPIO_Init(GPIOA->CFGLR,&uStack_c);
  lcd_set_window((int)uVar5,iVar4,(int)uVar5 + 0xf,iVar4 + 0xf);
  GPIO_WriteBit(GPIOA->CFGLR,0x200,0);
  GPIO_WriteBit(GPIOC->CFGLR,8,0);
  GPIO_WriteBit(GPIOA->CFGLR,0x40,1);
  uVar3 = 0;
  do {
    uVar2 = uVar3 >> 3;
    if ((char)(&g_param_records)[uVar2] < '\0') {
      spi_xfer_byte(extraout_a3 >> 8);
      uVar1 = extraout_a3;
    }
    else {
      spi_xfer_byte(extraout_a4 >> 8);
      uVar1 = extraout_a4;
    }
    spi_xfer_byte(uVar1 & 0xff);
    uVar3 = uVar3 + 1;
    (&g_param_records)[uVar2] = (&g_param_records)[uVar2] << 1;
  } while (uVar3 != 0x100);
  GPIO_WriteBit(GPIOA->CFGLR,0x200,1);
  GPIO_WriteBit(GPIOC->CFGLR,8,1);
  return;
}

/* ------------------------------------------------------------------
 * lcd_mono_image_mix   @ 0x37D4
 * 1-bit image over a background page
 * Signature: undefined FUN_ram_000037d4()
 * ------------------------------------------------------------------ */
void lcd_mono_image_mix(void)

{
  char cVar1;
  uint uVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  byte *extraout_a2;
  uint uVar6;
  undefined2 extraout_a3;
  int extraout_a4;
  undefined1 *puVar7;
  int iVar8;
  int iVar9;
  uint uVar10;
  undefined8 uVar11;
  undefined4 uStack_1c;
  undefined4 uStack_18;
  undefined4 uStack_14;
  char local_10 [16];
  
  uVar11 = __riscv_save_13();
  iVar9 = (int)((ulonglong)uVar11 >> 0x20);
  iVar3 = (int)uVar11;
  spi_dma_tx_init(&DMA1_CH3->CFGR,SPI1->DATAR,&g_param_records,0x100);
  uStack_1c = 0x40;
  uStack_14 = 0x10;
  uStack_18 = 1;
  GPIO_Init(GPIOA->CFGLR,&uStack_1c);
  lcd_set_window(iVar3,iVar9,iVar3 + 0xf,iVar9 + 0xf);
  iVar9 = 0;
  do {
    uStack_14 = 4;
    GPIO_Init(GPIOA->CFGLR,&uStack_1c);
    uVar10 = 0;
    do {
      iVar4 = (int)((ulonglong)uVar11 >> 0x20);
      uVar2 = (uint)uVar11;
      if (uVar2 < iVar3 + 0x10U) {
        uVar6 = (iVar3 - uVar2) + 0x10;
        iVar5 = uVar2 * 2 + extraout_a4 * 0x100 + iVar4 * 0x1e0;
        if (0x80 - uVar10 < uVar6) {
          flash_read_data(&g_param_records + uVar10 * 2,iVar5,uVar10 * -2 + 0x100 & 0xffff);
          iVar5 = (uVar2 + 0x80) - uVar10;
          iVar8 = iVar4;
          break;
        }
        flash_read_data(&g_param_records + uVar10 * 2,iVar5,(uVar6 & 0x7fff) << 1);
        uVar10 = (iVar3 - uVar2) + uVar10 + 0x10;
      }
      iVar8 = iVar4 + 1;
      uVar11 = CONCAT44(iVar4 + 1,iVar3);
      iVar5 = iVar3;
    } while (uVar10 < 0x80);
    uVar11 = CONCAT44(iVar8,iVar5);
    flash_read_data(local_10,((*extraout_a2 - 0xa1) * 0x5e + (uint)extraout_a2[1] + 0x11cf) * 0x20
                              + iVar9,0x10);
    puVar7 = &g_param_records;
    uVar10 = 0;
    do {
      cVar1 = local_10[uVar10 >> 3];
      if (cVar1 < '\0') {
        puVar7[1] = (char)extraout_a3;
        *puVar7 = (char)((ushort)extraout_a3 >> 8);
      }
      local_10[uVar10 >> 3] = cVar1 << 1;
      uVar10 = uVar10 + 1;
      puVar7 = puVar7 + 2;
    } while (uVar10 != 0x80);
    uStack_14 = 0x10;
    GPIO_Init(GPIOA->CFGLR,&uStack_1c);
    GPIO_WriteBit(GPIOA->CFGLR,0x200,0);
    GPIO_WriteBit(GPIOC->CFGLR,8,0);
    GPIO_WriteBit(GPIOA->CFGLR,0x40,1);
    DMA_Cmd(&DMA1_CH3->CFGR,0);
    DMA1_CH3->CNTR = 0x100;
    DMA_Cmd(&DMA1_CH3->CFGR,1);
    do {
      iVar4 = DMA_GetFlagStatus(0x200);
    } while (iVar4 == 0);
    DMA_ClearFlag(0x200);
    GPIO_WriteBit(GPIOA->CFGLR,0x200,1);
    GPIO_WriteBit(GPIOC->CFGLR,8,1);
    iVar9 = iVar9 + 0x10;
    if (iVar9 == 0x20) {
      __riscv_restore_1();
      return;
    }
  } while( true );
}
