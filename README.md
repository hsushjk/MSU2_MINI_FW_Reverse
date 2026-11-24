# MSU2_MINI_FW_Reverse

MSU2_MINI 小屏原厂固件（V1.1）的静态反编译产物与分析报告。目标器件为 CH32X033F8P6（RISC-V，48 MHz，20 KB SRAM，Flash 标称 64 KB、可用 62 KB）。在没有调试接口、符号表和源码的前提下，本仓库把一个 24 224 B 的固件镜像还原成可读的 C 代码、函数表、寄存器映射与缺陷清单，并记录所用的方法与证据。

本仓库**不包含原厂固件本体**（镜像、HEX 文件及其派生的二进制块均不在仓库内），只包含分析结果、重建源码与数据表。

## 仓库内容

| 内容 | 说明 |
| --- | --- |
| 重建源码 | `decompiled/` 下按模块拆分的 C 文件，共 12 个模块、165 个函数，保留原始地址与调用关系 |
| 分析报告 | `ANALYSIS.md`：启动链、运行时结构、USB、命令协议、显示与存储、缺陷清单、复现方法 |
| 原始数据 | `data/`：反汇编清单、调用图、函数表、符号表、寄存器映射、字符串表、缺陷清单 |
| 方法文档 | `docs/METHOD.md`：Ghidra 环境、私有指令扩展的处理、可达性修复、验证手段 |
| 指纹 | `docs/FINGERPRINT.md`：镜像大小、首字、函数数、字符串特征，用于确认输入 HEX 的版本 |
| 结构图 | `docs/diagrams/`：启动链、命令分派、状态机、USB 数据通路四张图 |
| 专项报告 | `reports/`：USB、时钟与内存布局、NOR 与屏、绘图、HAL、printf 六份逐项复核 |

本仓库不提供 Ghidra 分析脚本、语言补丁与源码生成脚本；`docs/METHOD.md` 记录了这些工具的用途、修改点与关键片段，可据此在本地复现。

## 目录结构

```text
ANALYSIS.md               主分析报告
NOTICE.md                 第三方材料与商标说明
decompiled/               重建源码（12 个模块）、registers.h、strings.h、index.md 函数索引
data/                     calls.tsv  index.tsv  symbols.tsv  regmap.json  strings.tsv
                          listing7.txt（全镜像反汇编）  q2sites.txt（私有指令位置）
                          defects.csv（缺陷清单）
docs/                     METHOD.md  FINGERPRINT.md  diagrams/{boot,commands,states,usb-path}.svg
reports/                  g1_usb.md … g6_printf.md
```

## 如何阅读

1. 先看 [`ANALYSIS.md`](ANALYSIS.md) §0 的结论摘要，了解这份固件的整体形态与主要问题。
2. 按兴趣读 §2–§6：启动链、运行时内存布局、USB 端点、6 字节命令协议、显示与片外 NOR 的使用方式。
3. §7 是 41 条缺陷清单，按 A（会挂死）/B（会画错）/C（协议与并发）/D（功能缺失与浪费）/E（工程质量）分级，并给出修复优先级；机器可读版本在 [`data/defects.csv`](data/defects.csv)。
4. 需要核对证据时查 `reports/` 与 `data/`：`data/listing7.txt` 是全镜像反汇编，报告的每条结论都能在其中定位到具体指令地址。
5. 读源码建议从 [`decompiled/main.c`](decompiled/main.c) 开始，再看 `boot.c`（启动与时钟）、`usb.c`（中断与端点）、`cmd.c`（协议分派），最后是 `flash.c`、`lcd.c`、`draw.c`、`hal.c`。每个函数头部标有原始地址、调用者，以及它用到的自定义指令的真实语义。
6. 四张结构图分别对应：`boot.svg` 启动链与初始化顺序，`commands.svg` 命令分派与协议层缺陷，`states.svg` 主循环状态机，`usb-path.svg` USB 批量端点与 DMA 缓冲的地址布局。

## 反编译产物一览

| 模块 | 文件 | 函数数 | 内容 |
| --- | --- | --- | --- |
| 启动 | `boot.c` | 9 | `reset_entry`、`SystemInit`、向量表初始化 |
| C 运行时 | `crt.c` | 1 | crt0：拷贝 `.data`、清零 `.bss`、跳转 main |
| 主程序 | `main.c` | 1 | `main_loop`（0x59A） |
| 命令 | `cmd.c` | 3 | `cmd_dispatch`（0x250）、`reset_session`（0x584）、命令分派跳转表 |
| 绘图 | `draw.c` | 17 | 填充、贴图、单色/混色图、差分刷屏 |
| Flash | `flash.c` | 22 | NOR 读/写/擦除、擦除范围计算 |
| 屏 | `lcd.c` | 8 | ST7735S 初始化、窗口设置、方向设置、ADC 触摸采样 |
| HAL | `hal.c` | 43 | 时钟、GPIO、SPI、ADC、TIM、看门狗相关的外设访问 |
| USB | `usb.c` | 9 | 设备栈、端点处理、`SET_REPORT` 数据通路 |
| 串口 | `uart.c` | 7 | USART2 发送 |
| 库 | `lib.c` | 8 | `memset`、`memcpy6` 等 |
| printf | `printf.c` | 37 | 精简 printf 引擎 |

寄存器与字符串符号分别在 [`decompiled/registers.h`](decompiled/registers.h) 与 [`decompiled/strings.h`](decompiled/strings.h)；地址、名字、所属模块与函数体行数的总表在 [`decompiled/index.md`](decompiled/index.md)。

## 分析环境与复现

### 环境

| 项 | 值 |
| --- | --- |
| Ghidra | 12.2（JDK 21） |
| 处理器 | `RISCV:LE:32:default` |
| Loader | `BinaryLoader`，基址 `0x0` |
| 输入 | 原厂 HEX 解析出的裸 bin（68 156 B 的 Intel HEX → 24 224 B 镜像，占 `0x0000`–`0x5E9F`） |

### 三个必须处理的障碍

| # | 障碍 | 现象 | 处理 |
| --- | --- | --- | --- |
| 1 | main 不可达 | crt0（0x3BB4）用 `lui t0,0x4; csrw mepc,t0; mret` 跳到 0x39B4，自动分析不跟 `mret`，只得到 101 个函数 | 在 0x39B4 强制造函数：`toAddr(0x39B4); disassemble(a); createFunction(a,"main")`，函数数变为 165 |
| 2 | `-msave-restore` 存根 | 0x100/0x108/0x116/0x130 共享尾部 0x12E 的 `c.jr t0`，被识别成普通函数，产生大量 `extraout_a0` | 先 `clearListing`（否则 `MemoryAccessException: Memory change conflicts with instruction`），再把 0x12E、0x13A 改成 `c.jr ra`（`0x8082`）。`extraout_*` 实际表示入参寄存器 |
| 3 | 命令分派跳转表 | `c.lui a4,0x5; c.slli a5,2; addi a4,a4,0x6D4`，表基址 0x56D4 | 按表逐个建立 17 个 `case` 目标函数 |

### 命令行框架

```powershell
analyzeHeadless <proj> MSU2 `
  -import image.bin `
  -processor RISCV:LE:32:default -loader BinaryLoader `
  -preScript Seed.java -preScript Main.java -preScript Patch.java `
  -postScript Dump.java
```

上述 Ghidra 脚本（`Seed`、`Main`、`Patch`、`Dump`、`Ls`、`Probe`、`Gaps`）、语言补丁与源码生成脚本**未随本仓库发布**，命令行仅示意分析流程；补丁内容与脚本逻辑见 [`docs/METHOD.md`](docs/METHOD.md)。

### 私有压缩指令

WCH 在这颗核上重定义了标准 RVC 编码中浮点压缩指令的槽位（芯片无 FPU，编译器不会生成浮点指令），用于 16 位 `lhu`/`sh` 一类的半字访存。原版 Ghidra 的 sleigh 描述不认识这些编码，需要修改 RISC-V 语言定义（`riscv.rvc.sinc`、`riscv.table.sinc`）并重编 `.sla` 才能完整反汇编。这类指令在象限 2 共 179 处，在产物中保留为无操作数标记指令；位置记在 [`data/q2sites.txt`](data/q2sites.txt)，语义写在每个函数的头部注释里。

## 分析结论索引

`ANALYSIS.md` 章节与行号：

| 章节 | 行 | 内容 |
| --- | --- | --- |
| §0 结论摘要 | 10 | 十条主要结论 |
| §1 目标硬件 | 23 | 芯片、Flash、外设、引脚约束 |
| §2 启动链 | 68 | `reset_entry` → crt0 → `mret` → main |
| §3 运行时结构 | 87 | 全局变量、缓冲、定时器 |
| §4 USB 设备 | 105 | 描述符、端点、`SET_REPORT` 通道 |
| §5 命令协议 | 130 | 6 字节记录、class 分派、刷屏流程 |
| §6 显示与存储 | 153 | 屏初始化、差分编码、NOR 使用 |
| §7 缺陷清单 | 162 | 41 条分级：A 168 / B 183 / C 196 / D 214 / E 225 / 修复优先级 236 |
| §8 复现方法 | 248 | 8.1 私有指令与可达性 250 / 8.2 工具链 261 / 8.3 残留问题 275 |
| §9 交付物 | 284 | 文件清单与统计 |
| 附录 | 312 | 触摸通道 9 的采样方式 |

### 十条主要结论

| # | 结论 |
| --- | --- |
| 1 | WCH 私有压缩指令占用标准 RVC 的浮点槽位，需要补丁才能完整反编译 |
| 2 | main 在自动分析可达范围之外，靠 `mepc=0x39B4 + mret` 进入；强制造函数后函数数从 101 增至 165 |
| 3 | USB 沿用 WCH 示例工程：`1A86:FE0C`，字符串 `wch.cn`/`CH32x035`/`0123456789`；声明 CDC+HID 三个接口，实际只用 EP2 输出与 EP3 输入 |
| 4 | 只使用内部 48 MHz RC，无 PLL、无晶振；SPI1 以 24 MHz 驱动 ST7735S，高于手册给出的上限 |
| 5 | PA6 一脚两用（NOR MISO 与屏 D/C 复用），排针 SPI 只有写通路；PA9 在每次刷屏事务中翻转，但板上无负载 |
| 6 | 无看门狗（全镜像无一处访问 `0x40003000`），无通信超时重传，只有 1 kHz 定时器实现的 5 秒会话超时 |
| 7 | 6 字节裸帧无长度、校验与序号；`SET_REPORT` 把主机数据写到 `0x2000102C + 16 位累加器`，构成越界写 SRAM 的通道 |
| 8 | NOR 擦除范围按 16 位累加，`P+N > 0xFFFF` 回绕时可能擦到固件自身 |
| 9 | 流式输出长度不设限：`len==0` 时 DMA 不启动而函数无限轮询；绘图路径把 `w*h` 直接当循环次数，大尺寸会写穿窗口 |
| 10 | 授权校验不 gate 任何功能，只在开机时叠三行未授权提示，标志位没有其它读取点 |

### 专项报告

| 报告 | 主题 |
| --- | --- |
| [`reports/g1_usb.md`](reports/g1_usb.md) | USB 设备栈与端点路径 |
| [`reports/g2_plumbing.md`](reports/g2_plumbing.md) | 启动、时钟、定时器、内存布局 |
| [`reports/g3_flash_lcd.md`](reports/g3_flash_lcd.md) | NOR 与屏驱动 |
| [`reports/g4_drawing.md`](reports/g4_drawing.md) | 绘图与差分刷屏 |
| [`reports/g5_hal.md`](reports/g5_hal.md) | HAL 层逐函数说明 |
| [`reports/g6_printf.md`](reports/g6_printf.md) | printf 引擎 |

## 与开源重写固件的关系

本仓库的分析结果可用于对照理解同一硬件的另一套实现。对应的开源工程单独发布，不在本仓库内：

| 仓库 | 内容 |
| --- | --- |
| `Open_MSU2_MINI` | 硬件文档与 `netlist.csv` |
| `Open_MSU2_MINI_FW` | 重写固件（当前版本 1.0.9） |
| `Open_MSU2_MINI_Console` | 上位机 |

重写固件的协议与原厂固件不同，两者不可混用。当前配套固件的要点：

- 设备上电后主动发送 `\0OPMSU`，主机回 `\0OKMSU`；
- 版本查询 `00 56` 返回 7 字节 `56 MA MI PA TH TL AA`，末两字节为触摸通道 9 的 ADC 读数；
- `0A 0D` 复位面包屑返回 8 字节；
- `0A 11` 分块签名：100 B 输入对应 10 B 掩码，16×16 像素一块，共 10 列 × 5 行 = 50 块；
- `0A 14` 就地搬运必须携带 `A5 5A` 令牌，且本次上电已成功执行 `0A 13`，否则拒绝并回 `15 FE`；
- `0A 0F` 返回升级结果，结果码 5 表示镜像无效。

原厂固件使用 6 字节记录与 `\0` `M` `S` `N` 开头的数据块头，本仓库描述的协议、地址与缺陷均针对原厂固件。

## 已知限制

| 限制 | 说明 |
| --- | --- |
| 命名是推断的 | 函数名按功能推断，不是原厂符号 |
| 179 处私有指令无法直接解码 | 以无操作数形式出现，靠 `data/q2sites.txt` 与函数头注释补足 |
| `extraout_*` 伪变量 | 由存根改 `ret` 引起，实际表示入参寄存器 |
| 仅有静态分析 | 结论来自反汇编与源码重建，未做单步跟踪 |
| 不提供原厂固件 | 镜像本体不在仓库内，只提供 [`docs/FINGERPRINT.md`](docs/FINGERPRINT.md) 中的指纹用于版本比对 |

## 免责声明

本仓库是非官方研究资料，与 MSU2_MINI 的设计者、制造商及 WCH 均无隶属或授权关系。重建源码由机器码反编译得到，不代表原厂实现细节；分析中的命名、注释与结论均为推断，可能存在错误。内容仅供研究与学习使用，不提供任何担保。

仓库未附许可文件；第三方材料、商标与 C 库归属见 [`NOTICE.md`](NOTICE.md)。