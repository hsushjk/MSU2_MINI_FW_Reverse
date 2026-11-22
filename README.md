# MSU2_MINI_FW_Reverse

MSU2_MINI 可编程 USB 屏幕（`Moriateliers` / `MSU2M-2403`）**官方固件 V1.1** 的反汇编 / 反编译结果与分析报告。

## ⚠️ 声明（请先读）

- **来源**：本仓库内容由一片市售 MSU2_MINI 模块的官方固件 `[固件]MSU2_MINI_Firmware_V1.1.hex`（Intel HEX，68 156 B，代码区 0x0000–0x5E9F，共 24 224 B）逆向分析得出。分析所用的 Ghidra 工程、原始镜像、反向工程脚本均为个人研究与学习用途。
- **目的**：互操作性研究——理解该硬件的接口语义，以便独立实现可替换的固件与上位机。这属于《计算机软件保护条例》下为兼容性目的进行反向工程的范畴。
- **不提供任何许可**：本仓库没有 LICENSE 文件，即"保留所有权利"。**未授予**任何复制、修改、再分发、商业使用的许可。
- **不含**原始固件二进制与厂商手册 PDF（避免二次分发厂商版权物）。如需原厂固件，请自行获取。
- 本仓库内容**不是**原厂源码，而是从机器码推断的近似重建：变量名、结构体布局、函数边界都可能与真实源码不同；数据流正确，但名称仅作说明用。
- "MSU2"、"Moriateliers"、"WCH"、"CH32" 等名称与商标归各自权利人所有，本项目与其无任何关联或授权关系。
- 分析结论按"尽力而为"提供，不附带任何担保。

硬件连线表与协议规范（可自由使用，MIT）在另一个仓库：[Open_MSU2_MINI](https://github.com/hsushjk/Open_MSU2_MINI)。

---

## 目录

```
ANALYSIS.md          完整分析文档（硬件 / 启动链 / 运行时 / USB / 协议 / 显示存储 / 缺陷清单 24 条 / 复现方法）
decompiled/          反编译 C 源码，12 个模块 + registers.h + strings.h + index.md
reports/             6 份专项复验报告（USB、启动链、flash/屏、绘图、HAL、printf）
tools/ghidra_scripts 分析用的 Ghidra 脚本（反汇编、建函数、修补、导出）
tools/ann.js         把不可解码的自定义指令标注回反编译结果
tools/peek.js        按地址查看反汇编上下文
docs/METHOD.md       完整方法学：Ghidra 语言补丁、可达性修复、验证方法、复现步骤
```

反编译源码规模：**165 个函数、12 个模块、858 个外设寄存器名、14 个字符串**。

| 模块 | 内容 | 规模 |
|---|---|---|
| `crt.c` / `boot.c` | 启动、系统初始化、时钟 | 1 877 B / 6 043 B |
| `main.c` | 主函数 | 2 841 B |
| `cmd.c` | 命令分派（6 字节帧） | 13 195 B |
| `uart.c` | USART2 调试输出、DMA | 8 048 B |
| `usb.c` | USB 设备栈、中断、端点 | 20 637 B |
| `flash.c` | SPI flash 全部操作 | 21 903 B |
| `lcd.c` | ST7735S 驱动 | 9 232 B |
| `draw.c` | 绘图、贴图、差分刷屏 | 23 123 B |
| `hal.c` | 外设库（GPIO/SPI/USART/DMA/ADC/RCC） | 23 121 B |
| `printf.c` | 完整 printf 实现 | 38 775 B |
| `lib.c` | memset / memcpy / strlen / strcmp 等 | 5 123 B |

## 关键结论

1. **主控是 CH32X033F8P6（RISC-V 青稞内核）**，固件里大量使用了 WCH 私有压缩指令扩展——这套扩展占用了标准 RVC 的浮点槽位，而芯片没有 FPU，所以 GCC 本来不会发射浮点指令。这是本次反编译最大的障碍，详见 `docs/METHOD.md`。
2. **`main` 根本不在 Ghidra 的自动分析范围内**：crt0 通过 `mepc = 0x39B4; mret` 跳进 `main`，任何基于调用流/引用的分析都追不到，导致 2.3 KB 主程序和它的整棵调用树（含命令分派、主循环）全部缺失。修补后函数数从 101 涨到 165。
3. **固件走的是 WCH 官方例程改的**：USB 描述符至今是 `VID 0x1A86 / PID 0xFE0C`，厂商字符串是 `wch.cn` / `CH32x035` / `0123456789`，一个字都没改。
4. **时钟只有 48 MHz HSI，没有 PLL、没有外部晶振**（`SystemInit` 就三条语句），却把 SPI 跑在 24 MHz 去驱 ST7735S（官方上限约 15 MHz）。
5. **PA6 一脚两用**（flash MISO + 屏 D/C），导致屏幕上那两组 SPI 排针只能写不能读；**PA9 是空脚**，固件每次刷屏仍翻转 28 次。
6. **没有看门狗**（全镜像零处访问 0x400030），**没有任何通信超时重传**，只靠 1 kHz tick 做 5 秒会话超时。
7. **命令协议是 6 字节裸帧**，无长度、无校验、无序号；`SET_REPORT` 路径可以把主机数据写到 `0x20000140 + addr`，`addr` 全由主机决定 —— 一个可越界写 SRAM 的洞。
8. **擦除范围用 16 位累加**，`P + N > 0xFFFF` 回绕会擦错区域。
9. **`len == 0` 会让刷屏函数死等**（`lcd_stream` / `lcd_stream_end` 直接用帧参数当循环次数）。
10. **授权校验形同虚设**：`licence_check` 只在开机时决定要不要显示三行"isn't activated"，不 gate 任何功能；而且它写的那个标志位没有其它读取点。

缺陷完整清单（A/B/C/D/E 五级共 24 条，每条带证明地址）见 `ANALYSIS.md` §7。

## 可靠性与已知残留问题

- **数据流是可靠的**：关键语义都做过双向验证（固件实现 ↔ 官方上位机实现 ↔ 实机行为）。
- **名称是推测的**：`decompiled/` 里的函数名、变量名、结构体成员名是分析者取的，不代表原厂命名。
- **236 个 WCH 私有指令**（16 位 `lhu`/`sh` 形式）**无法被 Ghidra 直接解码**，因此在反编译输出中以空操作标记 + `ann.js` 独立标注的方式呈现。受此影响，少数函数里会出现 `extraout_*` 之类的伪变量（实为进入函数时的参数寄存器），阅读时请对照 `ANALYSIS.md` 与 `reports/`。
- `-msave-restore` 的寄存器存取桩已被改写为 `ret`，函数尾部的公共还原代码保留在 0x13C 之后。

## 验证

反编译结论不是"看代码猜语义"，而是三重验证：

1. **6 路并行专项复验**：每个模块由一个独立分析者逐函数核对，产出 `reports/` 下 6 份报告；
2. **18 个语义锚点**：例如 `0x122C` 的 `lhu a5,0x32(s0)` 对应 USBFS `UEP4_CTRL_H`、`0x4196` 的 `sh a1,0xc(a0)` 对应 `SPI1->DATAR`；
3. **实机验证**：按反编译结论重写了开机画面（自检彩条）并生成可刷固件，模块刷入后正常显示 8 色竖条 —— 证明 `lcd_fill_colour(x,y,w,h,colour)` 的语义与参数顺序理解正确。

## 复现

工具链：Ghidra 12.2 + JDK 21。完整步骤（含语言补丁的精确 diff、sleigh 编译命令、踩过的坑）见 [docs/METHOD.md](docs/METHOD.md)。

```bash
# 1. hex → bin（24 224 B，0x0–0x5E9F）
# 2. 打语言补丁并重编译 riscv.ilp32d.sla
sleigh.exe riscv.ilp32d.slaspec
# 3. 建工程（关键是 -preScript Main.java 强制建 main）
analyzeHeadless <proj> MSU2 -import image.bin -processor RISCV:LE:32:default \
  -loader BinaryLoader -preScript Seed.java -preScript Main.java \
  -preScript Patch.java -postScript Dump.java
# 4. 导出反汇编与标注
Ls.java 0x0 0x5ea0 40000
node ann.js
```

## 为什么原厂固件"拉跨"

一句话总结：**它是一份演示工程的半成品**——WCH 例程的 USB 栈 + 手写状态机 + 差分刷屏协议，硬件上还留着一个空脚和一个两用脚；没有看门狗、没有超时、没有边界检查、没有版本协商，产品化该做的加固一件没做。详见 `ANALYSIS.md` §7。