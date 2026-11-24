# 方法学：从 HEX 到可读 C

记录从原厂 HEX 到可读 C 的完整路径，包括两个主要障碍（**私有指令扩展**、**入口不可达**）以及若干工程细节。目标不只是产出可读伪代码，还要让结论足以支撑同一硬件的另一套实现。

## 0. 环境

| 项 | 值 |
|---|---|
| 反汇编器 | Ghidra 12.2（`RISCV:LE:32:default`，`BinaryLoader`，base 0x0） |
| JDK | 21 |
| 原始素材 | `MSU2_MINI_Firmware_V1.1.hex`（Intel HEX，68 156 B） |
| 镜像 | 解析后 24 224 B，连续占用 0x0000–0x5E9F |

## 1. 载入与第一印象

Intel HEX 逐记录解析（类型 00/01/04/05）后得到裸镜像。首字 `0x3B50306F` = `jal x0,+0x3BB4`（跳到 crt0），全镜像有 **109 个 `0x8082` 半字**——说明是 RV32 + C 扩展，且大量使用 `ret`。

手册确认主控为 CH32X033F8P6（RISC-V，48 MHz，20 KB SRAM；Flash 标称 64 KB、可用 62 KB）。Ghidra 自动分析跑完后只有 **101 个函数**，且明显缺了两大块：`0x0240–0x0B33`、`0x3922–0x3B32`。

## 2. 障碍一：`main` 不可达

crt0（0x3BB4）初始化 gp/sp 后，用 CSR 直接跳进 main：

```asm
lui   t0, 0x4        # 0x39B4
csrw  mepc, t0
mret
```

Ghidra 的流程分析、引用分析、甚至"激进指令查找器"（`AggressiveInstructionFinderAnalyzer`）都**追不到 `mret` 跳转**，于是 `main` 和它下面 2.3 KB 的调用树（命令分派、主循环、全部绘图与 flash 代码）整体丢失。

解决：用一个 preScript（`Main.java`，未随本仓库发布）在自动分析前强制造函数：

```java
Address a = toAddr(0x39B4);
disassemble(a);
createFunction(a, "main");
```

函数数 101 → **165**，新解锁的区域与缺口位置完全对应。

## 3. 障碍二：`-msave-restore` 存根

GCC 的 `-msave-restore` 让所有函数用 `jal t0, <存/取桩>` 代替内联的寄存器保存。这些桩（0x100 / 0x108 / 0x116 / 0x130）**共享同一个尾部** 0x12E 的 `c.jr t0`（"返回调用者"），Ghidra 因此把它当成普通函数，输出里满地 `extraout_a0` 之类的伪变量。

解决：用一个 preScript（`Patch.java`，未随本仓库发布）在 `clearListing` 后把 0x12E 与 0x13A 两个"共享尾"改成 `c.jr ra`（`82 80` = `ret`），让存桩就地返回。

> 关键：必须先 `clearListing`，否则 Ghidra 抛 `MemoryAccessException: Memory change conflicts with instruction`。
> 副作用：`extraout_a0` 这类伪变量其实表示"进入函数时的参数寄存器值"，阅读时当成参数即可。

## 4. 障碍三：跳转表

`cmd_dispatch` 里有一处间接跳转：

```asm
c.lui  a4, 0x5
c.slli a5, 2
addi   a4, a4, 0x6D4      # a4 = 0x56D4
c.add  a5, a4
c.lw   a5, 0(a5)
c.jr   a5
```

`0x56D4` 处是 **17 个代码指针**的表（0x354, 0x36E, 0x388, 0x3A2, 0x4FE, 0x3B8, 0x3D6, 0x3FC, 0x420, 0x43A, 0x464, 0x488, 0x4A2, 0x4B8, 0x4CA, 0x4D0, 0x4DC），对应 `0x354–0x50B` 这 440 字节的 17 个 case 体。Ghidra 报 `Could not recover jumptable at 0x00000352. Too many branches`，需要人工确认表位置。

## 5. 核心障碍：WCH 私有压缩指令

### 现象

反编译结果里出现大量浮点指令：`c.fld` 198 条、`c.fldsp` 92 条、`c.fsd` 66 条、`c.fsdsp` 56 条，共 **412 条**。但 CH32X033 **没有 FPU**，GCC 绝无可能发射浮点指令。同时反编译结果出现 `extraout_fa0` 驱动的 switch、`undefined8` 自赋值等荒谬结构。

统计 funct3 分布：只有 `001`/`101`（象限 0）与 sp 形式（象限 2），**从未出现 `011`/`111`**（标准 RVC 的 `c.flw`/`c.fsw`）——这不是偶然，是**槽位挪用**。

### 结论

WCH 青稞内核在"没有 FPU 所以浮点槽位空闲"这一点上做了文章，把标准 RVC 的浮点编码槽改为**字节/半字访存指令**：

| 标准编码 | 实际含义 | 说明 |
|---|---|---|
| 象限 0，funct3=`001`（`c.fld`） | **`lbu rd', off(rs1')`** | 8 位无符号加载 |
| 象限 0，funct3=`101`（`c.fsd`） | **`sb rs2', off(rs1')`** | 8 位存储 |
| 象限 2（`c.fldsp`） | **`lhu rd', off(sp)`** | 16 位加载（`rd` 不是 sp 相对编码） |
| 象限 2（`c.fsdsp`） | **`sh rs2', off(sp)`** | 16 位存储 |
| 象限 0，funct3=`100`（保留槽） | 无操作 | 全镜像仅 3 处（0xAF6、0x1CCC、0x1E02） |

**字段布局（关键，Ghidra 官方实现与真实布局完全不同）**：

```text
象限 0（字节访存）  off = b12 + 2*b5 + 4*b6      （0..7）
象限 2（半字访存）  rd   = x8 + (b4:b2)
                    base = x8 + (b9:b7)
                    off  = 8*(b12:b10) + 2*(b6:b5)   （0..62，偶数）
```

### 判定依据

1. **数据流反证**：`0x84E` 的加载结果紧接着被 `c.add a5,a0` 使用，目标必须是 `a5`（= x8+7），即 `rd = x8 + b2..b4`；Ghidra 原实现按 `b2..b6` 解析，会释放出 `gp`/`sp`/`zero`/`t1`/`t2` —— 这些寄存器在上下文里根本不可能。
2. **偏移奇偶性**：按 Ghidra 原公式会得到 5、7、9、11、13 这类**奇数偏移**，对 16 位访问是不可能的；按上式全部为偶数。
3. **编译器习惯**：`gp-0x7B4`、`gp-0x7AE` 这类地址都能用单条 `addi` 编出来，因此这些指令的偏移必然是 0，隐藏的 +8 偏置不符合编译器行为。
4. **硬件寄存器锚点**：`0x122C` = `lhu a5,0x32(s0)` 后紧跟 `xori a5,a5,0x40` —— 正是 USBFS `UEP4_CTRL_H`（0x40023432）的 TX_LEN/翻转位；`0x4196` = `sh a1,0xc(a0)` —— 正是 `SPI1->DATAR`（0x4001300C）。
5. **规范槽位**：Ghidra 的 `riscv.table.sinc` 里 `cldimm` 公式把字段位移搞反，导致 Q0 偏移呈现 4 倍误差；对照锚点 `0x209A 9CB0` 应为 `c.sb a5,0x1(s1)`。

### 语言补丁

修改 Ghidra 自带的 RISC-V 语言文件（`Ghidra/Processors/RISCV/data/languages/`），然后重编译。以下修改未随本仓库发布，按描述在本地 Ghidra 安装目录进行即可：

```text
riscv.table.sinc  新增/改写字偏移表项：
  clbuimm: uimm is cop1012 & cop0506
           [ uimm = (cop0506 << 1) | (cop1012 >> 2); ]   # = b12 + 2*b5 + 4*b6
riscv.rvc.sinc    把 c.fld/c.fsd 四条构造器替换为 c.lbu / c.sb（使用整数寄存器
                  cr0204s / cr0709s），并加一条 c.wch100 NOP 占位
编译：sleigh.exe riscv.ilp32d.slaspec      （必须在 languages 目录内，需 SLEIGHHOME）
```

**象限 2 的构造器无法实现**：任何带操作数的附加定义（3 位寄存器、固定寄存器、枚举 128 种、`@include`）都会让 `sleigh.exe` **静默 exit 1 且不产出 .sla**；只有语法错误才会给出 exit 2 与行号。最终保留无操作数的标记构造器，把语义交给事后标注：

```text
// 事后标注脚本（未随本仓库发布）：读反汇编清单，按上述公式把标记替换成人读的 lhu/sh
rd = x8 + (b2..b4);  base = x8 + (b7..b9);  off = 8*(b10..b12) + 2*(b5..b6)
```

结果：179 处标记，分布在 33 个函数里（USB 中断 71 处最多），全部人工核对过上下文。

## 6. 导出与二次加工

1. 用 Ls 脚本导出全镜像反汇编清单（`0x0`–`0x5EA0`，9 963 行）；
2. 用 Dump 脚本导出 165 个函数的 C 代码；
3. `q2sites` 标注 → 每个函数头部附上本函数的私有指令清单；
4. `symbols.tsv`：165 个函数按模块命名（crt/lib/cmd/uart/usb/flash/lcd/draw/main/boot/hal/printf）；
5. `regmap.json`：从 CH32 官方头文件的结构体定义计算 858 个外设寄存器偏移，把 `_DAT_ram_40023418` 之类的裸地址换回 `UEP2_DMA` 这样的寄存器名；
6. 生成 12 个模块的 C 源码（保留原始反编译控制流，只做"命名 + 寄存器 + 字符串"三件事，保证结论可追溯）。

## 7. 验证

- **逐函数专项复核**：按 USB、启动链、flash/屏、绘图、HAL、printf 六个主题，逐函数对照反汇编清单，结果见 `reports/` 六份报告（每份 9–14 KB，含缺陷地址与证据）。
- **18 个语义锚点**：每个寄存器语义都由两处独立证据确认。
- **实机验证**：按 `lcd_blit_colour(x,y,w,h,colour)` 的结论改写开机画面，所得镜像烧录后显示正常。

## 8. 残留问题

| 问题 | 影响 | 处理 |
|---|---|---|
| 179 处象限 2 私有半字指令无法解码 | 相关函数的 C 代码里缺读写语句 | 已全部标注，附在函数头注释 |
| `extraout_*` 伪变量 | 阅读障碍 | 理解为"入参寄存器"，报告里有对照表 |
| 跳转表依赖人工确认 | 若表地址猜错会误判 case | 已用 17 个目标地址交叉验证 |
| 结构体布局为推测 | 字段名不可信，偏移可信 | 只按偏移命名，不臆造语义 |

## 9. 工程注意事项

1. 自动分析不会跟随 `mret` 入口，主程序的整棵调用树会缺失；应先确定入口，并在入口处强制建立函数。
2. 出现浮点指令时先确认芯片是否有 FPU；没有 FPU 时按私有扩展处理，不要按标准编码解读。
3. `sleigh.exe` 失败时可能没有任何输出，可用 `.sla` 是否存在及时间戳是否更新来判断。
4. Ghidra 的 `cldimm` 公式对 RVC 字段位移的处理容易误导，偏移量必须用数据流验证。
5. 修改字节前必须 `clearListing`，否则抛 `MemoryAccessException`。
6. 反汇编清单的字节列是内存序，取半字前必须先交换字节再抽字段，否则字段全错。
7. PowerShell 重定向可能丢弃 `sleigh.exe` 的 stderr，可改用 `cmd /c "... > log 2>&1"`。