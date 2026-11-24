/* MSU2_MINI firmware 1.1 -- reconstructed C, module "main".
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
 * main   @ 0x39B4
 * application entry
 * Signature: undefined main()
 *
 * WCH 16-bit load/store sites (quadrant-2 custom opcodes, invisible to the decompiler):
 *   0x3B26  sh a5,0x6(s0)
 * ------------------------------------------------------------------ */
void main(void)

{
  undefined1 uVar1;
  undefined2 uVar2;
  undefined4 uVar3;
  int iVar4;
  
  __riscv_save_8();
  write_once_flag(1);
  SystemCoreClockUpdate();
  Delay_Init();
  USART2_Init(0x1c200);
  printf(str_5C38,SystemCoreClock);
  uVar3 = GetChipID();
  printf(str_5C48,uVar3);
  board_clocks_enable();
  tim3_init();
  board_gpio_init();
  uVar3 = pvd_probe();
  usb_device_init(1,uVar3);
  *(undefined1 *)(gp + 0x798) = 1;
  *(undefined1 *)(gp + 0x79a) = 0;
  uVar1 = *(undefined1 *)(gp + 0x79b);
  *(undefined1 *)(gp + 0x79b) = 0;
  spi_dma_tx_init(&DMA1_CH3->CFGR,SPI1->DATAR,&g_param_records,0,uVar1);
  spi_dma_rx_init(&DMA1_CH2->CFGR,SPI1->DATAR,&g_param_records,0);
  dma_ring_init(DMA1_CH8->CFGR,gp + -0x5bc,gp + -0x5b6,0);
  spi1_gpio_init();
  uVar2 = flash_read_id();
  *(undefined2 *)(gp + -0x5b8) = uVar2;
  flash_uid_to_record();
  flash_read_uid();
  flash_read_security(gp + -0x57e);
  iVar4 = licence_check();
  if (iVar4 != 0) {
    *(undefined1 *)(gp + -0x7b1) = 1;
  }
  spi1_gpio_init2();
  lcd_init();
  lcd_blit_colour(0,0,0xa0,0x50,0xffff);
  lcd_blit_colour_bg(0,4,0xa0,0x44,0xeec,0x1f,0xffff);
  if (*(char *)(gp + -0x7b1) == '\0') {
    lcd_draw_text(0,0x60,str_5C58,0x1f,0xffff);
    lcd_draw_text(0x20,0x60,str_5C60,0x1f,0xffff);
    lcd_draw_text(0x50,0x60,str_5C68,0x1f,0xffff);
  }
  adc_init();
  reset_session();
  do {
    main_loop();
  } while( true );
}
