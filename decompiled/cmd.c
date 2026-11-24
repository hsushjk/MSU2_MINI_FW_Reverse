/* MSU2_MINI firmware 1.1 -- reconstructed C, module "cmd".
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
 * cmd_dispatch   @ 0x0250
 * host command dispatcher (6-byte record)
 * Signature: undefined FUN_ram_00000250()
 *
 * WCH 16-bit load/store sites (quadrant-2 custom opcodes, invisible to the decompiler):
 *   0x02E0  sh a4,0x0(a5)
 *   0x030E  lhu a4,0x0(a5)
 * ------------------------------------------------------------------ */
void cmd_dispatch(void)

{
  byte bVar1;
  byte *pbVar2;
  undefined4 uVar3;
  short sVar4;
  
  pbVar2 = (byte *)__riscv_save_3();
  bVar1 = *pbVar2;
  if (bVar1 == 2) {
    switch(pbVar2[1]) {
    case 0:
      g_rect_x0 = CONCAT11(pbVar2[2],pbVar2[3]);
      g_rect_y0 = CONCAT11(pbVar2[4],pbVar2[5]);
      break;
    case 1:
      g_rect_x1 = CONCAT11(pbVar2[2],pbVar2[3]);
      g_rect_y1 = CONCAT11(pbVar2[4],pbVar2[5]);
      break;
    case 2:
      g_page_a = CONCAT11(pbVar2[2],pbVar2[3]);
      *(ushort *)(gp + -2000) = CONCAT11(pbVar2[4],pbVar2[5]);
      break;
    case 3:
      switch(pbVar2[2]) {
      case 0:
        lcd_fill_rect(g_rect_x0,g_rect_y0,
                         g_rect_x1,g_rect_y1,
                         CONCAT11(pbVar2[3],pbVar2[4]));
        break;
      case 1:
        lcd_blit_colour_bg(g_rect_x0,g_rect_y0,
                         g_rect_x1,g_rect_y1,
                         CONCAT11(pbVar2[3],pbVar2[4]),g_page_a,
                         *(undefined2 *)(gp + -2000));
        break;
      case 2:
        lcd_mono_image_a(g_rect_x0,g_rect_y0,pbVar2[3],
                         g_page_a,*(undefined2 *)(gp + -2000),
                         CONCAT11(pbVar2[4],pbVar2[5]));
        break;
      case 3:
        lcd_mono_image(g_rect_x0,g_rect_y0,pbVar2 + 3,
                         g_page_a,*(undefined2 *)(gp + -2000));
        break;
      case 4:
        lcd_image_with_bg(g_rect_x0,g_rect_y0,
                         g_rect_x1,g_rect_y1,
                         CONCAT11(pbVar2[3],pbVar2[4]),g_page_a,
                         *(undefined2 *)(gp + -2000));
        break;
      case 5:
        lcd_mono_image_b(g_rect_x0,g_rect_y0,pbVar2[3],
                         g_page_a,*(undefined2 *)(gp + -2000),
                         CONCAT11(pbVar2[4],pbVar2[5]));
        break;
      case 6:
        lcd_mono_image_mix(g_rect_x0,g_rect_y0,pbVar2 + 3,
                         g_page_a,*(undefined2 *)(gp + -2000));
        break;
      case 7:
        lcd_draw_hline(g_rect_x0,g_rect_y0,
                         g_rect_x1,g_rect_y1);
        break;
      case 8:
        lcd_stream(gp + -0x750,CONCAT11(pbVar2[3],pbVar2[4]));
        return;
      case 9:
        lcd_stream_end();
        return;
      case 10:
        g_display_rotation = pbVar2[3];
        lcd_init();
        break;
      case 0xb:
        lcd_blit_colour(g_rect_x0,g_rect_y0,
                         g_rect_x1,g_rect_y1,
                         CONCAT11(pbVar2[3],pbVar2[4]));
        break;
      default:
        goto switchD_ram_00000352_default;
      }
      goto LAB_ram_00000296;
    case 4:
      lcd_set_tile(gp + -0x750,pbVar2 + 2);
    }
  }
  else {
    if (bVar1 < 3) {
      if (bVar1 != 0) {
        return;
      }
      if (pbVar2[1] == 0x30) {
        bVar1 = pbVar2[2] >> 5;
        if (bVar1 == 1) {
          sVar4 = 0;
          if (pbVar2[3] < 0x10) {
            sVar4 = (ushort)pbVar2[3] << 1;
          }
          g_filter_k = sVar4;
          pbVar2[4] = (byte)((ushort)sVar4 >> 8);
          pbVar2[5] = (byte)sVar4;
        }
        else {
          if (bVar1 == 0) {
            uVar3 = lut_get(CONCAT11(pbVar2[3],pbVar2[4]));
            *(char *)(gp + -0x7aa) = (char)uVar3;
            goto LAB_ram_000002f6;
          }
          if (bVar1 == 4) {
            if (CONCAT11(pbVar2[3],pbVar2[4]) < 0x100) {
              *(byte *)((uint)CONCAT11(pbVar2[3],pbVar2[4]) + gp + -0x59c) = pbVar2[5];
            }
          }
          else if (bVar1 != 5) {
            return;
          }
        }
      }
      else if (pbVar2[1] != 0x4d) {
        return;
      }
    }
    else if (bVar1 == 3) {
      bVar1 = pbVar2[1];
      if (bVar1 == 1) {
        flash_page_program(gp + -0x754,(uint)CONCAT21(CONCAT11(pbVar2[2],pbVar2[3]),pbVar2[4]),
                         pbVar2[5]);
      }
      else {
        if (bVar1 == 0) {
          uVar3 = flash_read_page((uint)CONCAT21(CONCAT11(pbVar2[2],pbVar2[3]),pbVar2[4]));
          goto LAB_ram_000002f6;
        }
        if (bVar1 == 2) {
          flash_erase_range(CONCAT11(pbVar2[2],pbVar2[3]),CONCAT11(pbVar2[4],pbVar2[5]));
        }
        else {
          if (bVar1 != 3) {
            return;
          }
          flash_page_program2(gp + -0x754,(uint)CONCAT21(CONCAT11(pbVar2[2],pbVar2[3]),pbVar2[4]),
                           pbVar2[5]);
        }
      }
    }
    else {
      if (bVar1 != 8) {
        return;
      }
      uVar3 = adc_read(pbVar2[1]);
      g_last_read = (short)uVar3;
      pbVar2[4] = (byte)((uint)uVar3 >> 8);
LAB_ram_000002f6:
      pbVar2[5] = (byte)uVar3;
    }
LAB_ram_00000296:
    memcpy6(gp + -0x7bc,pbVar2);
    usb_ep_send(3,gp + -0x7bc,6,1);
  }
switchD_ram_00000352_default:
  return;
}

/* ------------------------------------------------------------------
 * reset_session   @ 0x0584
 * reset tick/state/ring pointers
 * Signature: undefined FUN_ram_00000584()
 * ------------------------------------------------------------------ */
void reset_session(void)

{
  g_tick = 0;
  *(undefined1 *)(gp + -0x7c2) = 0;
  *(undefined1 *)(gp + -0x7c1) = 0;
  g_main_state = 0;
  g_rx_cmd = 0;
  return;
}

/* ------------------------------------------------------------------
 * main_loop   @ 0x059A
 * command + streaming loop
 * Signature: undefined FUN_ram_0000059a()
 *
 * WCH 16-bit load/store sites (quadrant-2 custom opcodes, invisible to the decompiler):
 *   0x07FA  lhu a4,0x0(s0)
 *   0x083C  lhu a4,0x0(s0)
 *   0x084E  lhu a5,0x0(a4)
 *   0x085C  sh a5,0x0(a4)
 *   0x085E  lhu a5,0x0(s0)
 *   0x0888  lhu a5,0x0(s0)
 *   0x089A  sh a5,0x0(s0)
 *   0x08B0  lhu a4,0x0(s0)
 *   0x08BA  lhu a5,0x0(s0)
 *   0x08C4  sh a5,0x0(s0)
 *   0x0A44  sh a4,0x0(s0)
 *   0x0A88  sh a4,0x0(s0)
 * ------------------------------------------------------------------ */
void main_loop(void)

{
  char cVar1;
  byte *pbVar2;
  undefined2 uVar3;
  int iVar4;
  char *pcVar5;
  uint extraout_a4;
  uint uVar6;
  uint uVar7;
  uint extraout_a4_00;
  byte bVar8;
  uint uVar9;
  char *pcVar10;
  int extraout_a5;
  byte *pbVar11;
  byte *pbVar12;
  int iVar13;
  int iVar14;
  
  __riscv_save_12();
  if (g_main_state != '\0') {
    if (g_main_state != '\x01') {
      return;
    }
    NVIC->ICER[1] = 0x2000;
    bVar8 = *(byte *)((uint)*(ushort *)(gp + 0x766) * 2 + gp + 0x76a);
    uVar6 = (uint)bVar8;
    g_frame_state = bVar8;
    pbVar12 = (byte *)(gp + -0x7c0);
    pbVar11 = (byte *)(gp + -0x7c1);
    pbVar2 = (byte *)(gp + -0x7c2);
    if (uVar6 == 0) {
      if (4999 < g_tick) {
        reset_session();
      }
    }
    else {
      uVar9 = (uint)*pbVar11;
      uVar7 = (uint)*pbVar2;
      if (uVar9 <= uVar7) {
        uVar9 = uVar9 - 0x70;
      }
      *(char *)(gp + -0x7b6) = (char)(uVar9 - uVar7);
      if (uVar6 <= (uVar9 - uVar7 & 0xff)) {
        g_tick = 0;
        iVar13 = gp + 0x364;
        iVar14 = gp + -0x64c;
        if (uVar7 + uVar6 < 0x91) {
          DMA_Cmd(DMA1_CH8->CFGR,0);
          DMA1_CH8->CNTR = (uint)*pbVar12;
          DMA1_CH8->PADDR = iVar13 + (uint)*(ushort *)(gp + 0x766) * 0x40;
          DMA1_CH8->MADDR = iVar14 + (uint)*pbVar2;
          DMA_Cmd(DMA1_CH8->CFGR,1);
          bVar8 = (byte)(((uint)*pbVar2 + (uint)*pbVar12 & 0xff) % 0x90);
        }
        else {
          DMA_Cmd(DMA1_CH8->CFGR,0);
          DMA1_CH8->CNTR = 0x90 - (uint)*pbVar2;
          DMA1_CH8->MADDR = (uint)*pbVar2 + iVar14;
          DMA1_CH8->PADDR = (uint)*(ushort *)(gp + 0x766) * 0x40 + iVar13;
          DMA_ClearFlag(0x20000000);
          DMA_Cmd(DMA1_CH8->CFGR,1);
          do {
            iVar4 = DMA_GetFlagStatus(0x20000000);
          } while (iVar4 == 0);
          DMA_Cmd(DMA1_CH8->CFGR,0);
          DMA1_CH8->CNTR = (uint)*pbVar12 - (0x90 - (uint)*pbVar2);
          DMA1_CH8->PADDR = (uint)*(ushort *)(gp + 0x766) * 0x40 + (0x90 - (uint)*pbVar2) + iVar13
          ;
          DMA1_CH8->MADDR = iVar14;
          DMA_Cmd(DMA1_CH8->CFGR,1);
          bVar8 = *pbVar2 + *pbVar12 + 0x70;
        }
        *pbVar2 = bVar8;
        iVar13 = (uint)*(ushort *)(gp + 0x766) * 2 + gp + 0x764;
        *(undefined1 *)(iVar13 + 0xe) = 0;
        *(undefined1 *)(iVar13 + 0xf) = 0;
        iVar13 = *(ushort *)(gp + 0x766) + 1;
        *(char *)(gp + 0x766) = (char)iVar13;
        *(char *)(gp + 0x767) = (char)((uint)iVar13 >> 8);
        if (0xf < *(ushort *)(gp + 0x766)) {
          *(undefined1 *)(gp + 0x76e) = 0;
          *(undefined1 *)(gp + 0x76f) = 0;
        }
        iVar13 = *(ushort *)(gp + 0x768) - 1;
        *(char *)(gp + 0x768) = (char)iVar13;
        *(char *)(gp + 0x769) = (char)((uint)iVar13 >> 8);
      }
    }
    USBFSD->UEP2_CTRL_H = USBFSD->UEP2_CTRL_H & 0xfff3;
    NVIC->ISER[1] = 0x2000;
    while (*pbVar11 / 6 != *pbVar2 / 6) {
      pcVar5 = (char *)((uint)*pbVar11 + gp + -0x64c);
      if (*pcVar5 == '\x04') {
        pcVar10 = (char *)(((byte)pcVar5[1] + 1) * 4 + gp + -0x754);
        *pcVar10 = pcVar5[2];
        pcVar10[1] = pcVar5[3];
        pcVar10[2] = pcVar5[4];
        pcVar10[3] = pcVar5[5];
      }
      else {
        cmd_dispatch();
      }
      *pbVar11 = (byte)((*pbVar11 + 6 & 0xff) % 0x90);
    }
    return;
  }
  if (extraout_a4 < 100) goto LAB_ram_000008c6;
  memcpy6(gp + -0x7bc,gp + -0x7e0);
  g_tick = 0;
  usb_ep_send(3,gp + -0x7bc,6,1);
  uVar6 = (uint)g_rx_cmd;
  if (uVar6 == 0x12) {
    uVar3 = adc_read(9);
    g_adc_value = uVar3;
    uVar6 = extraout_a4_00;
  }
  iVar13 = 0x13;
  if (uVar6 == 0x13) {
    iVar13 = adc_read(9);
    iVar13 = ((extraout_a5 + iVar13 & 0xffffU) >> 1) - 0x7d;
  }
  iVar14 = 0x23;
  if ((iVar13 - 0x14U & 0xffff) < 0x24) {
    lcd_fill_rect(0,0,0xa0,0x50,(iVar13 - 0x14U) * 100);
    uVar6 = adc_read(9);
    pcVar5 = (char *)(gp + -0x7c4);
    if (uVar6 < g_adc_value) {
      iVar14 = 1;
      *pcVar5 = '\x01';
      goto code_r0x000008b2;
    }
    iVar14 = 1;
    if (*pcVar5 != '\x01') {
      *pcVar5 = '\0';
      goto code_r0x000008b2;
    }
    *pcVar5 = '\0';
  }
  else {
code_r0x000008b2:
    if (iVar14 != 0x38) goto LAB_ram_000008c6;
  }
  lcd_fill_rect(0,0,0xa0,0x50,0xef2);
  uVar6 = adc_read(9);
  if (uVar6 < g_adc_value) {
    *(undefined1 *)(gp + -0x7c4) = 1;
  }
  else {
    *(undefined1 *)(gp + -0x7c4) = 0;
  }
LAB_ram_000008c6:
  NVIC->ICER[1] = 0x2000;
  cVar1 = *(char *)((uint)*(ushort *)(gp + 0x766) * 2 + gp + 0x76a);
  g_frame_state = cVar1;
  if (cVar1 != '\0') {
    if ((((cVar1 == '\x06') &&
         (iVar13 = gp + 0x364, *(char *)((uint)*(ushort *)(gp + 0x766) * 0x40 + iVar13) == '\0')) &&
        (*(char *)((uint)*(ushort *)(gp + 0x766) * 0x40 + iVar13 + 1) == 'M')) &&
       (((*(char *)((uint)*(ushort *)(gp + 0x766) * 0x40 + iVar13 + 2) == 'S' &&
         (*(char *)((uint)*(ushort *)(gp + 0x766) * 0x40 + iVar13 + 3) == 'N')) &&
        ((*(char *)((uint)*(ushort *)(gp + 0x766) * 0x40 + iVar13 + 4) == 'C' &&
         (*(char *)((uint)*(ushort *)(gp + 0x766) * 0x40 + iVar13 + 5) == 'N')))))) {
      memcpy6(gp + -0x7bc,gp + -0x7e8);
      g_main_state = 1;
      usb_ep_send(3,gp + -0x7bc,6,1);
    }
    iVar13 = (uint)*(ushort *)(gp + 0x766) * 2 + gp + 0x764;
    *(undefined1 *)(iVar13 + 0xe) = 0;
    *(undefined1 *)(iVar13 + 0xf) = 0;
    iVar13 = *(ushort *)(gp + 0x766) + 1;
    *(char *)(gp + 0x766) = (char)iVar13;
    *(char *)(gp + 0x767) = (char)((uint)iVar13 >> 8);
    if (0xf < *(ushort *)(gp + 0x766)) {
      *(undefined1 *)(gp + 0x76e) = 0;
      *(undefined1 *)(gp + 0x76f) = 0;
    }
    iVar13 = *(ushort *)(gp + 0x768) - 1;
    *(char *)(gp + 0x768) = (char)iVar13;
    *(char *)(gp + 0x769) = (char)((uint)iVar13 >> 8);
  }
  NVIC->ISER[1] = 0x2000;
  USBFSD->UEP2_CTRL_H = USBFSD->UEP2_CTRL_H & 0xfff3;
  return;
}
