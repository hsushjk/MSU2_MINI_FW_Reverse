/* MSU2_MINI firmware 1.1 -- reconstructed C, module "flash".
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
 * flash_uid_to_record   @ 0x1A66
 * cache the 128-bit UID in the licence record
 * Signature: undefined FUN_ram_00001a66()
 * ------------------------------------------------------------------ */
void flash_uid_to_record(void)

{
  undefined1 uVar1;
  undefined1 *puVar2;
  undefined1 *puVar3;
  
  puVar2 = &DAT_ram_1ffff7e8;
  puVar3 = (undefined1 *)(gp + -0x59c);
  do {
    uVar1 = *puVar2;
    puVar2 = puVar2 + 1;
    *puVar3 = uVar1;
    puVar3 = puVar3 + 1;
  } while (puVar2 != (undefined1 *)0x1ffff7f4);
  return;
}

/* ------------------------------------------------------------------
 * licence_check   @ 0x1A86
 * 16-pair arithmetic check, destroys the record on failure
 * Signature: undefined FUN_ram_00001a86()
 * ------------------------------------------------------------------ */
undefined4 licence_check(void)

{
  byte bVar1;
  byte bVar2;
  byte *pbVar3;
  byte *pbVar4;
  byte *pbVar5;
  byte *pbVar6;
  
  *(undefined1 *)(gp + -0x598) = 0x59;
  *(undefined1 *)(gp + -0x597) = 0x58;
  pbVar6 = (byte *)(gp + -0x57e);
  pbVar3 = (byte *)(gp + -0x59c);
  pbVar4 = (byte *)(gp + -0x59c);
  do {
    bVar1 = pbVar4[6];
    bVar2 = pbVar4[7];
    pbVar4[6] = ~bVar1;
    pbVar4[7] = (byte)(bVar2 ^ 0x51);
    pbVar5 = (byte *)(gp + -0x59c);
    if ((uint)(byte)~bVar1 * 0x100 + (bVar2 ^ 0x51) != (uint)*pbVar3 * (uint)pbVar3[0xf]) {
      do {
        pbVar5[0x1e] = 0;
        pbVar3 = pbVar5 + 1;
        *pbVar5 = *pbVar5 ^ 0xa5;
        pbVar5 = pbVar3;
      } while (pbVar3 != pbVar6);
      *(undefined1 *)(gp + -0x57d) = 0;
      return 0;
    }
    pbVar4 = pbVar4 + 2;
    pbVar3 = pbVar3 + 1;
  } while (pbVar4 != pbVar6);
  do {
    pbVar5[0x1e] = 0;
    pbVar3 = pbVar5 + 1;
    *pbVar5 = *pbVar5 ^ 0xa5;
    pbVar5 = pbVar3;
  } while (pbVar3 != pbVar6);
  *(undefined1 *)(gp + -0x595) = 1;
  return 1;
}

/* ------------------------------------------------------------------
 * spi_xfer_byte   @ 0x1B10
 * blocking 8-bit SPI transfer (returns 0 on timeout)
 * Signature: undefined FUN_ram_00001b10()
 * ------------------------------------------------------------------ */
undefined1 spi_xfer_byte(void)

{
  char cVar1;
  undefined1 uVar2;
  undefined4 uVar3;
  int iVar4;
  
  uVar3 = __riscv_save_3();
  cVar1 = -0x37;
  while (iVar4 = SPI1_GetFlagStatus(SPI1->CTLR1,2), iVar4 == 0) {
    cVar1 = cVar1 + -1;
    if (cVar1 == '\0') {
      return 0;
    }
  }
  SPI1_SendData(SPI1->CTLR1,uVar3);
  cVar1 = -0x37;
  do {
    iVar4 = SPI1_GetFlagStatus(SPI1->CTLR1,1);
    if (iVar4 != 0) {
      uVar2 = SPI1_ReceiveData(SPI1->CTLR1);
      return uVar2;
    }
    cVar1 = cVar1 + -1;
  } while (cVar1 != '\0');
  return 0;
}

/* ------------------------------------------------------------------
 * flash_read_uid   @ 0x1B66
 * 0x4B
 * Signature: undefined FUN_ram_00001b66()
 * ------------------------------------------------------------------ */
void flash_read_uid(void)

{
  int iVar1;
  undefined1 uVar2;
  
  __riscv_save_3();
  GPIO_WriteBit(GPIOB->CFGLR,0x80,0);
  spi_xfer_byte(0x4b);
  spi_xfer_byte(0xff);
  spi_xfer_byte(0xff);
  spi_xfer_byte(0xff);
  iVar1 = gp + -0x59c;
  spi_xfer_byte(0xff);
  do {
    uVar2 = spi_xfer_byte(0xff);
    *(undefined1 *)(iVar1 + 4) = uVar2;
    iVar1 = iVar1 + 1;
  } while (iVar1 != gp + -0x58c);
  GPIO_WriteBit(GPIOB->CFGLR,0x80,1);
  return;
}

/* ------------------------------------------------------------------
 * spi1_gpio_init   @ 0x1BC6
 * SPI1 + CS + D/C pins (first copy)
 * Signature: undefined FUN_ram_00001bc6()
 * ------------------------------------------------------------------ */
void spi1_gpio_init(void)

{
  undefined4 local_20;
  undefined4 uStack_1c;
  undefined4 uStack_18;
  undefined4 uStack_14;
  undefined4 uStack_10;
  undefined4 uStack_c;
  undefined4 uStack_8;
  undefined2 uStack_4;
  
  __riscv_save_8();
  local_20 = 0;
  uStack_1c = 0;
  uStack_18 = 0;
  uStack_14 = 0;
  uStack_10 = 0;
  uStack_c = 0;
  uStack_8 = 0;
  uStack_4 = 0;
  RCC_APB2PeriphClockCmd(8,1);
  local_20 = 0x80;
  uStack_18 = 0x10;
  uStack_1c = 1;
  GPIO_Init(GPIOB->CFGLR,&local_20);
  GPIO_SetBits(GPIOB->CFGLR,0x80);
  local_20 = 1;
  uStack_18 = 0x10;
  uStack_1c = 1;
  GPIO_Init(GPIOA->CFGLR,&local_20);
  GPIO_SetBits(GPIOA->CFGLR,1);
  RCC_APB2PeriphClockCmd(0x10,1);
  local_20 = 8;
  uStack_18 = 0x10;
  uStack_1c = 1;
  GPIO_Init(GPIOC->CFGLR,&local_20);
  GPIO_SetBits(GPIOC->CFGLR,8);
  RCC_APB2PeriphClockCmd(0x1014,1);
  local_20 = 0x20;
  uStack_18 = 0x18;
  uStack_1c = 1;
  GPIO_Init(GPIOA->CFGLR,&local_20);
  local_20 = 0x40;
  uStack_18 = 4;
  GPIO_Init(GPIOA->CFGLR,&local_20);
  local_20 = 0x80;
  uStack_18 = 0x18;
  uStack_1c = 1;
  GPIO_Init(GPIOA->CFGLR,&local_20);
  local_20 = 0x200;
  uStack_18 = 0x10;
  uStack_1c = 1;
  GPIO_Init(GPIOA->CFGLR,&local_20);
  GPIO_SetBits(GPIOA->CFGLR,0x200);
  uStack_14 = 0x1040000;
  uStack_10 = 0x20000;
  uStack_c = 0x2000001;
  uStack_8 = 0;
  SPI1_CTLR1_merge(SPI1->CTLR1,&uStack_14);
  SPI1_CTLR1_write(SPI1->CTLR1,2,1);
  SPI1_CTLR1_write(SPI1->CTLR1,1,1);
  SPI1_Cmd(SPI1->CTLR1,1);
  return;
}

/* ------------------------------------------------------------------
 * spi1_gpio_init2   @ 0x1CFC
 * SPI1 + CS + D/C pins (second copy)
 * Signature: undefined FUN_ram_00001cfc()
 * ------------------------------------------------------------------ */
void spi1_gpio_init2(void)

{
  undefined4 local_20;
  undefined4 uStack_1c;
  undefined4 uStack_18;
  undefined4 uStack_14;
  undefined4 uStack_10;
  undefined4 uStack_c;
  undefined4 uStack_8;
  undefined2 uStack_4;
  
  __riscv_save_8();
  local_20 = 0;
  uStack_1c = 0;
  uStack_18 = 0;
  uStack_14 = 0;
  uStack_10 = 0;
  uStack_c = 0;
  uStack_8 = 0;
  uStack_4 = 0;
  RCC_APB2PeriphClockCmd(0x1014,1);
  RCC_APB2PeriphClockCmd(8,1);
  local_20 = 0x80;
  uStack_18 = 0x10;
  uStack_1c = 1;
  GPIO_Init(GPIOB->CFGLR,&local_20);
  local_20 = 1;
  uStack_18 = 0x10;
  uStack_1c = 1;
  GPIO_Init(GPIOA->CFGLR,&local_20);
  GPIO_SetBits(GPIOA->CFGLR,1);
  RCC_APB2PeriphClockCmd(0x10,1);
  local_20 = 8;
  uStack_18 = 0x10;
  uStack_1c = 1;
  GPIO_Init(GPIOC->CFGLR,&local_20);
  GPIO_SetBits(GPIOC->CFGLR,8);
  local_20 = 0x20;
  uStack_18 = 0x18;
  uStack_1c = 1;
  GPIO_Init(GPIOA->CFGLR,&local_20);
  local_20 = 0x40;
  uStack_18 = 0x10;
  uStack_1c = 1;
  GPIO_Init(GPIOA->CFGLR,&local_20);
  GPIO_SetBits(GPIOA->CFGLR,0x40);
  local_20 = 0x80;
  uStack_18 = 0x18;
  uStack_1c = 1;
  GPIO_Init(GPIOA->CFGLR,&local_20);
  local_20 = 0x200;
  uStack_18 = 0x10;
  uStack_1c = 1;
  GPIO_Init(GPIOA->CFGLR,&local_20);
  GPIO_SetBits(GPIOA->CFGLR,0x200);
  uStack_14 = 0x1040000;
  uStack_10 = 0x20000;
  uStack_c = 0x2000001;
  uStack_8 = 0;
  SPI1_CTLR1_merge(SPI1->CTLR1,&uStack_14);
  SPI1_CTLR1_write(SPI1->CTLR1,2,1);
  SPI1_CTLR1_write(SPI1->CTLR1,1,1);
  SPI1_Cmd(SPI1->CTLR1,1);
  return;
}

/* ------------------------------------------------------------------
 * flash_read_status   @ 0x1E32
 * 0x05
 * Signature: undefined FUN_ram_00001e32()
 * ------------------------------------------------------------------ */
undefined4 flash_read_status(void)

{
  undefined4 uVar1;
  
  __riscv_save_3();
  GPIO_WriteBit(GPIOB->CFGLR,0x80,0);
  spi_xfer_byte(5);
  uVar1 = spi_xfer_byte(0xff);
  GPIO_WriteBit(GPIOB->CFGLR,0x80,1);
  return uVar1;
}

/* ------------------------------------------------------------------
 * flash_wait_busy   @ 0x1E6C
 * poll BUSY, no timeout
 * Signature: undefined FUN_ram_00001e6c()
 * ------------------------------------------------------------------ */
void flash_wait_busy(void)

{
  uint uVar1;
  
  __riscv_save_3();
  do {
    uVar1 = flash_read_status();
  } while ((uVar1 & 1) != 0);
  return;
}

/* ------------------------------------------------------------------
 * flash_write_enable   @ 0x1E7A
 * 0x06
 * Signature: undefined FUN_ram_00001e7a()
 * ------------------------------------------------------------------ */
void flash_write_enable(void)

{
  __riscv_save_3();
  GPIO_WriteBit(GPIOB->CFGLR,0x80,0);
  spi_xfer_byte(6);
  GPIO_WriteBit(GPIOB->CFGLR,0x80,1);
  return;
}

/* ------------------------------------------------------------------
 * flash_read_id   @ 0x1EA8
 * 0x90
 * Signature: undefined FUN_ram_00001ea8()
 * ------------------------------------------------------------------ */
ushort flash_read_id(void)

{
  short sVar1;
  ushort uVar2;
  
  __riscv_save_3();
  GPIO_WriteBit(GPIOB->CFGLR,0x80,0);
  spi_xfer_byte(0x90);
  spi_xfer_byte(0);
  spi_xfer_byte(0);
  spi_xfer_byte(0);
  sVar1 = spi_xfer_byte(0xff);
  uVar2 = spi_xfer_byte(0xff);
  GPIO_WriteBit(GPIOB->CFGLR,0x80,1);
  return sVar1 << 8 | uVar2;
}

/* ------------------------------------------------------------------
 * flash_read_data   @ 0x1F0A
 * 0x03
 * Signature: undefined FUN_ram_00001f0a()
 * ------------------------------------------------------------------ */
void flash_read_data(void)

{
  uint uVar1;
  undefined1 uVar2;
  uint extraout_a2;
  undefined8 uVar3;
  
  uVar3 = __riscv_save_3();
  uVar1 = (uint)((ulonglong)uVar3 >> 0x20);
  GPIO_WriteBit(GPIOB->CFGLR,0x80,0);
  spi_xfer_byte(3);
  spi_xfer_byte(uVar1 >> 0x10 & 0xff);
  spi_xfer_byte(uVar1 >> 8 & 0xff);
  spi_xfer_byte(uVar1 & 0xff);
  for (uVar1 = 0; (uVar1 & 0xffff) < extraout_a2; uVar1 = uVar1 + 1) {
    uVar2 = spi_xfer_byte(0xff);
    *(undefined1 *)((int)uVar3 + uVar1) = uVar2;
  }
  GPIO_WriteBit(GPIOB->CFGLR,0x80,1);
  return;
}

/* ------------------------------------------------------------------
 * flash_read_page   @ 0x1F80
 * 0x03 by page number
 * Signature: undefined FUN_ram_00001f80()
 * ------------------------------------------------------------------ */
undefined4 flash_read_page(void)

{
  uint uVar1;
  undefined4 uVar2;
  undefined4 uStack_c;
  undefined4 uStack_8;
  undefined4 uStack_4;
  
  uVar1 = __riscv_save_3();
  uStack_c = 0x40;
  uStack_4 = 4;
  uStack_8 = 0;
  GPIO_Init(GPIOA->CFGLR,&uStack_c);
  GPIO_WriteBit(GPIOB->CFGLR,0x80,0);
  spi_xfer_byte(3);
  spi_xfer_byte(uVar1 >> 0x10 & 0xff);
  spi_xfer_byte(uVar1 >> 8 & 0xff);
  spi_xfer_byte(uVar1 & 0xff);
  uVar2 = spi_xfer_byte(0xff);
  GPIO_WriteBit(GPIOB->CFGLR,0x80,1);
  return uVar2;
}

/* ------------------------------------------------------------------
 * flash_read_security   @ 0x1FF6
 * 0x48
 * Signature: undefined FUN_ram_00001ff6()
 * ------------------------------------------------------------------ */
void flash_read_security(void)

{
  int iVar1;
  undefined1 uVar2;
  int iVar3;
  
  iVar3 = __riscv_save_3();
  GPIO_WriteBit(GPIOB->CFGLR,0x80,0);
  spi_xfer_byte(0x48);
  spi_xfer_byte(0);
  spi_xfer_byte(0x10);
  spi_xfer_byte(0);
  spi_xfer_byte(0xff);
  iVar1 = 0;
  do {
    uVar2 = spi_xfer_byte(0xff);
    *(undefined1 *)(iVar3 + iVar1) = uVar2;
    iVar1 = iVar1 + 1;
  } while (iVar1 != 0x3c);
  GPIO_WriteBit(GPIOB->CFGLR,0x80,1);
  return;
}

/* ------------------------------------------------------------------
 * flash_fast_read   @ 0x2060
 * 0x0B + DMA
 * Signature: undefined FUN_ram_00002060()
 * ------------------------------------------------------------------ */
void flash_fast_read(void)

{
  int iVar1;
  int extraout_a2;
  undefined8 uVar2;
  
  uVar2 = __riscv_save_8();
  iVar1 = (int)uVar2;
  DMA1_CH2->CFGR = DMA1_CH2->CFGR & 0xfffe;
  DMA1_CH3->CFGR = DMA1_CH3->CFGR & 0xfffe;
  DMA1_CH3->MADDR = iVar1 + 1;
  DMA1_CH2->MADDR = iVar1;
  GPIO_WriteBit(GPIOB->CFGLR,0x80,0);
  *(undefined1 *)(iVar1 + 1) = 0xb;
  *(char *)(iVar1 + 2) = (char)((ulonglong)uVar2 >> 0x30);
  *(char *)(iVar1 + 3) = (char)((ulonglong)uVar2 >> 0x28);
  *(char *)(iVar1 + 4) = (char)((ulonglong)uVar2 >> 0x20);
  DMA1_CH3->CNTR = extraout_a2 + 5;
  DMA1_CH2->CNTR = extraout_a2 + 6;
  DMA1_CH2->CFGR = DMA1_CH2->CFGR | 1;
  DMA1_CH3->CFGR = DMA1_CH3->CFGR | 1;
  do {
    iVar1 = DMA_GetFlagStatus(0x200);
  } while (iVar1 == 0);
  DMA_ClearFlag(0x200);
  GPIO_WriteBit(GPIOB->CFGLR,0x80,1);
  return;
}

/* ------------------------------------------------------------------
 * flash_fast_read_dma   @ 0x20F8
 * 0x0B + DMA, overlapping CPU expansion
 * Signature: undefined FUN_ram_000020f8()
 * ------------------------------------------------------------------ */
void flash_fast_read_dma(void)

{
  undefined1 *puVar1;
  int iVar2;
  char *pcVar3;
  uint extraout_a2;
  int extraout_a3;
  uint extraout_a4;
  uint uVar4;
  undefined2 extraout_a5;
  undefined8 uVar5;
  
  uVar5 = __riscv_save_8();
  puVar1 = (undefined1 *)uVar5;
  DMA1_CH2->CFGR = DMA1_CH2->CFGR & 0xfffe;
  DMA1_CH3->CFGR = DMA1_CH3->CFGR & 0xfffe;
  DMA1_CH3->MADDR = puVar1 + 1;
  DMA1_CH2->MADDR = puVar1;
  GPIO_WriteBit(GPIOB->CFGLR,0x80,0);
  puVar1[1] = 0xb;
  puVar1[2] = (char)((ulonglong)uVar5 >> 0x30);
  puVar1[3] = (char)((ulonglong)uVar5 >> 0x28);
  puVar1[4] = (char)((ulonglong)uVar5 >> 0x20);
  DMA1_CH2->CNTR = extraout_a2 + 5;
  DMA1_CH2->CFGR = DMA1_CH2->CFGR | 1;
  DMA1_CH3->CFGR = DMA1_CH3->CFGR | 1;
  do {
    uVar4 = extraout_a4;
    DMA1_CH3->CNTR = DMA1_CH2->CNTR;
  } while (extraout_a2 < DMA1_CH2->CNTR);
  for (; uVar4 < ((extraout_a2 >> 1) + extraout_a4 & 0xffff); uVar4 = uVar4 + 1 & 0xffff) {
    pcVar3 = (char *)((uVar4 >> 3) + extraout_a3);
    if (*pcVar3 < '\0') {
      *puVar1 = (char)((ushort)extraout_a5 >> 8);
      puVar1[1] = (char)extraout_a5;
    }
    else {
      *puVar1 = puVar1[5];
      puVar1[1] = puVar1[6];
    }
    *pcVar3 = *pcVar3 << 1;
    puVar1 = puVar1 + 2;
  }
  do {
    iVar2 = DMA_GetFlagStatus(0x200);
  } while (iVar2 == 0);
  DMA_ClearFlag(0x200);
  do {
    iVar2 = DMA_GetFlagStatus(0x20);
  } while (iVar2 == 0);
  DMA_ClearFlag(0x20);
  GPIO_WriteBit(GPIOB->CFGLR,0x80,1);
  return;
}

/* ------------------------------------------------------------------
 * flash_page_program   @ 0x2202
 * 0x06 + 0x81 + 0x02
 * Signature: undefined FUN_ram_00002202()
 * ------------------------------------------------------------------ */
void flash_page_program(void)

{
  uint uVar1;
  int iVar2;
  int extraout_a1;
  int extraout_a2;
  uint uVar3;
  undefined4 uStack_c;
  undefined4 uStack_8;
  undefined4 uStack_4;
  
  __riscv_save_12();
  uStack_c = 0x40;
  uStack_8 = 0;
  uStack_4 = 4;
  GPIO_Init(GPIOA->CFGLR,&uStack_c);
  uVar1 = extraout_a1 * 0x100;
  uVar3 = extraout_a2 * 0x100 + uVar1;
  for (; uVar1 != uVar3; uVar1 = uVar1 + 0x100) {
    flash_write_enable();
    flash_wait_busy();
    GPIO_WriteBit(GPIOB->CFGLR,0x80,0);
    spi_xfer_byte(0x81);
    spi_xfer_byte(uVar1 >> 0x10 & 0xff);
    spi_xfer_byte(uVar1 >> 8 & 0xff);
    spi_xfer_byte(0);
    GPIO_WriteBit(GPIOB->CFGLR,0x80,1);
    flash_wait_busy();
    flash_write_enable();
    flash_wait_busy();
    GPIO_WriteBit(GPIOB->CFGLR,0x80,0);
    g_param_records = 2;
    DAT_ram_2000012d = (undefined1)(uVar1 >> 0x10);
    DAT_ram_2000012e = (undefined1)(uVar1 >> 8);
    DAT_ram_2000012f = 0;
    DMA_Cmd(&DMA1_CH3->CFGR,0);
    DMA1_CH3->MADDR = &g_param_records;
    DMA1_CH3->CNTR = 0x104;
    DMA_Cmd(&DMA1_CH3->CFGR,1);
    do {
      iVar2 = DMA_GetFlagStatus(0x200);
    } while (iVar2 == 0);
    DMA_ClearFlag(0x200);
    GPIO_WriteBit(GPIOB->CFGLR,0x80,1);
    flash_wait_busy();
  }
  return;
}

/* ------------------------------------------------------------------
 * flash_page_program2   @ 0x2306
 * 0x06 + 0x81 + 0x02 (second copy)
 * Signature: undefined FUN_ram_00002306()
 * ------------------------------------------------------------------ */
void flash_page_program2(void)

{
  int iVar1;
  int iVar2;
  int extraout_a1;
  int extraout_a2;
  int iVar3;
  undefined4 uStack_c;
  undefined4 uStack_8;
  undefined4 uStack_4;
  
  __riscv_save_8();
  iVar1 = extraout_a1 * 0x100;
  uStack_c = 0x40;
  uStack_8 = 0;
  uStack_4 = 4;
  iVar3 = extraout_a2 * 0x100 + iVar1;
  GPIO_Init(GPIOA->CFGLR,&uStack_c);
  for (; iVar1 != iVar3; iVar1 = iVar1 + 0x100) {
    flash_write_enable();
    flash_wait_busy();
    GPIO_WriteBit(GPIOB->CFGLR,0x80,0);
    DAT_ram_2000012d = (undefined1)((uint)iVar1 >> 0x10);
    DAT_ram_2000012e = (undefined1)((uint)iVar1 >> 8);
    g_param_records = 2;
    DAT_ram_2000012f = 0;
    DMA_Cmd(&DMA1_CH3->CFGR,0);
    DMA1_CH3->MADDR = &g_param_records;
    DMA1_CH3->CNTR = 0x104;
    DMA_Cmd(&DMA1_CH3->CFGR,1);
    do {
      iVar2 = DMA_GetFlagStatus(0x200);
    } while (iVar2 == 0);
    DMA_ClearFlag(0x200);
    GPIO_WriteBit(GPIOB->CFGLR,0x80,1);
    flash_wait_busy();
  }
  return;
}

/* ------------------------------------------------------------------
 * flash_page_erase   @ 0x23BE
 * 0x06 + vendor 0x81
 * Signature: undefined FUN_ram_000023be()
 * ------------------------------------------------------------------ */
void flash_page_erase(void)

{
  uint uVar1;
  
  uVar1 = __riscv_save_3();
  flash_write_enable();
  flash_wait_busy();
  GPIO_WriteBit(GPIOB->CFGLR,0x80,0);
  spi_xfer_byte(0x81);
  spi_xfer_byte(uVar1 >> 8);
  spi_xfer_byte(uVar1 & 0xff);
  spi_xfer_byte(0);
  GPIO_WriteBit(GPIOB->CFGLR,0x80,1);
  flash_wait_busy();
  return;
}

/* ------------------------------------------------------------------
 * flash_sector_erase   @ 0x2412
 * 0x06 + 0x20
 * Signature: undefined FUN_ram_00002412()
 * ------------------------------------------------------------------ */
void flash_sector_erase(void)

{
  uint uVar1;
  
  uVar1 = __riscv_save_3();
  flash_write_enable();
  flash_wait_busy();
  GPIO_WriteBit(GPIOB->CFGLR,0x80,0);
  spi_xfer_byte(0x20);
  spi_xfer_byte(uVar1 >> 8);
  spi_xfer_byte(uVar1 & 0xff);
  spi_xfer_byte(0);
  GPIO_WriteBit(GPIOB->CFGLR,0x80,1);
  flash_wait_busy();
  return;
}

/* ------------------------------------------------------------------
 * flash_block_erase   @ 0x2466
 * 0x06 + 0x52
 * Signature: undefined FUN_ram_00002466()
 * ------------------------------------------------------------------ */
void flash_block_erase(void)

{
  uint uVar1;
  
  uVar1 = __riscv_save_3();
  flash_write_enable();
  flash_wait_busy();
  GPIO_WriteBit(GPIOB->CFGLR,0x80,0);
  spi_xfer_byte(0x52);
  spi_xfer_byte(uVar1 >> 8);
  spi_xfer_byte(uVar1 & 0xff);
  spi_xfer_byte(0);
  GPIO_WriteBit(GPIOB->CFGLR,0x80,1);
  flash_wait_busy();
  return;
}

/* ------------------------------------------------------------------
 * flash_erase_page_n   @ 0x24BA
 * Signature: undefined FUN_ram_000024ba()
 * ------------------------------------------------------------------ */
void flash_erase_page_n(void)

{
  uint uVar1;
  
  uVar1 = __riscv_save_3();
  flash_write_enable();
  flash_wait_busy();
  GPIO_WriteBit(GPIOB->CFGLR,0x80,0);
  spi_xfer_byte(0xd8);
  spi_xfer_byte(uVar1 >> 8);
  spi_xfer_byte(uVar1 & 0xff);
  spi_xfer_byte(0);
  GPIO_WriteBit(GPIOB->CFGLR,0x80,1);
  flash_wait_busy();
  return;
}

/* ------------------------------------------------------------------
 * flash_erase_range   @ 0x250E
 * page/sector/block planner
 * Signature: undefined FUN_ram_0000250e()
 * ------------------------------------------------------------------ */
void flash_erase_range(void)

{
  ulonglong uVar1;
  uint uVar2;
  uint uVar3;
  uint uVar4;
  uint uVar5;
  uint uVar6;
  int iVar7;
  ulonglong uVar8;
  undefined4 uStack_c;
  undefined4 uStack_8;
  undefined4 uStack_4;
  
  uVar8 = __riscv_save_8();
  uVar1 = uVar8 >> 0x20;
  uVar2 = (uint)uVar8;
  uStack_c = 0x40;
  uStack_4 = 4;
  uStack_8 = 0;
  GPIO_Init(GPIOA->CFGLR,&uStack_c);
  uVar6 = (uVar2 & 0x7f) >> 4;
  uVar5 = uVar2 >> 7 & 1;
  if ((uVar8 & 0xf) != 0) {
    uVar6 = uVar6 + 1;
  }
  uVar6 = uVar6 & 7;
  if (uVar6 != 0) {
    uVar5 = uVar5 + 1;
  }
  for (; (uVar3 = (uint)(uVar8 >> 0x20), (uVar8 & 0xf) != 0 && (uVar3 != 0));
      uVar8 = CONCAT44(uVar3 - 1,((uint)uVar8 & 0xf) + 1) & 0xffffffffffff) {
    flash_page_erase();
  }
  iVar7 = uVar3 + ((uVar2 + (int)uVar1) - uVar3 & 0xffff);
  for (; uVar2 = iVar7 - uVar3 & 0xffff, uVar6 != 0; uVar6 = uVar6 + 1 & 7) {
    if (uVar3 < 0x10) goto LAB_ram_0000257c;
    flash_sector_erase(uVar2);
    uVar3 = uVar3 - 0x10 & 0xffff;
  }
  if (((uVar5 & 1) != 0) && (0x7f < uVar3)) {
    flash_block_erase(uVar2);
    uVar2 = uVar2 + 0x80 & 0xffff;
    uVar3 = uVar3 - 0x80 & 0xffff;
  }
LAB_ram_0000257c:
  for (uVar5 = uVar3; 0xff < uVar5; uVar5 = uVar5 - 0x100 & 0xffff) {
    flash_erase_page_n((uVar2 + uVar3) - uVar5 & 0xffff);
  }
  uVar2 = uVar2 + (uVar3 & 0xffffff00) & 0xffff;
  uVar3 = uVar3 + (uVar3 >> 8) * -0x100;
  uVar6 = (uVar3 & 0x7f) >> 4;
  uVar5 = uVar3 & 0xf;
  if ((uVar3 & 0xffff) >> 7 != 0) {
    flash_block_erase(uVar2);
    uVar2 = uVar2 + 0x80 & 0xffff;
  }
  uVar4 = uVar2;
  for (uVar3 = uVar6; (uVar3 & 0xff) != 0; uVar3 = (uVar3 & 0xff) - 1) {
    flash_sector_erase(uVar4);
    uVar4 = uVar4 + 0x10 & 0xffff;
  }
  uVar2 = uVar2 + uVar6 * 0x10;
  for (; uVar6 = uVar2 & 0xffff, uVar5 != 0; uVar5 = uVar5 - 1 & 0xff) {
    uVar2 = uVar6 + 1;
    flash_page_erase(uVar6);
  }
  return;
}
