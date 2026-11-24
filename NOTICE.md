# NOTICE

本仓库包含的第三方材料与相应归属如下。

## 分析对象

固件的版权归原厂或其权利人所有。本仓库不包含该二进制文件，也不包含由它派生的镜像文件，仅包含对它的分析结果与重建源码。重建源码是从机器码推断得到的，不复制任何厂商源码文本。

## Ghidra

分析所用的 Ghidra 脚本（`Seed`、`Main`、`Dump`、`Patch`、`Ls`、`Probe`、`Gaps`）与 RISC-V 语言补丁面向 Ghidra 12.2 编写，**未随本仓库发布**，本仓库只记录其用途与修改点（见 [`docs/METHOD.md`](docs/METHOD.md)）。Ghidra 由美国国家安全局研究局开发并以 Apache License 2.0 发布，本仓库不包含也不重新分发 Ghidra 本体。

语言补丁修改的是 Ghidra 自带的 RISC-V 语言定义文件（`riscv.rvc.sinc`、`riscv.table.sinc`），需在本地 Ghidra 安装目录中应用；修改方法与关键片段见 [`docs/METHOD.md`](docs/METHOD.md)。这些文件的原始内容同样受 Apache License 2.0 约束。

## 固件中的 C 库

`decompiled/printf.c` 与 `decompiled/lib.c` 反映的是厂商固件中链接的 C 库实现，其来源特征与 newlib 一致。newlib 采用 BSD 风格的多重许可，允许在保留版权声明的前提下使用与再分发。本仓库的重建源码由反编译得到，不含这些库的原始注释与版权头，因此此处仅作来源说明，不重复其条款。

若需要以 newlib 源码为参照，请从 newlib 项目获取并遵守其许可。

## USB 描述符与字符串

`decompiled/strings.h` 与 `ANALYSIS.md` 第 4 节引用了固件中的 USB 描述符与字符串常量，包括厂商 ID、产品 ID 与 `wch.cn`、`CH32x035` 等文本。这些内容来自 WCH 的示例工程，相关标识归 WCH 所有。引用它们是为了说明该固件的来源与兼容性影响，不代表任何授权关系。

## 本仓库自有内容

`README.md`、`ANALYSIS.md`、`docs/`、`data/`、`reports/` 中由本仓库撰写的内容，版权归仓库作者所有。本仓库未附许可文件，内容仅供研究与学习使用。

## 商标

`MSU2`、`Moriateliers`、`WCH`、`CH32`、`RISC-V`、`Ghidra` 等名称与商标归各自权利人所有。