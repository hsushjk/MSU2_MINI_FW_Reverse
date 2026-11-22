# MSU2_MINI 官方固件逆向分析

本仓库是对 MSU2_MINI 可编程 USB 屏幕（丝印 `Moriateliers`、`MSU2M-2403`）出厂固件 V1.1 的静态逆向结果，包含反编译源码、分析文档、复验报告与可复现的分析工具链。

分析目的为互操作性研究：弄清该硬件的接口语义，以便独立实现可替换的固件与上位机。仓库内容不是厂商源码，而是从机器码推断出的近似重建。

## 来源与边界

- 分析对象为市售模块的官方固件 `[固件]MSU2_MINI_Firmware_V1.1.hex`（Intel HEX，68 156 B，1 516 条记录），解析后代码区为 `0x0000`–`0x5E9F`，共 24 224 B。确切校验值见 [docs/FINGERPRINT.md](docs/FINGERPRINT.md)。
- 本仓库不存放厂商二进制、厂商手册 PDF、原始镜像与厂商原理图。需要这些材料请自行获取；若手中的文件指纹与本仓库记录不一致，则本仓库的地址、函数边界与缺陷结论都可能失效。
- 本仓库不设 LICENSE 文件，即保留所有权利，未授予复制、修改、再分发或商业使用的许可。硬件连线与通信协议在此仓库之外，以 MIT 许可单独发布。

## 仓库结构

```
ANALYSIS.md               分析主文档：硬件、启动链、运行时、USB、协议、显示与存储、缺陷清单、复现方法
decompiled/               反编译 C 源码：12 个模块 + registers.h + strings.h + index.md
reports/                  6 份专项复验报告（USB、启动链、flash 与屏、绘图、HAL、printf）
data/                     分析中间产物：符号表、寄存器映射、自定义指令标注、调用图、清单
docs/METHOD.md            方法学：Ghidra 语言补丁、可达性修复、验证手段、复现步骤
docs/FINGERPRINT.md       固件指纹与分区校验值
docs/diagrams/            四张结构图（启动链、状态机、命令分派、USB 数据通路）
tools/                    分析脚本：Ghidra 脚本、汇编与补丁、符号生成、源码生成、图示生成
tools/language-patch/     RISC-V 语言定义补丁
```

反编译产物规模：165 个函数、12 个模块、858 个外设寄存器名、14 个字符串常量。

| 模块 | 内容 | 体积 |
|---|---|---|
| `crt.c` / `boot.c` | 启动、系统初始化、时钟 | 1 877 B / 6 043 B |
| `main.c` | 主函数 | 2 841 B |
| `cmd.c` | 命令分派 | 13 195 B |
| `uart.c` | USART2 调试输出与 DMA | 8 048 B |
| `usb.c` | USB 设备栈、中断、端点 | 20 637 B |
| `flash.c` | SPI flash 操作 | 21 903 B |
| `lcd.c` | ST7735S 驱动 | 9 232 B |
| `draw.c` | 绘图、贴图、差分刷屏 | 23 123 B |
| `hal.c` | 外设库（GPIO/SPI/USART/DMA/ADC/RCC） | 23 121 B |
| `printf.c` | C 库格式化输出实现 | 38 775 B |
| `lib.c` | memset / memcpy / strlen / strcmp 等 | 5 123 B |

## 主要结论

1. 主控为 CH32X033F8P6（WCH 青稞 RISC-V 内核）。固件大量使用 WCH 私有压缩指令扩展，它们占用了标准 RVC 的浮点指令槽位；芯片本身没有浮点单元，GCC 不可能发射浮点指令，因此这些槽位被重新定义为字节与半字访存。这是本次反编译最主要的障碍，处理方式见 `docs/METHOD.md`。
2. `main` 处于所有自动分析的可达范围之外。crt0 通过 `mepc = 0x39B4; mret` 进入 `main`，基于调用流与引用关系的分析无法追踪，导致 2.3 KB 主程序及其整棵调用树（命令分派、主循环）在初次分析中完全缺失。修补后函数数由 101 增至 165。
3. USB 部分直接沿用 WCH 官方示例：VID `0x1A86`、PID `0xFE0C`，厂商字符串为 `wch.cn`、`CH32x035`、`0123456789`，均未修改；描述符声明了 CDC-ACM、CDC 数据与 HID 三个接口，实际只使用 EP2 输出与 EP3 输入。
4. 系统只用内部 48 MHz RC 振荡器，未启用 PLL 与外部晶振，`SystemInit` 仅三条语句；SPI1 却以 24 MHz 驱动 ST7735S，超过该屏约 15 MHz 的写周期上限。
5. PA6 一脚两用（flash 的 MISO 与屏的 D/C），因此板上两组 SPI 排针只有写通路可用；PA9 在固件中每次刷屏仍被翻转 28 次，但该脚在板上没有任何负载。
6. 固件不含看门狗（全镜像无一处访问 `0x40003000`），没有通信超时与重传，仅靠 1 kHz 定时器做 5 秒会话超时。
7. 命令协议为 6 字节裸帧，无长度字段、无校验、无序号。`SET_REPORT` 控制传输会把主机数据写到 `0x20000140 + addr`，地址完全由主机决定，构成可越界写 SRAM 的通道。
8. flash 擦除范围以 16 位累加，`P + N > 0xFFFF` 时回绕，可能擦除固件自身所在的区域。
9. 刷屏函数以帧参数直接作为循环次数，`len == 0` 时 DMA 不会启动而函数无限轮询，整机停在该处。
10. 授权校验不 gate 任何功能，仅在开机时决定是否叠加三行未授权提示；校验失败后写入的标志位没有其它读取点。

缺陷完整清单按严重程度分为 A 至 E 五级共 41 条，每条附证明地址与修复建议，见 `ANALYSIS.md` 第 7 节与 [data/defects.csv](data/defects.csv)。

## 方法与验证

分析在 Ghidra 12.2 上完成：先为 WCH 私有指令补丁语言定义并重新编译 `.sla`，再用预脚本强制建立 `main` 以恢复可达性，随后导出全镜像反汇编作为唯一权威数据源。

结论经过三重核对：

1. 六路并行复验，每个模块由独立分析逐函数核对并写入 `reports/`；
2. 语义锚点对照，例如 `0x122C` 的 `lhu a5,0x32(s0)` 对应 USBFS `UEP4_CTRL_H`，`0x4196` 的 `sh a1,0xc(a0)` 对应 `SPI1->DATAR`；
3. 实机验证，按反编译结论改写开机画面并生成可刷固件，模块刷入后正常显示 8 色竖条，证明 `lcd_fill_colour(x,y,w,h,colour)` 的语义与参数顺序判断正确。

复现所需的完整步骤见 [docs/METHOD.md](docs/METHOD.md)。摘要：

```bash
# 1. Intel HEX 转镜像并核对指纹
node tools/hex2bin.js "image.hex" image.bin

# 2. 应用 RISC-V 语言补丁并重新编译
cd $GHIDRA/Ghidra/Processors/RISCV/data/languages
git apply .../tools/language-patch/riscv-rvc.patch
git apply .../tools/language-patch/riscv-table.patch
$GHIDRA/Ghidra/Features/Decompiler/os/win_x86_64/sleigh.exe riscv.ilp32d.slaspec

# 3. 建工程，关键是 preScript 强制建立 main
analyzeHeadless <proj> MSU2 -import image.bin -processor RISCV:LE:32:default \
  -loader BinaryLoader -preScript Seed.java -preScript Main.java \
  -preScript Patch.java -postScript Dump.java

# 4. 导出反汇编与自定义指令标注
#    Ls.java 0x0 0x5ea0 40000 > listing.txt
node tools/ann.js
```

## 已知限制

- 数据流可靠：关键语义均做过固件实现、上位机实现与实机行为三方面核对。
- 命名是推测的：`decompiled/` 中的函数名、变量名与结构体成员名由分析者拟定，不代表原厂命名。
- 象限 2 的 179 处 WCH 私有指令（16 位 `lhu` / `sh`）无法被 Ghidra 直接解码，在反编译结果中以无操作数标记出现，由 `data/q2sites.txt` 与各函数头部的注释补足语义。受此影响，少数函数中会出现 `extraout_*` 形式的伪变量，其真实含义是进入函数时的参数寄存器。
- `-msave-restore` 的寄存器存取桩已改写为 `ret`，函数尾部的公共还原代码保留在 `0x13C` 之后。

## 相关仓库

- [Open_MSU2_MINI](https://github.com/hsushjk/Open_MSU2_MINI)：硬件连线、物料清单与通信协议，以 MIT 许可发布。

## 商标与免责

`MSU2`、`Moriateliers`、`WCH`、`CH32` 等名称与商标归各自权利人所有，本仓库与权利人无任何关联或授权关系。全部分析结论按现状提供，不附带担保。