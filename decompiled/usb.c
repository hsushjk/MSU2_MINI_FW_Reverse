/* MSU2_MINI firmware 1.1 -- reconstructed C, module "usb".
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
 * vec_hardfault   @ 0x0E48
 * infinite loop
 * Signature: undefined vec_0C()
 * ------------------------------------------------------------------ */
void vec_0C(void)

{
  do {
                    /* WARNING: Do nothing block with infinite loop */
  } while( true );
}

/* ------------------------------------------------------------------
 * TIM3_IRQHandler   @ 0x0E4C
 * tick counter
 * Signature: undefined vec_DC()
 * ------------------------------------------------------------------ */
void vec_DC(void)

{
  int iVar1;
  
  iVar1 = void_428e(TIM3->CTLR1,1);
  if (iVar1 != 0) {
    void_42a6(TIM3->CTLR1,1);
    g_tick = g_tick + 1;
  }
  return;
}

/* ------------------------------------------------------------------
 * board_gpio_init   @ 0x0E7C
 * GPIO clock + mode setup
 * Signature: undefined FUN_ram_00000e7c()
 * ------------------------------------------------------------------ */
void board_gpio_init(void)

{
  __riscv_save_3();
  RCC_APB2PeriphClockCmd(1,1);
  RCC_AHBPeriphClockCmd(0x1000,1);
  return;
}

/* ------------------------------------------------------------------
 * usb_ep_init   @ 0x0E94
 * endpoint MOD/DMA setup
 * Signature: undefined FUN_ram_00000e94()
 * ------------------------------------------------------------------ */
void usb_ep_init(void)

{
  USBFSD->UEP4_1_MOD = 0x4c;
  USBFSD->UEP2_3_MOD = 0x48;
  USBFSD->UEP0_DMA = gp + 0x7ec;
  USBFSD->UEP1_DMA = 0x2000112c;
  USBFSD->UEP2_DMA = gp + 0x364;
  USBFSD->UEP3_DMA = 0x2000116c;
  USBFSD->UEP0_CTRL_H = 2;
  USBFSD->UEP2_CTRL_H = 0;
  USBFSD->UEP4_CTRL_H = 2;
  USBFSD->UEP1_CTRL = 0;
  USBFSD->UEP3_CTRL = 0;
  USBFSD->UEP1_CTRL_H = 2;
  USBFSD->UEP3_CTRL_H = 2;
  *(undefined1 *)(gp + -0x794) = 0;
  *(undefined1 *)(gp + -0x793) = 0;
  *(undefined1 *)(gp + -0x792) = 0;
  *(undefined1 *)(gp + -0x791) = 0;
  *(undefined1 *)(gp + -0x790) = 0;
  *(undefined1 *)(gp + -0x78f) = 0;
  *(undefined1 *)(gp + -0x78e) = 0;
  *(undefined1 *)(gp + -0x78d) = 0;
  return;
}

/* ------------------------------------------------------------------
 * usb_ep_reset   @ 0x0F14
 * Signature: undefined FUN_ram_00000f14()
 * ------------------------------------------------------------------ */
void usb_ep_reset(void)

{
  undefined4 uStack_c;
  undefined4 uStack_8;
  undefined4 uStack_4;
  
  __riscv_save_3();
  uStack_c = 0;
  uStack_8 = 0;
  uStack_4 = 0;
  RCC_APB2PeriphClockCmd(0x10,1);
  uStack_c = 0x10000;
  uStack_4 = 4;
  GPIO_Init(GPIOC->CFGLR,&uStack_c);
  uStack_c = 0x20000;
  uStack_4 = 0x48;
  GPIO_Init(GPIOC->CFGLR,&uStack_c);
  return;
}

/* ------------------------------------------------------------------
 * usb_device_init   @ 0x0F56
 * USBFS clock, D+ pull-up, interrupts
 * Signature: undefined FUN_ram_00000f56()
 * ------------------------------------------------------------------ */
void usb_device_init(void)

{
  undefined8 uVar1;
  
  uVar1 = __riscv_save_3();
  if ((int)((ulonglong)uVar1 >> 0x20) == 0) {
    AFIO->CTLR = AFIO->CTLR & 0xffffff30 | 0x88;
  }
  else {
    AFIO->CTLR = AFIO->CTLR & 0xffffff30 | 0xcc;
  }
  if ((int)uVar1 == 0) {
    USBFSD->BASE_CTRL = 6;
    delay_cycles(10);
    USBFSD->BASE_CTRL = 0;
    NVIC->ICER[1] = 0x2000;
  }
  else {
    usb_ep_reset();
    USBFSD->BASE_CTRL = 0;
    usb_ep_init();
    USBFSD->DEV_ADDR = 0;
    USBFSD->BASE_CTRL = 0x29;
    USBFSD->INT_FG = 0xff;
    USBFSD->UDEV_CTRL = 0x81;
    USBFSD->INT_EN = 7;
    NVIC->ISER[1] = 0x2000;
  }
  return;
}

/* ------------------------------------------------------------------
 * usb_ep_send   @ 0x0FD2
 * write len/payload to an endpoint and arm it
 * Signature: undefined FUN_ram_00000fd2()
 * ------------------------------------------------------------------ */
undefined4 usb_ep_send(char param_1)

{
  byte bVar1;
  byte *pbVar2;
  int iVar3;
  uint uVar4;
  undefined4 extraout_a2;
  int extraout_a3;
  int iVar5;
  int iVar6;
  uint uVar7;
  int extraout_a6;
  undefined8 uVar8;
  
  if (6 < (byte)(param_1 - 1U)) {
    return 1;
  }
  uVar8 = __riscv_save_8();
  uVar4 = (uint)((ulonglong)uVar8 >> 0x20);
  iVar3 = (int)uVar8;
  if (*(char *)(gp + -0x794 + iVar3) != '\0') {
    return 1;
  }
  if ((iVar3 == 1) || (iVar3 == 4)) {
    iVar5 = 1;
    bVar1 = USBFSD->UEP4_1_MOD;
LAB_ram_00001012:
    uVar7 = (uint)bVar1;
    if (iVar3 == iVar5) {
LAB_ram_00001016:
      uVar7 = uVar7 >> 4;
    }
  }
  else {
    if ((iVar3 - 2U & 0xff) < 2) {
      iVar5 = 3;
      bVar1 = USBFSD->UEP2_3_MOD;
      goto LAB_ram_00001012;
    }
    if ((iVar3 - 5U & 0xff) < 2) {
      uVar7 = (uint)USBFSD->UEP567_MOD;
      if (iVar3 != extraout_a6) goto LAB_ram_00001052;
      goto LAB_ram_00001016;
    }
    uVar7 = (uint)DAT_ram_4002340f;
  }
LAB_ram_00001052:
  if ((uVar7 & 4) == 0) {
    return 0;
  }
  iVar5 = iVar3 * 4;
  pbVar2 = &USBFSD->UEP0_CTRL_H + iVar5;
  if ((uVar7 & 8) == 0) {
    if (((uVar7 & 1) != 0) && ((*pbVar2 & 0x40) != 0)) {
      iVar6 = 0x40;
      goto LAB_ram_0000108c;
    }
    if (extraout_a3 == 0) {
      *(uint *)(&USBFSD->UEP0_DMA + iVar5) = uVar4 & 0xffff;
      goto LAB_ram_000010ce;
    }
    iVar6 = *(uint *)(&USBFSD->UEP0_DMA + iVar5) + 0x20000000;
  }
  else {
    iVar6 = 0x40;
    if (((uVar7 & 1) != 0) && (iVar6 = 0xc0, (*pbVar2 & 0x40) == 0)) {
      iVar6 = 0x80;
    }
LAB_ram_0000108c:
    iVar6 = *(int *)(&USBFSD->UEP0_DMA + iVar5) + 0x20000000 + iVar6;
  }
  sub_44E6(iVar6,uVar4,extraout_a2);
LAB_ram_000010ce:
  *(short *)(&USBFSD->UEP0_CTRL + iVar5) = (short)extraout_a2;
  *pbVar2 = *pbVar2 & 0xfc;
  *(undefined1 *)(gp + -0x794 + iVar3) = 1;
  return 0;
}

/* ------------------------------------------------------------------
 * USBFS_IRQHandler   @ 0x1110
 * USB device interrupt handler
 * Signature: undefined vec_B8()
 *
 * WCH 16-bit load/store sites (quadrant-2 custom opcodes, invisible to the decompiler):
 *   0x11D0  lhu a5,0x0(s0)
 *   0x122C  lhu a5,0x32(s0)
 *   0x1232  sh a5,0x32(s0)
 *   0x1234  lhu a5,0x32(s0)
 *   0x1240  sh a5,0x32(s0)
 *   0x1250  lhu a5,0x0(a5)
 *   0x1260  sh a5,0x22(s0)
 *   0x1280  lhu a5,0x0(s1)
 *   0x128E  lhu s0,0x0(s1)
 *   0x12A8  lhu a5,0x0(s1)
 *   0x12B6  sh a5,0x0(s1)
 *   0x12E6  lhu a5,0x26(s0)
 *   0x12EC  sh a5,0x26(s0)
 *   0x12EE  lhu a5,0x26(s0)
 *   ... 57 more
 * ------------------------------------------------------------------ */
undefined2 * vec_B8(undefined2 *param_1)

{
  undefined1 uVar1;
  ushort uVar2;
  undefined1 uVar3;
  char cVar4;
  undefined2 *puVar5;
  undefined2 *puVar6;
  uint uVar7;
  byte bVar8;
  int iVar9;
  ushort uVar10;
  uint uVar11;
  uint extraout_a5;
  byte *pbVar12;
  ushort *puVar13;
  byte *pbVar14;
  int *piVar15;
  int iVar16;
  
  uVar7 = USBFSD->RX_LEN;
  if ((USBFSD->INT_FG & 2) == 0) {
    if ((USBFSD->INT_FG & 1) != 0) {
      *(undefined1 *)(gp + -0x799) = 0;
      *(undefined1 *)(gp + -0x79a) = 0;
      *(undefined1 *)(gp + -0x797) = 0;
      *(undefined1 *)(gp + -0x798) = 0;
      USBFSD->DEV_ADDR = 0;
      usb_ep_init();
      puVar5 = (undefined2 *)usb_ep_flags_clear(1);
      USBFSD->INT_FG = 1;
      return puVar5;
    }
    if ((USBFSD->INT_FG & 4) == 0) {
      return param_1;
    }
    USBFSD->INT_FG = 4;
    puVar5 = (undefined2 *)delay_cycles(10);
    pbVar12 = (byte *)(gp + -0x797);
    if ((USBFSD->MIS_ST & 4) == 0) {
      *pbVar12 = *pbVar12 & 0xfd;
      return puVar5;
    }
    *pbVar12 = *pbVar12 | 2;
    return puVar5;
  }
  if ((USBFSD->INT_ST & 0x30) == 0x20) {
    bVar8 = USBFSD->INT_ST & 0x3f;
    if (bVar8 == 0x21) {
      *(undefined1 *)(gp + -0x793) = 0;
      USBFSD->INT_FG = 2;
      return param_1;
    }
    if (0x21 < bVar8) {
      if (bVar8 == 0x23) {
        *(undefined1 *)(gp + -0x791) = 0;
        *(undefined1 *)(gp + 0x798) = 0;
        USBFSD->INT_FG = 2;
        return param_1;
      }
      if (bVar8 == 0x24) {
        *(undefined1 *)(gp + -0x790) = 0;
        USBFSD->INT_FG = 2;
        return param_1;
      }
      USBFSD->INT_FG = 2;
      return param_1;
    }
    if (bVar8 != 0x20) {
      USBFSD->INT_FG = 2;
      return param_1;
    }
    if ((*(byte *)(gp + -0x784) & 0x60) != 0) {
      USBFSD->INT_FG = 2;
      return param_1;
    }
    if (*(char *)(gp + -0x78a) == '\x05') {
      USBFSD->DEV_ADDR = USBFSD->DEV_ADDR & 0x80 | *(byte *)(gp + -0x79a);
      USBFSD->INT_FG = 2;
      return param_1;
    }
    if (*(char *)(gp + -0x78a) == '\x06') {
      iVar9 = *(int *)(gp + -0x780);
      puVar5 = (undefined2 *)sub_44E6(gp + 0x7ec,iVar9,0x40);
      *(int *)(gp + -0x780) = iVar9 + 0x40;
      USBFSD->INT_FG = 2;
      USBFSD->UEP0_CTRL = 0x40;
      USBFSD->UEP0_CTRL_H = USBFSD->UEP0_CTRL_H ^ 0x40;
      return puVar5;
    }
    USBFSD->INT_FG = 2;
    return param_1;
  }
  if ((USBFSD->INT_ST & 0x30) != 0x30) {
    if ((USBFSD->INT_ST & 0x30) != 0) {
      USBFSD->INT_FG = 2;
      return param_1;
    }
    if ((USBFSD->INT_ST & 0x3f) == 2) {
      puVar13 = (ushort *)(gp + 0x764);
      uVar10 = *puVar13;
      uVar7 = USBFSD->RX_LEN >> 8;
      uVar2 = puVar13[uVar10 + 3];
      *(char *)(puVar13 + uVar10 + 3) = (char)USBFSD->RX_LEN;
      *(char *)((int)puVar13 + (uint)uVar10 * 2 + 7) = (char)uVar7;
      uVar10 = *puVar13;
      *(char *)puVar13 = (char)(uVar10 + 1);
      *(char *)(gp + 0x765) = (char)(uVar10 + 1 >> 8);
      USBFSD->UEP2_DMA = (uint)*puVar13 * 0x40 + gp + 0x364;
      if (0xf < *puVar13) {
        *(undefined1 *)(gp + 0x76c) = 0;
        *(undefined1 *)(gp + 0x76d) = 0;
        USBFSD->UEP2_DMA = gp + 0x364;
      }
      iVar9 = *(ushort *)(gp + 0x768) + 1;
      *(char *)(gp + 0x768) = (char)iVar9;
      *(char *)(gp + 0x769) = (char)((uint)iVar9 >> 8);
      if (*(ushort *)(gp + 0x768) < 0xe) {
        USBFSD->INT_FG = 2;
        return (undefined2 *)(uint)(byte)uVar2;
      }
      *(undefined1 *)(gp + 0x79d) = 1;
      USBFSD->INT_FG = 2;
      return (undefined2 *)(uint)(byte)uVar2;
    }
    if ((USBFSD->INT_ST & 0x3f) == 4) {
      uVar7 = USBFSD->RX_LEN & 0xffff;
      iVar9 = gp + 0x7ec;
      for (uVar11 = 0; uVar11 != uVar7; uVar11 = uVar11 + 1 & 0xffff) {
        *(byte *)(iVar9 + 0x80) = ~*(byte *)(iVar9 + 0x40);
        iVar9 = iVar9 + 1;
      }
      USBFSD->INT_FG = 2;
      USBFSD->UEP4_CTRL = (short)uVar11;
      USBFSD->UEP4_CTRL_H = USBFSD->UEP4_CTRL_H & 0xfffc;
      return param_1;
    }
    if ((USBFSD->INT_ST & 0x3f) != 0) {
      USBFSD->INT_FG = 2;
      return param_1;
    }
    if ((USBFSD->INT_ST & 0x40) == 0) {
      USBFSD->INT_FG = 2;
      return param_1;
    }
    uVar11 = *(byte *)(gp + -0x784) & 0x60;
    if ((*(byte *)(gp + -0x784) & 0x60) != 0) {
      *(undefined2 *)(gp + -0x786) = 0;
      if (*(char *)(gp + -0x78a) == ' ') {
        *(undefined1 *)(gp + 0x79e) = *(undefined1 *)(gp + 0x7ec);
        *(undefined1 *)(gp + 0x79f) = *(undefined1 *)(gp + 0x7ed);
        *(undefined1 *)(gp + 0x7a0) = *(undefined1 *)(gp + 0x7ee);
        *(undefined1 *)(gp + 0x7a1) = *(undefined1 *)(gp + 0x7ef);
        *(undefined1 *)(gp + 0x7a2) = *(undefined1 *)(gp + 0x7f0);
        *(undefined1 *)(gp + 0x7a3) = *(undefined1 *)(gp + 0x7f1);
        *(undefined1 *)(gp + 0x7a4) = *(undefined1 *)(gp + 0x7f2);
        *(undefined1 *)(gp + 0x7a5) = 0x1e;
        *(undefined1 *)(gp + 0x7a5) = *(undefined1 *)(gp + 0x76b);
        param_1 = (undefined2 *)usb_uart_reinit();
        uVar11 = extraout_a5;
      }
      else {
        uVar11 = (uint)*(byte *)(gp + -0x78a);
        if (uVar11 == 9) {
          puVar13 = (ushort *)(gp + -0x79c);
          param_1 = (undefined2 *)
                    sub_44E6((uint)*puVar13 + gp + 0x7ac,gp + 0x7ec,uVar7 & 0xffff);
          *puVar13 = (short)uVar7 + *puVar13;
          uVar11 = 0x40023000;
          USBFSD->UEP0_CTRL_H = USBFSD->UEP0_CTRL_H & 0xc;
        }
      }
    }
    if ((uVar11 & 0xffff) == 0) {
      USBFSD->INT_FG = 2;
      USBFSD->UEP0_CTRL = 0;
      USBFSD->UEP0_CTRL_H = 0x40;
      return param_1;
    }
    USBFSD->INT_FG = 2;
    return param_1;
  }
  *(byte *)(gp + -0x784) = *(byte *)(gp + 0x7ec);
  pbVar12 = (byte *)(gp + -0x78a);
  *pbVar12 = *(byte *)(gp + 0x7ed);
  *(undefined2 *)(gp + -0x782) = 0x20;
  puVar5 = (undefined2 *)(gp + -0x788);
  *puVar5 = *(undefined2 *)(gp + 0x7f0);
  uVar3 = DAT_ram_00005c77;
  puVar13 = (ushort *)(gp + -0x786);
  pbVar14 = (byte *)(gp + -0x784);
  puVar6 = (undefined2 *)(gp + 0x7ec);
  iVar9 = gp + -0x788;
  if ((*(byte *)(gp + -0x784) & 0x60) != 0) {
    piVar15 = (int *)(gp + -0x780);
    if ((*pbVar14 & 0x20) == 0) {
      cVar4 = -((*pbVar14 >> 6 ^ 1) & 1);
    }
    else {
      bVar8 = *pbVar12;
      if (bVar8 == 10) {
        cVar4 = -1;
      }
      else if (bVar8 < 0xb) {
        if (bVar8 == 2) {
          cVar4 = -1;
        }
        else if (bVar8 < 3) {
          if (bVar8 == 1) {
            cVar4 = -1;
          }
          else {
LAB_ram_00001532:
            cVar4 = -1;
          }
        }
        else {
          if (bVar8 != 3) {
            if (bVar8 == 9) {
              *(undefined2 *)(gp + -0x79c) = 0;
              goto LAB_ram_000015aa;
            }
            goto LAB_ram_00001532;
          }
          cVar4 = -1;
        }
      }
      else if (bVar8 == 0x21) {
        *piVar15 = gp + 0x79e;
LAB_ram_000015aa:
        cVar4 = '\0';
      }
      else if (bVar8 < 0x22) {
        if (bVar8 == 0xb) {
          cVar4 = -1;
        }
        else {
          cVar4 = '\0';
          if (bVar8 != 0x20) goto LAB_ram_00001532;
        }
      }
      else {
        cVar4 = -1;
        if (bVar8 < 0x24) goto LAB_ram_000015aa;
      }
    }
    uVar7 = 0x40;
    if (*puVar13 < 0x40) {
      uVar7 = (uint)*puVar13;
    }
    iVar9 = *piVar15;
    puVar5 = (undefined2 *)sub_44E6(gp + 0x7ec,iVar9,uVar7);
    *piVar15 = uVar7 + iVar9;
    goto LAB_ram_00001594;
  }
  bVar8 = *pbVar12;
  if (0xb < bVar8) {
    USBFSD->INT_FG = 2;
    USBFSD->UEP0_CTRL_H = 0xcf;
    return puVar5;
  }
  uVar1 = SUB41((&switchD_ram:0000168e::switchdataD_ram_00005818)[bVar8],0);
  switch(bVar8) {
  case 0:
    *puVar6 = 0;
    if ((*pbVar14 & 0x1f) == 0) {
      cVar4 = '\0';
      if ((*(byte *)(gp + -0x797) & 1) != 0) {
        *(undefined1 *)puVar6 = 2;
      }
    }
    else {
      cVar4 = -1;
      if ((*pbVar14 & 0x1f) == 2) {
        cVar4 = '\0';
      }
    }
    if (2 < *puVar13) {
      *puVar13 = 2;
    }
    goto LAB_ram_00001594;
  case 1:
    if ((*pbVar14 & 0x1f) != 0) {
      USBFSD->INT_FG = 2;
      USBFSD->UEP0_CTRL_H = 0xcf;
      return puVar5;
    }
    if ((*pbVar14 & 0x1f) != 1) break;
    bVar8 = *(byte *)(gp + -0x797) & 0xfe;
    goto LAB_ram_00001828;
  default:
    goto switchD_ram_0000168e_caseD_2;
  case 3:
    if ((*pbVar14 & 0x1f) != 0) {
      USBFSD->INT_FG = 2;
      USBFSD->UEP0_CTRL_H = 0xcf;
      return puVar5;
    }
    if ((*pbVar14 & 0x1f) != 1) {
      USBFSD->INT_FG = 2;
      USBFSD->UEP0_CTRL_H = 0xcf;
      return puVar5;
    }
    if ((DAT_ram_00005c7b & 0x20) == 0) {
      USBFSD->INT_FG = 2;
      USBFSD->UEP0_CTRL_H = 0xcf;
      return puVar5;
    }
    bVar8 = *(byte *)(gp + -0x797) | 1;
LAB_ram_00001828:
    *(byte *)(gp + -0x797) = bVar8;
    break;
  case 5:
    *(undefined1 *)(gp + -0x79a) = uVar1;
    break;
  case 6:
    uVar7 = (uint)(&switchD_ram:0000168e::switchdataD_ram_00005818)[bVar8] >> 8;
    piVar15 = (int *)(gp + -0x780);
    if (uVar7 == 3) {
      iVar9 = 1;
      uVar7 = gp - 0x782U & 0xff;
      if (uVar7 == 1) {
        *piVar15 = (int)usb_str_manufacturer;
        uVar10 = (ushort)usb_str_manufacturer;
      }
      else if (uVar7 == 0) {
        *piVar15 = gp + -0x7d8;
        uVar10 = (ushort)*(byte *)(gp + -0x7d8);
      }
      else {
        iVar9 = 2;
        if (uVar7 == 2) {
          *piVar15 = (int)usb_str_product;
          uVar10 = (ushort)usb_str_product;
        }
        else {
          if (uVar7 != 3) goto LAB_ram_000016ae;
          *piVar15 = (int)usb_str_serial;
          uVar10 = (ushort)usb_str_serial;
        }
      }
LAB_ram_000016e0:
      cVar4 = '\0';
    }
    else {
      if (uVar7 < 4) {
        if (uVar7 == 1) {
          uVar10 = (ushort)usb_device_desc;
          *piVar15 = (int)usb_device_desc;
        }
        else {
          if (uVar7 != 2) goto LAB_ram_000016ae;
          *piVar15 = (int)usb_config_desc;
          uVar10 = CONCAT11(uVar3,str_5C76);
        }
        goto LAB_ram_000016e0;
      }
      if ((uVar7 != 0x21) && (uVar7 == 0x22)) {
        *piVar15 = (int)usb_report_desc;
        uVar10 = 0x22;
        goto LAB_ram_000016e0;
      }
LAB_ram_000016ae:
      uVar10 = 0;
      cVar4 = -1;
    }
    if (uVar10 < *puVar13) {
      *puVar13 = uVar10;
    }
    uVar7 = 0x40;
    if (*puVar13 < 0x40) {
      uVar7 = (uint)*puVar13;
    }
    iVar16 = *piVar15;
    puVar5 = (undefined2 *)sub_44E6(gp + 0x7ec,iVar16,uVar7,iVar9);
    *piVar15 = uVar7 + iVar16;
LAB_ram_00001594:
    if (cVar4 == -1) {
switchD_ram_0000168e_caseD_2:
      USBFSD->INT_FG = 2;
      USBFSD->UEP0_CTRL_H = 0xcf;
      return puVar5;
    }
    break;
  case 8:
    *(undefined1 *)puVar6 = *(undefined1 *)(gp + -0x799);
    goto LAB_ram_000017e8;
  case 9:
    *(undefined1 *)(gp + -0x799) = uVar1;
    *(undefined1 *)(gp + -0x798) = 1;
    break;
  case 10:
    *(undefined1 *)puVar6 = 0;
LAB_ram_000017e8:
    if (1 < *puVar13) {
      *puVar13 = 1;
    }
    break;
  case 0xb:
    break;
  }
  if ((char)*pbVar14 < '\0') {
    USBFSD->UEP0_CTRL = 0x40;
    if (*puVar13 < 0x41) {
      USBFSD->UEP0_CTRL = *puVar13;
    }
    *puVar13 = *puVar13 - USBFSD->UEP0_CTRL;
    USBFSD->UEP0_CTRL_H = 0x40;
  }
  else if (*puVar13 == 0) {
    USBFSD->UEP0_CTRL = 0;
  }
  USBFSD->INT_FG = 2;
  return puVar5;
}

/* ------------------------------------------------------------------
 * sub_44E6   @ 0x44E6
 * Signature: undefined FUN_ram_000044e6()
 * ------------------------------------------------------------------ */
void sub_44E6(undefined4 *param_1,undefined4 *param_2,uint param_3)

{
  undefined1 uVar1;
  undefined4 uVar2;
  undefined4 uVar3;
  undefined4 *puVar4;
  undefined4 *puVar5;
  undefined4 uVar6;
  undefined4 uVar7;
  undefined4 uVar8;
  undefined4 uVar9;
  undefined4 uVar10;
  undefined4 uVar11;
  
  puVar4 = (undefined4 *)((int)param_1 + param_3);
  if (((((uint)param_2 ^ (uint)param_1) & 3) == 0) && (3 < param_3)) {
    for (; ((uint)param_1 & 3) != 0; param_1 = (undefined4 *)((int)param_1 + 1)) {
      uVar1 = *(undefined1 *)param_2;
      param_2 = (undefined4 *)((int)param_2 + 1);
      *(undefined1 *)param_1 = uVar1;
    }
    for (; param_1 < (undefined4 *)((uint)puVar4 & 0xfffffffc) + -8; param_1 = param_1 + 9) {
      uVar2 = param_2[1];
      uVar11 = param_2[2];
      uVar10 = param_2[3];
      uVar9 = param_2[4];
      uVar8 = param_2[5];
      uVar3 = param_2[6];
      uVar7 = param_2[7];
      *param_1 = *param_2;
      uVar6 = param_2[8];
      param_1[1] = uVar2;
      param_1[2] = uVar11;
      param_1[3] = uVar10;
      param_1[4] = uVar9;
      param_1[5] = uVar8;
      param_1[6] = uVar3;
      param_1[7] = uVar7;
      param_1[8] = uVar6;
      param_2 = param_2 + 9;
    }
    for (; param_1 < (undefined4 *)((uint)puVar4 & 0xfffffffc); param_1 = param_1 + 1) {
      uVar2 = *param_2;
      param_2 = param_2 + 1;
      *param_1 = uVar2;
    }
  }
  if (puVar4 <= param_1) {
    return;
  }
  do {
    uVar1 = *(undefined1 *)param_2;
    puVar5 = (undefined4 *)((int)param_1 + 1);
    param_2 = (undefined4 *)((int)param_2 + 1);
    *(undefined1 *)param_1 = uVar1;
    param_1 = puVar5;
  } while (puVar5 < puVar4);
  return;
}
