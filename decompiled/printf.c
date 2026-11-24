/* MSU2_MINI firmware 1.1 -- reconstructed C, module "printf".
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
 * printf   @ 0x4598
 * Signature: undefined FUN_ram_00004598()
 * ------------------------------------------------------------------ */
void printf(undefined4 param_1)

{
  int iVar1;
  
  iVar1 = *(int *)(gp + -0x7ec);
  if ((iVar1 != 0) && (*(int *)(iVar1 + 0x18) == 0)) {
    sub_468C(iVar1);
  }
  vfprintf(iVar1,*(undefined4 *)(iVar1 + 8),param_1);
  return;
}

/* ------------------------------------------------------------------
 * sub_45D6   @ 0x45D6
 * Signature: undefined FUN_ram_000045d6()
 *
 * WCH 16-bit load/store sites (quadrant-2 custom opcodes, invisible to the decompiler):
 *   0x45DE  sh a1,0xc(a0)
 *   0x45E0  sh a2,0xe(a0)
 * ------------------------------------------------------------------ */
void sub_45D6(undefined4 *param_1)

{
  *param_1 = 0;
  param_1[1] = 0;
  param_1[2] = 0;
  param_1[0x19] = 0;
  param_1[4] = 0;
  param_1[5] = 0;
  param_1[6] = 0;
  memset(param_1 + 0x17,0,8);
  param_1[9] = sub_4FAA;
  param_1[10] = &DAT_ram_00004fd6;
  param_1[0xb] = sub_501E;
  param_1[8] = param_1;
  param_1[0xc] = &DAT_ram_0000504e;
  return;
}

/* ------------------------------------------------------------------
 * sub_463C   @ 0x463C
 * Signature: undefined FUN_ram_0000463c()
 * ------------------------------------------------------------------ */
void sub_463C(undefined4 param_1)

{
  sub_4798(param_1,sub_53A6);
  return;
}

/* ------------------------------------------------------------------
 * sub_4646   @ 0x4646
 * Signature: undefined FUN_ram_00004646()
 * ------------------------------------------------------------------ */
undefined4 * sub_4646(undefined4 param_1,int param_2)

{
  int iVar1;
  undefined4 *puVar2;
  
  iVar1 = (param_2 + -1) * 0x68;
  puVar2 = (undefined4 *)sub_4802(param_1,iVar1 + 0x74);
  if (puVar2 != (undefined4 *)0x0) {
    *puVar2 = 0;
    puVar2[1] = param_2;
    puVar2[2] = puVar2 + 3;
    memset(puVar2 + 3,0,iVar1 + 0x68);
  }
  return puVar2;
}

/* ------------------------------------------------------------------
 * sub_468C   @ 0x468C
 * Signature: undefined FUN_ram_0000468c()
 * ------------------------------------------------------------------ */
void sub_468C(int param_1)

{
  undefined4 uVar1;
  int iVar2;
  
  if (*(int *)(param_1 + 0x18) != 0) {
    return;
  }
  *(code **)(param_1 + 0x28) = sub_463C;
  iVar2 = *(int *)(gp + -0x7d4);
  *(undefined4 *)(param_1 + 0x48) = 0;
  *(undefined4 *)(param_1 + 0x4c) = 0;
  *(undefined4 *)(param_1 + 0x50) = 0;
  if (param_1 == iVar2) {
    *(undefined4 *)(param_1 + 0x18) = 1;
  }
  uVar1 = sub_46F8();
  *(undefined4 *)(param_1 + 4) = uVar1;
  uVar1 = sub_46F8(param_1);
  *(undefined4 *)(param_1 + 8) = uVar1;
  uVar1 = sub_46F8(param_1);
  *(undefined4 *)(param_1 + 0xc) = uVar1;
  sub_45D6(*(undefined4 *)(param_1 + 4),4,0);
  sub_45D6(*(undefined4 *)(param_1 + 8),9,1);
  sub_45D6(*(undefined4 *)(param_1 + 0xc),0x12,2);
  *(undefined4 *)(param_1 + 0x18) = 1;
  return;
}

/* ------------------------------------------------------------------
 * sub_46F8   @ 0x46F8
 * Signature: undefined FUN_ram_000046f8()
 * ------------------------------------------------------------------ */
undefined4 * sub_46F8(undefined4 *param_1)

{
  undefined4 *puVar1;
  int iVar2;
  int *piVar3;
  
  iVar2 = *(int *)(gp + -0x7d4);
  if (*(int *)(iVar2 + 0x18) == 0) {
    sub_468C(iVar2);
  }
  piVar3 = (int *)(iVar2 + 0x48);
  do {
    puVar1 = (undefined4 *)piVar3[2];
    iVar2 = piVar3[1];
    while (iVar2 = iVar2 + -1, -1 < iVar2) {
      if (*(short *)(puVar1 + 3) == 0) {
        puVar1[0x19] = 0;
        *puVar1 = 0;
        puVar1[1] = 0;
        puVar1[2] = 0;
        puVar1[3] = 0xffff0001;
        puVar1[4] = 0;
        puVar1[5] = 0;
        puVar1[6] = 0;
        memset(puVar1 + 0x17,0,8);
        puVar1[0xd] = 0;
        puVar1[0xe] = 0;
        puVar1[0x12] = 0;
        puVar1[0x13] = 0;
        return puVar1;
      }
      puVar1 = puVar1 + 0x1a;
    }
    if (*piVar3 == 0) {
      iVar2 = sub_4646(param_1,4);
      *piVar3 = iVar2;
      if (iVar2 == 0) {
        *param_1 = 0xc;
        return (undefined4 *)0x0;
      }
    }
    piVar3 = (int *)*piVar3;
  } while( true );
}

/* ------------------------------------------------------------------
 * sub_4798   @ 0x4798
 * Signature: undefined FUN_ram_00004798()
 *
 * WCH 16-bit load/store sites (quadrant-2 custom opcodes, invisible to the decompiler):
 *   0x47E4  lhu a5,0xc(s1)
 * ------------------------------------------------------------------ */
uint sub_4798(int param_1,code *param_2)

{
  int *piVar1;
  int iVar2;
  uint uVar3;
  uint in_a5;
  uint extraout_a5;
  uint uVar4;
  int iVar5;
  
  uVar4 = 0;
  for (piVar1 = (int *)(param_1 + 0x48); piVar1 != (int *)0x0; piVar1 = (int *)*piVar1) {
    iVar2 = piVar1[2];
    iVar5 = piVar1[1];
    while (iVar5 = iVar5 + -1, -1 < iVar5) {
      if ((1 < in_a5) && (in_a5 = (uint)*(short *)(iVar2 + 0xe), in_a5 != 0xffffffff)) {
        uVar3 = (*param_2)(param_1,iVar2);
        uVar4 = uVar4 | uVar3;
        in_a5 = extraout_a5;
      }
      iVar2 = iVar2 + 0x68;
    }
  }
  return uVar4;
}

/* ------------------------------------------------------------------
 * sub_4802   @ 0x4802
 * Signature: undefined FUN_ram_00004802()
 * ------------------------------------------------------------------ */
uint sub_4802(undefined4 *param_1,uint param_2)

{
  uint *puVar1;
  uint uVar2;
  undefined4 uVar3;
  uint *puVar4;
  int iVar5;
  uint *puVar6;
  uint uVar7;
  
  uVar2 = (param_2 + 3 & 0xfffffffc) + 8;
  if (uVar2 < 0xc) {
    uVar2 = 0xc;
  }
  else if ((int)uVar2 < 0) goto LAB_ram_00004862;
  if (param_2 <= uVar2) {
    sub_5542();
    puVar4 = *(uint **)(gp + -0x774);
    puVar6 = *(uint **)(gp + -0x774);
    do {
      puVar1 = puVar4;
      if (puVar1 == (uint *)0x0) {
        if (*(int *)(gp + -0x770) == 0) {
          uVar3 = sub_4F78(param_1,0);
          *(undefined4 *)(gp + -0x770) = uVar3;
        }
        puVar4 = (uint *)sub_4F78(param_1,uVar2);
        if ((puVar4 == (uint *)0xffffffff) ||
           ((puVar1 = (uint *)((int)puVar4 + 3U & 0xfffffffc), puVar4 != puVar1 &&
            (iVar5 = sub_4F78(param_1,(int)puVar1 - (int)puVar4), iVar5 == -1)))) {
          *param_1 = 0xc;
          sub_5544(param_1);
          return 0;
        }
LAB_ram_00004888:
        *puVar1 = uVar2;
LAB_ram_00004896:
        sub_5544(param_1);
        uVar2 = (int)puVar1 + 0xbU & 0xfffffff8;
        iVar5 = uVar2 - (int)(puVar1 + 1);
        if (iVar5 == 0) {
          return uVar2;
        }
        *(uint *)((int)puVar1 + iVar5) = (int)(puVar1 + 1) - uVar2;
        return uVar2;
      }
      uVar7 = *puVar1 - uVar2;
      if (-1 < (int)uVar7) {
        if (0xb < uVar7) {
          *puVar1 = uVar7;
          puVar1 = (uint *)((int)puVar1 + uVar7);
          goto LAB_ram_00004888;
        }
        if (puVar6 == puVar1) {
          *(uint *)(gp + -0x774) = puVar1[1];
        }
        else {
          puVar6[1] = puVar1[1];
        }
        goto LAB_ram_00004896;
      }
      puVar4 = (uint *)puVar1[1];
      puVar6 = puVar1;
    } while( true );
  }
LAB_ram_00004862:
  *param_1 = 0xc;
  return 0;
}

/* ------------------------------------------------------------------
 * sub_48D6   @ 0x48D6
 * Signature: undefined FUN_ram_000048d6()
 * ------------------------------------------------------------------ */
int sub_48D6(undefined4 param_1,int param_2,undefined4 *param_3)

{
  int iVar1;
  undefined1 *puVar2;
  
  iVar1 = param_3[2] + -1;
  param_3[2] = iVar1;
  if ((iVar1 < 0) && ((iVar1 < (int)param_3[6] || (param_2 == 10)))) {
    iVar1 = sub_5054();
    return iVar1;
  }
  puVar2 = (undefined1 *)*param_3;
  *param_3 = puVar2 + 1;
  *puVar2 = (char)param_2;
  return param_2;
}

/* ------------------------------------------------------------------
 * sub_48FC   @ 0x48FC
 * Signature: undefined FUN_ram_000048fc()
 * ------------------------------------------------------------------ */
undefined4 sub_48FC(undefined4 param_1,undefined4 param_2,undefined1 *param_3,int param_4)

{
  undefined1 *puVar1;
  int iVar2;
  
  puVar1 = param_3 + param_4;
  do {
    if (param_3 == puVar1) {
      return 0;
    }
    iVar2 = sub_48D6(param_1,*param_3,param_2);
    param_3 = param_3 + 1;
  } while (iVar2 != -1);
  return 0xffffffff;
}

/* ------------------------------------------------------------------
 * vfprintf   @ 0x493E
 * Signature: undefined FUN_ram_0000493e()
 *
 * WCH 16-bit load/store sites (quadrant-2 custom opcodes, invisible to the decompiler):
 *   0x4978  lhu a5,0xc(s1)
 *   0x4BA8  lhu a5,0xc(s1)
 * ------------------------------------------------------------------ */
int vfprintf(int param_1,undefined *param_2,byte *param_3,int *param_4)

{
  byte *pbVar1;
  int iVar2;
  bool bVar3;
  int *piVar4;
  int unaff_s4;
  int iVar5;
  int *piStack_94;
  uint uStack_90;
  int iStack_8c;
  undefined4 uStack_88;
  int iStack_84;
  int iStack_7c;
  byte bStack_78;
  undefined1 uStack_77;
  undefined1 uStack_76;
  undefined1 uStack_4d;
  undefined4 uStack_38;
  
  if ((param_1 != 0) && (*(int *)(param_1 + 0x18) == 0)) {
    sub_468C();
  }
  if (param_2 == &DAT_ram_00005d74) {
    param_2 = *(undefined **)(param_1 + 4);
  }
  else if (param_2 == &DAT_ram_00005d94) {
    param_2 = *(undefined **)(param_1 + 8);
  }
  else if (param_2 == &DAT_ram_00005d54) {
    param_2 = *(undefined **)(param_1 + 0xc);
  }
  iVar5 = sub_5148(param_1,param_2);
  if (iVar5 != 0) {
    return -1;
  }
  uStack_77 = 0x20;
  iStack_7c = 0;
  uStack_76 = 0x30;
  piStack_94 = param_4;
  do {
    for (pbVar1 = param_3; (*pbVar1 != 0 && (*pbVar1 != 0x25)); pbVar1 = pbVar1 + 1) {
    }
    iVar5 = (int)pbVar1 - (int)param_3;
    if (iVar5 != 0) {
      iVar2 = sub_48FC(param_1,param_2,param_3,iVar5);
      bVar3 = true;
      if (iVar2 == -1) {
code_r0x00004baa:
        if (!bVar3) {
          return iStack_7c;
        }
        return -1;
      }
      iStack_7c = iStack_7c + iVar5;
    }
    bVar3 = false;
    if (*pbVar1 == 0) goto code_r0x00004baa;
    uStack_90 = 0;
    iStack_84 = 0;
    iStack_8c = -1;
    uStack_88 = 0;
    uStack_4d = 0;
    uStack_38 = 0;
    pbVar1 = pbVar1 + 1;
    while( true ) {
      iVar5 = sub_552A(str_5DB4,*pbVar1,5);
      param_3 = pbVar1 + 1;
      if (iVar5 == 0) break;
      uStack_90 = uStack_90 | 1 << (iVar5 - 0x5db4U & 0x1f);
      pbVar1 = param_3;
    }
    if ((uStack_90 & 0x10) != 0) {
      uStack_4d = 0x20;
    }
    if ((uStack_90 & 8) != 0) {
      uStack_4d = 0x2b;
    }
    if (*pbVar1 == 0x2a) {
      iStack_84 = *piStack_94;
      if (iStack_84 < 0) {
        iStack_84 = -iStack_84;
        uStack_90 = uStack_90 | 2;
        piStack_94 = piStack_94 + 1;
      }
      else {
        piStack_94 = piStack_94 + 1;
      }
    }
    else {
      bVar3 = false;
      param_3 = pbVar1;
      iVar5 = iStack_84;
      while( true ) {
        if (9 < *param_3 - 0x30) break;
        bVar3 = true;
        iVar5 = iVar5 * 10 + (*param_3 - 0x30);
        param_3 = param_3 + 1;
      }
      if (bVar3) {
        iStack_84 = iVar5;
      }
    }
    if (*param_3 == 0x2e) {
      if (param_3[1] == 0x2a) {
        param_3 = param_3 + 2;
        piVar4 = piStack_94 + 1;
        iVar5 = *piStack_94;
        piStack_94 = piVar4;
        if (iVar5 < 0) {
          iVar5 = -1;
        }
      }
      else {
        iStack_8c = 0;
        bVar3 = false;
        iVar5 = 0;
        while( true ) {
          param_3 = param_3 + 1;
          if (9 < *param_3 - 0x30) break;
          bVar3 = true;
          iVar5 = iVar5 * 10 + (*param_3 - 0x30);
        }
        if (!bVar3) goto LAB_ram_00004b0e;
      }
      iStack_8c = iVar5;
    }
LAB_ram_00004b0e:
    iVar5 = sub_552A(&DAT_ram_00005dbc,*param_3,3);
    if (iVar5 != 0) {
      param_3 = param_3 + 1;
      uStack_90 = uStack_90 | 0x40 << (iVar5 - 0x5dbcU & 0x1f);
    }
    bStack_78 = *param_3;
    param_3 = param_3 + 1;
    iVar5 = sub_552A(str_5DC0,bStack_78,6);
    if (iVar5 == 0) {
      unaff_s4 = printf_emit(param_1,&uStack_90,param_2,sub_48FC,&piStack_94);
      bVar3 = true;
      if (unaff_s4 == -1) goto code_r0x00004baa;
    }
    else if ((uStack_90 & 0x100) == 0) {
      piStack_94 = (int *)(((int)piStack_94 + 7U & 0xfffffff8) + 8);
    }
    else {
      piStack_94 = piStack_94 + 1;
    }
    iStack_7c = iStack_7c + unaff_s4;
  } while( true );
}

/* ------------------------------------------------------------------
 * printf_pad_field   @ 0x4BCA
 * Signature: undefined FUN_ram_00004bca()
 * ------------------------------------------------------------------ */
printf_pad_field(undefined4 param_1,uint *param_2,uint *param_3,undefined4 param_4,code *param_5)

{
  int iVar1;
  undefined4 uVar2;
  int iVar3;
  uint uVar4;
  uint uVar5;
  int iVar6;
  
  uVar5 = param_2[4];
  if ((int)param_2[4] < (int)param_2[2]) {
    uVar5 = param_2[2];
  }
  *param_3 = uVar5;
  if (*(char *)((int)param_2 + 0x43) != '\0') {
    *param_3 = uVar5 + 1;
  }
  if ((*param_2 & 0x20) != 0) {
    *param_3 = *param_3 + 2;
  }
  if ((*param_2 & 6) == 0) {
    iVar1 = 0;
    while( true ) {
      if ((int)(param_2[3] - *param_3) <= iVar1) break;
      iVar6 = (*param_5)(param_1,param_4,(int)param_2 + 0x19,1);
      if (iVar6 == -1) goto LAB_ram_00004c8a;
      iVar1 = iVar1 + 1;
    }
  }
  uVar4 = (uint)(*(char *)((int)param_2 + 0x43) != '\0');
  uVar5 = uVar4;
  if ((*param_2 & 0x20) != 0) {
    *(undefined1 *)((int)param_2 + uVar4 + 0x43) = 0x30;
    uVar5 = uVar4 + 2;
    *(undefined1 *)((int)param_2 + uVar4 + 0x44) = *(undefined1 *)((int)param_2 + 0x45);
  }
  iVar1 = (*param_5)(param_1,param_4,(int)param_2 + 0x43,uVar5);
  if (iVar1 == -1) {
LAB_ram_00004c8a:
    uVar2 = 0xffffffff;
  }
  else {
    iVar1 = 0;
    if (((*param_2 & 6) == 4) && (iVar1 = param_2[3] - *param_3, iVar1 < 0)) {
      iVar1 = 0;
    }
    if ((int)param_2[4] < (int)param_2[2]) {
      iVar1 = iVar1 + (param_2[2] - param_2[4]);
    }
    for (iVar6 = 0; iVar1 != iVar6; iVar6 = iVar6 + 1) {
      iVar3 = (*param_5)(param_1,param_4,(int)param_2 + 0x1a,1);
      if (iVar3 == -1) goto LAB_ram_00004c8a;
    }
    uVar2 = 0;
  }
  return uVar2;
}

/* ------------------------------------------------------------------
 * printf_emit   @ 0x4CD6
 * Signature: undefined FUN_ram_00004cd6()
 *
 * WCH 16-bit load/store sites (quadrant-2 custom opcodes, invisible to the decompiler):
 *   0x4E00  lhu a5,0x0(a5)
 *   0x4F28  sh a1,0x0(a5)
 * ------------------------------------------------------------------ */
uint printf_emit(undefined4 param_1,uint *param_2,undefined4 param_3,code *param_4,int *param_5
                     )

{
  byte bVar1;
  uint *puVar2;
  int iVar3;
  int iVar4;
  uint uVar5;
  byte *pbVar6;
  char *pcVar7;
  uint *puVar8;
  undefined4 uVar9;
  uint uVar10;
  undefined4 *puVar11;
  byte *pbVar12;
  uint uStack_24;
  
  bVar1 = (byte)*param_2;
  pbVar6 = (byte *)((int)param_2 + 0x43);
  pbVar12 = pbVar6;
  if (bVar1 == 0x69) {
LAB_ram_00004d34:
    uVar10 = *param_2;
    puVar2 = (uint *)*param_5;
    if ((uVar10 & 0x80) == 0) {
      puVar8 = (uint *)*puVar2;
      *param_5 = (int)(puVar2 + 1);
      if ((uVar10 & 0x40) != 0) {
        puVar8 = (uint *)(int)(short)puVar8;
      }
    }
    else {
      puVar8 = (uint *)*puVar2;
      *param_5 = (int)(puVar2 + 1);
    }
    if ((int)puVar8 < 0) {
      puVar8 = (uint *)-(int)puVar8;
      *(byte *)((int)param_2 + 0x43) = 0x2d;
    }
    pcVar7 = str_5DC8;
    uVar10 = 10;
LAB_ram_00004e18:
    uVar5 = param_2[1];
    param_2[2] = uVar5;
    if (-1 < (int)uVar5) {
      *param_2 = *param_2 & 0xfffffffb;
    }
    if ((puVar8 != (uint *)0x0) || (uVar5 != 0)) {
      while( true ) {
        if (uVar10 == 0) {
          uVar5 = (uint)puVar8;
        }
        else {
          uVar5 = (uint)puVar8 % uVar10;
        }
        pbVar12 = pbVar12 + -1;
        *pbVar12 = pcVar7[uVar5];
        if (uVar10 == 0) {
          uVar5 = 0xffffffff;
        }
        else {
          uVar5 = (uint)puVar8 / uVar10;
        }
        if (puVar8 < uVar10) break;
        puVar8 = (uint *)uVar5;
      }
    }
    if (((uVar10 == 8) && ((*param_2 & 1) != 0)) && ((int)param_2[1] <= (int)param_2[4])) {
      pbVar12[-1] = 0x30;
      pbVar12 = pbVar12 + -1;
    }
    param_2[4] = (int)pbVar6 - (int)pbVar12;
    goto LAB_ram_00004e68;
  }
  if (bVar1 < 0x6a) {
    if (bVar1 == 0x58) {
      *(byte *)((int)param_2 + 0x45) = 0x58;
      pcVar7 = str_5DC8;
LAB_ram_00004e9e:
      uVar10 = *param_2;
      puVar8 = *(uint **)*param_5;
      puVar2 = (uint *)*param_5 + 1;
      if ((uVar10 & 0x80) == 0) {
        *param_5 = (int)puVar2;
        if ((uVar10 & 0x40) != 0) {
          puVar8 = (uint *)((uint)puVar8 & 0xffff);
        }
      }
      else {
        *param_5 = (int)puVar2;
      }
      if ((uVar10 & 1) != 0) {
        *param_2 = uVar10 | 0x20;
      }
      uVar10 = 0x10;
      if (puVar8 == (uint *)0x0) {
        *param_2 = *param_2 & 0xffffffdf;
      }
LAB_ram_00004e14:
      *(byte *)((int)param_2 + 0x43) = 0;
      goto LAB_ram_00004e18;
    }
    if (bVar1 < 0x59) {
      if (bVar1 == 0) {
LAB_ram_00004f2a:
        param_2[4] = 0;
        goto LAB_ram_00004e68;
      }
      if (bVar1 == 0x43) goto LAB_ram_00004dd2;
LAB_ram_00004d1a:
      *(byte *)((int)param_2 + 0x42) = bVar1;
    }
    else {
      if (bVar1 != 99) {
        if (bVar1 == 100) goto LAB_ram_00004d34;
        goto LAB_ram_00004d1a;
      }
LAB_ram_00004dd2:
      uVar9 = *(undefined4 *)*param_5;
      *param_5 = (int)((undefined4 *)*param_5 + 1);
      *(byte *)((int)param_2 + 0x42) = (byte)uVar9;
    }
    pbVar12 = (byte *)((int)param_2 + 0x42);
    uVar10 = 1;
  }
  else {
    if (bVar1 == 0x70) {
      *param_2 = *param_2 | 0x20;
LAB_ram_00004ed2:
      *(byte *)((int)param_2 + 0x45) = 0x78;
      pcVar7 = str_5DDC;
      goto LAB_ram_00004e9e;
    }
    if (bVar1 < 0x71) {
      if (bVar1 == 0x6e) {
        uVar5 = *param_2;
        puVar11 = (undefined4 *)*param_5;
        uVar10 = param_2[5];
        if ((uVar5 & 0x80) == 0) {
          *param_5 = (int)(puVar11 + 1);
          puVar8 = (uint *)*puVar11;
          if ((uVar5 & 0x40) != 0) goto LAB_ram_00004f2a;
        }
        else {
          *param_5 = (int)(puVar11 + 1);
          puVar8 = (uint *)*puVar11;
        }
        *puVar8 = uVar10;
        goto LAB_ram_00004f2a;
      }
      if (bVar1 != 0x6f) goto LAB_ram_00004d1a;
LAB_ram_00004d7e:
      uVar10 = *param_2;
      puVar8 = (uint *)*param_5;
      if ((uVar10 & 0x80) == 0) {
        *param_5 = (int)(puVar8 + 1);
        if ((uVar10 & 0x40) == 0) goto LAB_ram_00004d90;
      }
      else {
        *param_5 = (int)(puVar8 + 1);
LAB_ram_00004d90:
        puVar8 = (uint *)*puVar8;
      }
      if (bVar1 == 0x6f) {
        pcVar7 = str_5DC8;
        uVar10 = 8;
      }
      else {
        pcVar7 = str_5DC8;
        uVar10 = 10;
      }
      goto LAB_ram_00004e14;
    }
    if (bVar1 == 0x75) goto LAB_ram_00004d7e;
    if (bVar1 == 0x78) goto LAB_ram_00004ed2;
    if (bVar1 != 0x73) goto LAB_ram_00004d1a;
    puVar11 = (undefined4 *)*param_5;
    uVar10 = param_2[1];
    *param_5 = (int)(puVar11 + 1);
    pbVar12 = (byte *)*puVar11;
    iVar3 = sub_552A(pbVar12,0,uVar10);
    if (iVar3 != 0) {
      param_2[1] = iVar3 - (int)pbVar12;
    }
    uVar10 = param_2[1];
  }
  param_2[4] = uVar10;
  *(byte *)((int)param_2 + 0x43) = 0;
LAB_ram_00004e68:
  iVar3 = printf_pad_field(param_1,param_2,&uStack_24,param_3,param_4);
  if ((iVar3 == -1) || (iVar3 = (*param_4)(param_1,param_3,pbVar12,param_2[4]), iVar3 == -1)) {
LAB_ram_00004e7c:
    uVar10 = 0xffffffff;
  }
  else {
    if ((*param_2 & 2) != 0) {
      for (iVar3 = 0; iVar3 < (int)(param_2[3] - uStack_24); iVar3 = iVar3 + 1) {
        iVar4 = (*param_4)(param_1,param_3,(byte *)((int)param_2 + 0x19),1);
        if (iVar4 == -1) goto LAB_ram_00004e7c;
      }
    }
    uVar10 = param_2[3];
    if ((int)param_2[3] < (int)uStack_24) {
      uVar10 = uStack_24;
    }
  }
  return uVar10;
}

/* ------------------------------------------------------------------
 * sub_4F78   @ 0x4F78
 * Signature: undefined FUN_ram_00004f78()
 * ------------------------------------------------------------------ */
void sub_4F78(int *param_1,undefined4 param_2)

{
  int iVar1;
  
  _DAT_ram_200011ac = 0;
  iVar1 = DMA_M2M_Copy(param_2);
  if ((iVar1 == -1) && (_DAT_ram_200011ac != 0)) {
    *param_1 = _DAT_ram_200011ac;
  }
  return;
}

/* ------------------------------------------------------------------
 * sub_4FAA   @ 0x4FAA
 * Signature: undefined FUN_ram_00004faa()
 *
 * WCH 16-bit load/store sites (quadrant-2 custom opcodes, invisible to the decompiler):
 *   0x4FCA  lhu a5,0xc(s0)
 *   0x4FD2  sh a5,0xc(s0)
 * ------------------------------------------------------------------ */
void sub_4FAA(undefined4 param_1,int param_2)

{
  int iVar1;
  
  iVar1 = strlen(param_1,(int)*(short *)(param_2 + 0xe));
  if (-1 < iVar1) {
    *(int *)(param_2 + 0x54) = *(int *)(param_2 + 0x54) + iVar1;
  }
  return;
}

/* ------------------------------------------------------------------
 * sub_501E   @ 0x501E
 * Signature: undefined FUN_ram_0000501e()
 *
 * WCH 16-bit load/store sites (quadrant-2 custom opcodes, invisible to the decompiler):
 *   0x502E  lhu a4,0xc(s0)
 *   0x503A  sh a5,0xc(s0)
 *   0x5048  sh a5,0xc(s0)
 * ------------------------------------------------------------------ */
void sub_501E(undefined4 param_1,int param_2)

{
  int iVar1;
  
  iVar1 = sub_540E(param_1,(int)*(short *)(param_2 + 0xe));
  if (iVar1 != -1) {
    *(int *)(param_2 + 0x54) = iVar1;
  }
  return;
}

/* ------------------------------------------------------------------
 * sub_5054   @ 0x5054
 * Signature: undefined FUN_ram_00005054()
 *
 * WCH 16-bit load/store sites (quadrant-2 custom opcodes, invisible to the decompiler):
 *   0x5082  lhu a5,0xc(s0)
 *   0x50C2  lhu a5,0xc(s0)
 * ------------------------------------------------------------------ */
uint sub_5054(int param_1,byte param_2,int *param_3)

{
  int iVar1;
  int iVar2;
  byte *pbVar3;
  uint uVar4;
  
  if ((param_1 != 0) && (*(int *)(param_1 + 0x18) == 0)) {
    sub_468C();
  }
  if (param_3 == (int *)&DAT_ram_00005d74) {
    param_3 = *(int **)(param_1 + 4);
  }
  else if (param_3 == (int *)&DAT_ram_00005d94) {
    param_3 = *(int **)(param_1 + 8);
  }
  else if (param_3 == (int *)&DAT_ram_00005d54) {
    param_3 = *(int **)(param_1 + 0xc);
  }
  param_3[2] = param_3[6];
  if ((((param_3[6] & 8U) != 0) && (param_3[4] != 0)) ||
     (iVar1 = sub_5148(param_1,param_3), iVar1 == 0)) {
    uVar4 = (uint)param_2;
    iVar1 = *param_3 - param_3[4];
    if (param_3[5] <= iVar1) {
      iVar2 = sub_53A6(param_1,param_3);
      iVar1 = 0;
      if (iVar2 != 0) {
        return 0xffffffff;
      }
    }
    param_3[2] = param_3[2] + -1;
    pbVar3 = (byte *)*param_3;
    *param_3 = (int)(pbVar3 + 1);
    *pbVar3 = param_2;
    if (param_3[5] != iVar1 + 1U) {
      if ((param_3[5] & 1U) == 0) {
        return uVar4;
      }
      if (uVar4 != 10) {
        return uVar4;
      }
    }
    iVar1 = sub_53A6(param_1,param_3);
    if (iVar1 == 0) {
      return uVar4;
    }
  }
  return 0xffffffff;
}

/* ------------------------------------------------------------------
 * _write_r   @ 0x5112
 * Signature: undefined FUN_ram_00005112()
 * ------------------------------------------------------------------ */
void _write_r(int *param_1,undefined4 param_2,undefined4 param_3,undefined4 param_4)

{
  int iVar1;
  
  _DAT_ram_200011ac = 0;
  iVar1 = usart_poll_write(param_2,param_3,param_4);
  if ((iVar1 == -1) && (_DAT_ram_200011ac != 0)) {
    *param_1 = _DAT_ram_200011ac;
  }
  return;
}

/* ------------------------------------------------------------------
 * sub_5148   @ 0x5148
 * Signature: undefined FUN_ram_00005148()
 *
 * WCH 16-bit load/store sites (quadrant-2 custom opcodes, invisible to the decompiler):
 *   0x5196  sh a4,0xc(s0)
 *   0x51DE  lhu a5,0xc(s0)
 *   0x51E8  sh a5,0xc(s0)
 *   0x51EE  lhu a5,0xc(s0)
 *   0x51F4  sh a5,0xc(s0)
 *   0x51FA  lhu a5,0xc(s0)
 *   0x520E  lhu a5,0xc(s0)
 *   0x5236  sh a5,0xc(s0)
 * ------------------------------------------------------------------ */
undefined4 sub_5148(undefined4 *param_1,undefined4 *param_2)

{
  ushort uVar1;
  int iVar2;
  undefined4 uVar3;
  uint uVar4;
  uint extraout_a5;
  
  iVar2 = *(int *)(gp + -0x7ec);
  if ((iVar2 != 0) && (*(int *)(iVar2 + 0x18) == 0)) {
    sub_468C(iVar2);
  }
  if (param_2 == (undefined4 *)&DAT_ram_00005d74) {
    param_2 = *(undefined4 **)(iVar2 + 4);
  }
  else if (param_2 == (undefined4 *)&DAT_ram_00005d94) {
    param_2 = *(undefined4 **)(iVar2 + 8);
  }
  else if (param_2 == (undefined4 *)&DAT_ram_00005d54) {
    param_2 = *(undefined4 **)(iVar2 + 0xc);
  }
  uVar1 = *(ushort *)(param_2 + 3);
  if ((uVar1 & 8) == 0) {
    if ((uVar1 & 0x10) == 0) {
      *param_1 = 9;
      return 0xffffffff;
    }
    if ((uVar1 & 4) != 0) {
      if ((undefined4 *)param_2[0xd] != (undefined4 *)0x0) {
        if ((undefined4 *)param_2[0xd] != param_2 + 0x11) {
          sub_5546(param_1);
        }
        param_2[0xd] = 0;
      }
      param_2[1] = 0;
      *param_2 = param_2[4];
    }
  }
  uVar4 = param_2[4];
  if (uVar4 == 0) {
    memcpy(param_1,param_2);
    uVar4 = extraout_a5;
  }
  if ((uVar4 & 1) == 0) {
    uVar3 = 0;
    if ((uVar4 & 2) == 0) {
      uVar3 = param_2[5];
    }
    param_2[2] = uVar3;
  }
  else {
    param_2[2] = 0;
    param_2[6] = -param_2[5];
  }
  if (param_2[4] != 0) {
    return 0;
  }
  if ((*(ushort *)(param_2 + 3) & 0x80) == 0) {
    return 0;
  }
  return 0xffffffff;
}

/* ------------------------------------------------------------------
 * sub_5246   @ 0x5246
 * Signature: undefined FUN_ram_00005246()
 * ------------------------------------------------------------------ */
void sub_5246(int *param_1,undefined4 param_2)

{
  int iVar1;
  
  _DAT_ram_200011ac = 0;
  iVar1 = sub_5684(param_2);
  if ((iVar1 == -1) && (_DAT_ram_200011ac != 0)) {
    *param_1 = _DAT_ram_200011ac;
  }
  return;
}

/* ------------------------------------------------------------------
 * sub_5276   @ 0x5276
 * Signature: undefined FUN_ram_00005276()
 *
 * WCH 16-bit load/store sites (quadrant-2 custom opcodes, invisible to the decompiler):
 *   0x5276  lhu a5,0xc(a1)
 *   0x52C0  lhu a5,0xc(s0)
 *   0x52DE  lhu a4,0xc(s0)
 *   0x535C  sh a4,0xc(s0)
 *   0x5394  lhu a5,0xc(s0)
 *   0x539A  sh a5,0xc(s0)
 * ------------------------------------------------------------------ */
undefined4 sub_5276(uint *param_1,int *param_2)

{
  int *piVar1;
  int iVar2;
  code *pcVar3;
  int extraout_a4;
  int iVar4;
  uint in_a5;
  uint uVar5;
  uint uVar6;
  int iVar7;
  
  if ((in_a5 & 8) == 0) {
    if ((param_2[1] < 1) && (param_2[0x10] < 1)) {
      return 0;
    }
    pcVar3 = (code *)param_2[0xb];
    if (pcVar3 == thunk__start) {
      return 0;
    }
    uVar6 = *param_1;
    *param_1 = 0;
    if ((int)(in_a5 << 0x13) < 0) {
      iVar2 = param_2[0x15];
    }
    else {
      iVar2 = (*pcVar3)(param_1,param_2[8],0,1);
      in_a5 = 0xffffffff;
      if (iVar2 == -1) {
        uVar5 = *param_1;
        in_a5 = 0;
        if (uVar5 != 0) {
          if ((uVar5 == 0x1d) || (uVar5 == 0x16)) {
            *param_1 = uVar6;
            return 0;
          }
          return 0xffffffff;
        }
      }
    }
    if (((in_a5 & 4) != 0) && (iVar2 = iVar2 - param_2[1], param_2[0xd] != 0)) {
      iVar2 = iVar2 - param_2[0x10];
    }
    iVar2 = (*(code *)param_2[0xb])(param_1,param_2[8],iVar2,0);
    if ((iVar2 == -1) && ((0x1d < *param_1 || ((0x20400001U >> (*param_1 & 0x1f) & 1) == 0)))) {
      return 0xffffffff;
    }
    param_2[1] = 0;
    *param_2 = param_2[4];
    if ((extraout_a4 << 0x13 < 0) && ((iVar2 != -1 || (*param_1 == 0)))) {
      param_2[0x15] = iVar2;
    }
    piVar1 = (int *)param_2[0xd];
    *param_1 = uVar6;
    if (piVar1 != (int *)0x0) {
      if (piVar1 != param_2 + 0x11) {
        sub_5546(param_1);
      }
      param_2[0xd] = 0;
    }
  }
  else {
    iVar2 = param_2[4];
    if (iVar2 != 0) {
      iVar7 = *param_2;
      *param_2 = iVar2;
      iVar7 = iVar7 - iVar2;
      iVar4 = 0;
      if ((in_a5 & 3) == 0) {
        iVar4 = param_2[5];
      }
      param_2[2] = iVar4;
      for (; 0 < iVar7; iVar7 = iVar7 - iVar4) {
        iVar4 = (*(code *)param_2[10])(param_1,param_2[8],iVar2,iVar7);
        if (iVar4 < 1) {
          return 0xffffffff;
        }
        iVar2 = iVar2 + iVar4;
      }
    }
  }
  return 0;
}

/* ------------------------------------------------------------------
 * sub_53A6   @ 0x53A6
 * Signature: undefined FUN_ram_000053a6()
 * ------------------------------------------------------------------ */
undefined4 sub_53A6(int param_1,undefined *param_2)

{
  undefined4 uVar1;
  
  if (*(int *)(param_2 + 0x10) == 0) {
    return 0;
  }
  if ((param_1 != 0) && (*(int *)(param_1 + 0x18) == 0)) {
    sub_468C();
  }
  if (param_2 == &DAT_ram_00005d74) {
    param_2 = *(undefined **)(param_1 + 4);
  }
  else if (param_2 == &DAT_ram_00005d94) {
    param_2 = *(undefined **)(param_1 + 8);
  }
  else if (param_2 == &DAT_ram_00005d54) {
    param_2 = *(undefined **)(param_1 + 0xc);
  }
  if (*(short *)(param_2 + 0xc) == 0) {
    return 0;
  }
  uVar1 = sub_5276(param_1);
  return uVar1;
}

/* ------------------------------------------------------------------
 * sub_540E   @ 0x540E
 * Signature: undefined FUN_ram_0000540e()
 * ------------------------------------------------------------------ */
void sub_540E(int *param_1,undefined4 param_2,undefined4 param_3,undefined4 param_4)

{
  int iVar1;
  
  _DAT_ram_200011ac = 0;
  iVar1 = sub_56B4(param_2,param_3,param_4);
  if ((iVar1 == -1) && (_DAT_ram_200011ac != 0)) {
    *param_1 = _DAT_ram_200011ac;
  }
  return;
}

/* ------------------------------------------------------------------
 * sub_5442   @ 0x5442
 * Signature: undefined FUN_ram_00005442()
 *
 * WCH 16-bit load/store sites (quadrant-2 custom opcodes, invisible to the decompiler):
 *   0x5456  lhu a5,0xc(s1)
 * ------------------------------------------------------------------ */
sub_5442(undefined4 param_1,int param_2,undefined4 *param_3,uint *param_4,undefined4 param_5
                ,uint param_6)

{
  int iVar1;
  undefined4 uVar2;
  uint extraout_a5;
  undefined1 auStack_68 [4];
  uint uStack_64;
  
  if (*(short *)(param_2 + 0xe) < 0) {
code_r0x00005458:
    *param_4 = 0;
    if ((param_6 & 0x80) != 0) {
      uVar2 = 0x40;
      goto LAB_ram_0000548a;
    }
  }
  else {
    iVar1 = strcmp(param_1,(int)*(short *)(param_2 + 0xe),auStack_68);
    param_6 = extraout_a5;
    if (iVar1 < 0) goto code_r0x00005458;
    *param_4 = (uint)((uStack_64 & 0xf000) == 0x2000);
  }
  uVar2 = 0x400;
LAB_ram_0000548a:
  *param_3 = uVar2;
  return 0;
}

/* ------------------------------------------------------------------
 * memcpy   @ 0x5498
 * Signature: undefined FUN_ram_00005498()
 *
 * WCH 16-bit load/store sites (quadrant-2 custom opcodes, invisible to the decompiler):
 *   0x5498  lhu a5,0xc(a1)
 *   0x54E8  sh a5,0xc(s0)
 *   0x54F6  lhu a5,0xc(s0)
 *   0x5500  sh a5,0xc(s0)
 *   0x5514  lhu a5,0xc(s0)
 *   0x551C  sh a5,0xc(s0)
 *   0x551E  lhu a5,0xc(s0)
 * ------------------------------------------------------------------ */
void memcpy(int param_1,int *param_2)

{
  ushort uVar1;
  int iVar2;
  ushort extraout_a5;
  ushort uVar3;
  uint in_a5;
  int iStack_18;
  int iStack_14;
  
  if ((in_a5 & 2) == 0) {
    uVar1 = sub_5442(param_1,param_2,&iStack_18,&iStack_14);
    iVar2 = sub_4802(param_1,iStack_18);
    if (iVar2 != 0) {
      *(code **)(param_1 + 0x28) = sub_463C;
      *param_2 = iVar2;
      param_2[4] = iVar2;
      param_2[5] = iStack_18;
      uVar3 = 0;
      if ((iStack_14 != 0) &&
         (iVar2 = sub_5654(param_1,(int)*(short *)((int)param_2 + 0xe)), uVar3 = extraout_a5
         , iVar2 != 0)) {
        uVar3 = extraout_a5 & 0xfffc | 1;
      }
      *(ushort *)(param_2 + 3) = uVar1 | uVar3;
      return;
    }
    if ((*(ushort *)(param_2 + 3) & 0x200) != 0) {
      return;
    }
  }
  *param_2 = (int)param_2 + 0x47;
  param_2[4] = (int)param_2 + 0x47;
  param_2[5] = 1;
  return;
}

/* ------------------------------------------------------------------
 * sub_552A   @ 0x552A
 * Signature: undefined FUN_ram_0000552a()
 * ------------------------------------------------------------------ */
char * sub_552A(char *param_1,char param_2,int param_3)

{
  char *pcVar1;
  
  pcVar1 = param_1 + param_3;
  while( true ) {
    if (param_1 == pcVar1) {
      return (char *)0x0;
    }
    if (*param_1 == param_2) break;
    param_1 = param_1 + 1;
  }
  return param_1;
}

/* ------------------------------------------------------------------
 * sub_5542   @ 0x5542
 * Signature: undefined FUN_ram_00005542()
 * ------------------------------------------------------------------ */
void sub_5542(void)

{
  return;
}

/* ------------------------------------------------------------------
 * sub_5544   @ 0x5544
 * Signature: undefined FUN_ram_00005544()
 * ------------------------------------------------------------------ */
void sub_5544(void)

{
  return;
}

/* ------------------------------------------------------------------
 * sub_5546   @ 0x5546
 * Signature: undefined FUN_ram_00005546()
 * ------------------------------------------------------------------ */
void sub_5546(undefined4 *param_1,int param_2)

{
  int *piVar1;
  int *piVar2;
  int iVar3;
  int *piVar4;
  int iVar5;
  int *piVar6;
  
  if (param_2 == 0) {
    return;
  }
  piVar1 = (int *)(param_2 - 4);
  if (*(int *)(param_2 + -4) < 0) {
    piVar1 = (int *)((int)piVar1 + *(int *)(param_2 + -4));
  }
  sub_5542();
  piVar6 = *(int **)(gp + -0x774);
  if (piVar6 == (int *)0x0) {
    piVar1[1] = 0;
  }
  else {
    if (piVar6 <= piVar1) {
      while ((piVar4 = (int *)piVar6[1], piVar4 != (int *)0x0 && (piVar4 <= piVar1))) {
        piVar6 = piVar4;
      }
      piVar2 = (int *)((int)piVar6 + *piVar6);
      if (piVar2 == piVar1) {
        iVar3 = *piVar6 + *piVar1;
        *piVar6 = iVar3;
        if (piVar4 == (int *)((int)piVar6 + iVar3)) {
          iVar5 = piVar4[1];
          *piVar6 = iVar3 + *piVar4;
          piVar6[1] = iVar5;
        }
      }
      else if (piVar1 < piVar2) {
        *param_1 = 0xc;
      }
      else {
        if (piVar4 == (int *)((int)piVar1 + *piVar1)) {
          iVar3 = *piVar4;
          piVar4 = (int *)piVar4[1];
          *piVar1 = iVar3 + *piVar1;
        }
        piVar1[1] = (int)piVar4;
        piVar6[1] = (int)piVar1;
      }
      goto LAB_ram_00005574;
    }
    if (piVar6 == (int *)((int)piVar1 + *piVar1)) {
      iVar3 = *piVar6;
      piVar6 = (int *)piVar6[1];
      *piVar1 = iVar3 + *piVar1;
    }
    piVar1[1] = (int)piVar6;
  }
  *(int **)(gp + -0x774) = piVar1;
LAB_ram_00005574:
  sub_5544(param_1);
  return;
}

/* ------------------------------------------------------------------
 * strlen   @ 0x55EE
 * Signature: undefined FUN_ram_000055ee()
 * ------------------------------------------------------------------ */
void strlen(int *param_1,undefined4 param_2,undefined4 param_3,undefined4 param_4)

{
  int iVar1;
  
  _DAT_ram_200011ac = 0;
  iVar1 = sub_56C4(param_2,param_3,param_4);
  if ((iVar1 == -1) && (_DAT_ram_200011ac != 0)) {
    *param_1 = _DAT_ram_200011ac;
  }
  return;
}

/* ------------------------------------------------------------------
 * strcmp   @ 0x5622
 * Signature: undefined FUN_ram_00005622()
 * ------------------------------------------------------------------ */
void strcmp(int *param_1,undefined4 param_2,undefined4 param_3)

{
  int iVar1;
  
  _DAT_ram_200011ac = 0;
  iVar1 = sub_5694(param_2,param_3);
  if ((iVar1 == -1) && (_DAT_ram_200011ac != 0)) {
    *param_1 = _DAT_ram_200011ac;
  }
  return;
}

/* ------------------------------------------------------------------
 * sub_5654   @ 0x5654
 * Signature: undefined FUN_ram_00005654()
 * ------------------------------------------------------------------ */
void sub_5654(int *param_1,undefined4 param_2)

{
  int iVar1;
  
  _DAT_ram_200011ac = 0;
  iVar1 = sub_56A4(param_2);
  if ((iVar1 == -1) && (_DAT_ram_200011ac != 0)) {
    *param_1 = _DAT_ram_200011ac;
  }
  return;
}

/* ------------------------------------------------------------------
 * sub_5684   @ 0x5684
 * Signature: undefined FUN_ram_00005684()
 * ------------------------------------------------------------------ */
undefined4 sub_5684(void)

{
  _DAT_ram_200011ac = 0x58;
  return 0xffffffff;
}

/* ------------------------------------------------------------------
 * sub_5694   @ 0x5694
 * Signature: undefined FUN_ram_00005694()
 * ------------------------------------------------------------------ */
undefined4 sub_5694(void)

{
  _DAT_ram_200011ac = 0x58;
  return 0xffffffff;
}

/* ------------------------------------------------------------------
 * sub_56A4   @ 0x56A4
 * Signature: undefined FUN_ram_000056a4()
 * ------------------------------------------------------------------ */
undefined4 sub_56A4(void)

{
  _DAT_ram_200011ac = 0x58;
  return 0;
}

/* ------------------------------------------------------------------
 * sub_56B4   @ 0x56B4
 * Signature: undefined FUN_ram_000056b4()
 * ------------------------------------------------------------------ */
undefined4 sub_56B4(void)

{
  _DAT_ram_200011ac = 0x58;
  return 0xffffffff;
}

/* ------------------------------------------------------------------
 * sub_56C4   @ 0x56C4
 * Signature: undefined FUN_ram_000056c4()
 * ------------------------------------------------------------------ */
undefined4 sub_56C4(void)

{
  _DAT_ram_200011ac = 0x58;
  return 0xffffffff;
}
