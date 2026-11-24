# MSU2_MINI 固件 V1.1 反编译报告

目标：`[固件]MSU2_MINI_Firmware_V1.1.hex`（68 156 字节 Intel HEX，对应 flash 0x0000–0x5E9F，共 24 224 字节）。
设备：墨砺工作室（MORI ATELIERS）MSU2 系列可编程 USB 屏幕的 MINI 版本。

本报告配套的**可读源码**在 [`decompiled/`](decompiled/index.md)：165 个函数全部反编译成功（0 失败），按 12 个模块拆成 C 文件，外设寄存器、字符串、全局变量都已换成符号名。

---

## 0. 结论摘要

1. **它其实是一个“USB 转屏幕”的胶水固件**：主机通过 USB 批量端点发 6 字节命令，固件把命令翻译成 ST7735S 窗口/填充/图像操作，图像数据放在外挂的 1 MB SPI NOR 里，按页号寻址。整个固件没有 PLL、没有 RTOS、没有协议栈抽象层，主循环是一个手写的两状态状态机。
2. **能跑，但工程上到处是“能亮就行”的痕迹**：USB 描述符是 WCH 官方示例原封不动的（VID 0x1A86 / PID 0xFE0C，字符串 “wch.cn”“CH32x035”“0123456789”），HID 接口在描述符里声明了却几乎没实现，命令路径上大量没有边界检查、没有超时、没有错误返回，DMA 和 CPU 抢同一块缓冲区。
3. **最要命的四处**（详见 §7）：
   - **`SET_REPORT` 可以让主机一路写出 SRAM**：64 字节一包往固定地址追加，16 位累加器回绕后越界也没人拦（A10）；
   - SPI1 总线被 flash 和 LCD 共用，而 **PA6 一根脚同时是 flash 的 MISO 和 LCD 的 D/C**，要靠每个函数自己重配引脚方向，漏配一次（例如重发一次 `lcd_init`）屏就在 D/C 悬空下收命令（A9 / E4）；
   - 主机可控的页号、长度、地址**全线不校验**，可以让 LCD 流式写入死等（长度 0 → 挂死）、可以让 `w*h` 溢出成永久循环、可以让 flash 擦除在 16 位边界回绕（A1/A4/A8）；
   - 命令队列用“144 字节环 / 6 字节步长”硬编码，而一帧实际是 24 字节，只因 24 = 4×6 才凑巧对齐；生产/消费之间没有互斥，连发 16 包就会覆盖未处理的数据（C1/C2/C10）。
4. 反编译本身也有坑：这颗 WCH RISC-V 核把 RVC 压缩指令里 `c.fld/c.fsd`（象限 0）和 `c.fldsp/c.fsdsp`（象限 2）的编码位挪去做 **字节/半字 load-store** 了，Ghidra 默认会把它解成浮点指令，需要按上述编码修改 RISC-V 语言定义，并由事后标注脚本恢复象限 2 的 179 个点（见 §8.1）。

---

## 1. 目标硬件

| 项 | 值 | 依据 |
|---|---|---|
| MCU | WCH **CH32X033F8P6**，RV32IMAC + 青稞扩展，48 MHz | 固件 `ChipID`/`SystemClk` 打印 + 外设地址表 |
| 内核 flash | 62 KB 可用（标称 64 KB；链接脚本只映射 62 KB），本固件占用 24 224 B（0x0000–0x5E9F） | HEX 解析 + 链接脚本 |
| SRAM | 20 KB @ 0x20000000（`sp` 初始 = 0x20005000） | crt0 @0x3BB4 |
| 片外 flash | **P25D80**，1 MB，256 B/页，共 4096 页 | flash 驱动命令集（0x03/0x0B/0x02/0x20/0x52/0x4B/0x90） |
| 屏 | **ST7735S**，0.96 英寸 160×80，RGB565，SPI | 初始化序列 @0x2778 + 手册 |
| 总线 | SPI1 同时挂 flash 与 LCD：Master / 8 位 / **模式 3** / 软件 NSS / 2 分频 ⇒ **SCK = 24 MHz**；USART2 仅作调试口（115200 8N1，仅发送） | 0x4138（CTLR1≈0x0307）、0x4184（CTLR2=0x03）、0x441E |

**板级连线**的完整表（含 J1 各脚、JP1/JP2/JP3 与物料清单）在另一个仓库维护：[Open_MSU2_MINI/docs/PINOUT.md](https://github.com/hsushjk/Open_MSU2_MINI/blob/main/docs/PINOUT.md)。此处只列与固件行为直接相关的几项，其余引脚不再重复。

| MCU 脚 | 网络名 | 固件中的表现 |
|---|---|---|
| PA6 (1) | SPI_MISO | U2(P25D80).DO 与 J1.4 屏 D/C 共用一脚；读 flash 时配浮空输入、写屏时配推挽输出，方向由各函数自行重配（A9 / E4） |
| PC3 (13) | SPI_CS_LCD | 真正的屏片选，随每笔 LCD 事务翻转 |
| PA9 (10) | PA9 | 板上无负载，固件仍在每次 LCD 事务里翻转 28 次，属历史遗留（B 级现象，不影响功能） |
| PA0 (14) | RESET | `lcd_init` 拉低 10 ms 后释放 |
| PB1 (3) / PC18 / PC19 / PA10 / PA11 / PA4 / PA1 | Touch1 / I2C / USART1 / SPI_CS1 / SPI_CS2 | 固件中零访问。触摸按键实际经 ADC 通道 9 采样（见 §5），不是数字脚 |

电源与背光：USB 5 V 经 U3（XC6206P33，SOT-23 LDO）出 3.3 V，C1/C2 各 10 µF；屏背光 `J1.1 LED+` 经 R1 = 5.1 Ω 直接接 5 V，没有调光管也没有 PWM，与固件中 TIM3 只做 1 kHz tick 的事实一致。U2 的 `/WP`、`/HOLD` 接死 3.3 V，flash 侧没有用 HOLD 做总线隔离。

固件镜像内部分区（反编译时按段验证）：

```text
0x0000  vector table (WCH 向量模式, 每项 4 字节, 未用项指向 0x3BB2 的 `j self`)
0x0240  cmd_dispatch 的 switch 分支体 (0x354..0x50B 由 0x56D4 的 17 项跳转表索引)
0x0A90  UART / TIM3 / DMA 相关
0x1110  USBFS 中断处理 (816 条指令, 全固件最大函数)
0x1A66  SPI flash 驱动
0x2668  LCD 初始化与基本写
0x2C9E  绘图/图像流水线
0x39B4  main
0x3B32  SystemInit, 0x3BB4 crt0
0x3D7C  WCH 风格外设库 (GPIO/DMA/USART/ADC/RCC/PWR)
0x4598  newlib printf 引擎 (0x493E vfprintf / 0x4BCA 补位 / 0x4CD6 输出)
0x5700  ~0x5C38 常量、查找表、8×8 字模
0x5C38  字符串 ("SystemClk:%d"、ChipID、"MSN "、"isn't "、"activated." …)
0x5C74  USB 描述符与字符串描述符
0x5DF0  .data 初值 (176 字节, 由 crt0 拷到 0x20000000)
```

---

## 2. 启动链

`0x0` 是一条 `j 0x3BB4`，`0x3BB4` 是 WCH 的标准 `handle_reset`（编译器产物，与厂家 `startup_ch32x035.S` 一致）：

1. `gp = 0x20000880`（全局指针，后面所有 `gp±N` 都落在这片 RAM 上），`sp = 0x20005000`；
2. `.data` 从 flash `0x5DF0` 拷到 `0x20000000..0x200000AF`（176 B）；
3. `.bss` 清零 `0x200000B0..0x200011AF`（4 352 B），堆从 `0x200011B0` 起，栈顶 0x20005000；
4. `csrw 0xBC0, 0x1F`（CORECFGR：流水线/预测全开）、`csrw 0x804, 3`（INTSYSCR：**中断嵌套 + 硬件压栈**）、`mstatus = 0x88`；
5. `mtvec = 0x7`：WCH 向量模式，中断 n 跳到 `4 + 4n` —— 所以 `0xB8` 是 USBFS（IRQ 45），`0xDC` 是 TIM3（IRQ 54）；
6. `SystemInit()`，然后 **`mepc = 0x39B4; mret`** 进入 `main`。

`mret` 进 main 这一步既开了全局中断（从 MPIE 恢复），也让所有静态分析工具失去控制流：Ghidra 的自动分析根本不会把 `0x39B4` 认成函数，`main` 及其 2.3 KB 调用树全部“消失”。这是本固件逆向时最大的坑（解决办法见 §8.1）。

`SystemInit`（0x3B32）只有三条语句：开内部 48 MHz RC（`RCC->CTLR |= 1`，且不轮询 HSIRDY）、`FLASH->ACTLR = (x & ~3) | 2`（**2 个等待周期**，48 MHz 其实只需要 1 个）、`RCC->CFGR0 &= 0xF8FFFF0F`（清 PLL/HSE/PLLSRC 位，最终写回 0）。**没有 PLL、没有外部晶振、不写 SW/PPRE**，三个时钟域全是 48 MHz HSI（`SystemCoreClock` 常量 0x02DC6C00 存在 flash 0x5E70，crt0 拷到 RAM 0x20000080）。

故障处理：HardFault（0x0C）和 0x10 号向量都是 2 字节的 `j self` 死循环。**任何异常都会让设备静默卡死，没有复位、没有错误码。**

---

## 3. 运行时结构

| 组件 | 配置 | 备注 |
|---|---|---|
| 系统时钟 | 48 MHz HSI，无 PLL | `SystemInit` 只设 2 个 flash 等待周期 |
| TIM3 | 1 kHz 节拍（PSC=47, ARR=999），中断里 `g_tick++` | 唯一用途：数据流态下 **5 秒无新包就 `reset_session()`** 退回命令态（`lhu -0x7b4(gp)` vs 4999，0x7AC–0x7BA） |
| USART2 | 115200 8N1，仅发送（TE），PA2/PA3 复用 | **只给 `printf` 用**：上电打印 `SystemClk`、`ChipID`，之后是调试输出 |
| USBFS | 全速设备，EP0 + EP2 OUT + EP3 IN（EP1 使能不用，EP4 只能发 ZLP，EP5–7 未配） | 见 §4 |
| DMA1 | CH2/CH3 = SPI1 RX/TX，CH8 = USB 包搬进环形队列 | CH8 是内存到内存 |
| ADC1 | 通道 9，TKey 触摸采样 | `adc_init` 置 `CTLR1` bit24（`ADC_TKENABLE`）；`adc_read` 写 `IDATAR1 = 0x70`、`RDATAR = 0x40`，轮询 `STATR` 的 EOC/JEOC 后读 `RDATAR` |

主循环 `main_loop`（0x59A）是一个**两状态状态机**，状态存在 `gp-0x7B2`：

- **状态 0（命令态）**：清节拍 → 把 6 字节接收缓冲 `gp-0x7E0` 拷到应答缓冲 → `usb_ep_send(3, …)` 发回 → 按收到的命令字（`gp-0x7AE`）处理 `0x12/0x13`（ADC，滤波/取平均）、`0x14..0x37`（整屏纯色，颜色 = (cmd-0x14)×100）、`0x38`（整屏 0xEF2，即接近白）；
- **状态 1（数据流态）**：关 USBFS 中断 → 看队列里当前包的“状态字节” `gp+0x76A+idx*2`，为 6 且 64 字节数据块头是 `00 'M' 'S' 'N' 'C' 'N'` 时把状态切到 1；否则用 **DMA1 CH8** 把包内容从 16×64 B 的插槽区搬进 144 字节的 6 字节步长环，然后逐条调用 `cmd_dispatch`；`g_tick > 4999`（5 秒）没有任何新包就 `reset_session()` 回到状态 0。

---

## 4. USB 设备

描述符全部是 WCH 官方示例，**看不出这是 MSU2**：

| 描述符 | 地址 | 内容 |
|---|---|---|
| 设备 | 0x5CE0 | `12 01 10 01 00 00 00 40 86 1A 0C FE 01 00 01 02 03 01` → bcdUSB 1.10，**VID 0x1A86 / PID 0xFE0C**，bcdDevice 1.00 |
| 配置 | 0x5C74 | 0x6B 字节，**3 个接口**，100 mA：IF0 = CDC-ACM（通知 EP 0x81，64 B/1 ms），IF1 = CDC 数据（EP 0x02 OUT / 0x83 IN，各 64 B 批量），IF2 = HID（EP 0x84 IN / 0x04 OUT，64 B 中断） |
| HID 报告 | 0x5CF4 | 用途页 0xFF00，一个 64 B 输入 + 一个 64 B 输出报告 |
| 字符串 | 0x5D18 / 0x5D28 / 0x5D3C | “wch.cn” / “CH32x035” / “0123456789” |

实际数据通路只有 **EP2 OUT（0x02）→ 16×64 B DMA 插槽 → 环形队列 → 命令分派**，应答走 **EP3 IN（0x83）**。CDC 的两条路径与 HID 的 EP4 基本是摆设（详见 §7 与 `decompiled/reports/`）。

端点真实配置（`UEP2_3_MOD = 0x48`、`UEP4_1_MOD = 0x4C`，`UEP567_MOD` 一次都没写）：

| 端点 | 方向 | 配置 | 实际用途 |
|---|---|---|---|
| EP2 | OUT | 双缓冲 | 主机 → 固件的数据通路（RX_LEN 写进 `gp+0x76A+2*producer`，载荷进 `0x20000BE4` 起的 16×64 B） |
| EP3 | IN | 单缓冲 | 6 字节应答（`usb_ep_send(3, …)`） |
| EP1 | IN | 已使能 | **从不装载数据** |
| EP4 | IN / OUT | TX + 双缓冲（IN 侧） | IN 只能发 ZLP，OUT 处理函数不可达 |
| EP5–7 | — | 未配置 | — |

---

## 5. 命令协议（host → device）

一次操作 = 6 字节记录：

```text
p[0] = class        p[1] = sub / opcode
p[2..3] = 16 位大端值 A       p[4..5] = 16 位大端值 B
```

| class | sub | 作用 | 实现 |
|---|---|---|---|
| 0 | 0x30 | 子操作 = `p[2] >> 5`（帧第 3 字节的高 3 位）：0 = 查表，1 = 设平滑系数，4 = 写 LUT 字节（序号 <0x100），5 = 空操作 | 0x250 |
| 0 | 0x4D | 原样回显 | 0x250 |
| 2 | 0,1,2 | 设矩形：x0,y0 / x1,y1 / 前景页+背景页 | 0x250 |
| 2 | 3 | 绘图操作组，`p[2]` = 0..0xB：填充、带背景色块、1-bit 图像、带背景页的 1-bit 图像（`LCD_Photo_wb_MIX`）、颜色图块、纯色流等 | 0x2EA4 / 0x31C0 / 0x367C / 0x36B8 / 0x33B8 / 0x3698 / 0x37D4 / 0x305A / 0x30C6 / 0x3148 / 0x2778 / 0x2A9C |
| 2 | 4 | 装 256 字节图案块 | 0x2E80 |
| 3 | 0..3 | flash：读一页 / 写一页 / 擦除范围 / 写页（第二实现）（24 位地址 = `A<<8 | p[4]`） | 0x1F80 / 0x2202 / 0x250E / 0x2306 |
| 8 | — | 读模拟量：`sub_2C58(p[1])`，结果放在应答的 B | 0x2C58 |

除 `class 2 / sub 3 / op 8,9` 外，每条命令都会把 6 字节原样回传（class 0/3 还会在 A/B 里带上读回的值）。

---

## 6. 显示与存储

- **页寻址**：对外 API 一律以 “页号”（256 B）为单位；固件内部 `page << 8` 变 24 位地址，**不做任何范围检查**（1 MB 只有 0..0xFFF 页）。
- **两种图像格式**：16-bit RGB565 彩色；1-bit 单色（每字节 8 像素，行按字节对齐），单色图可叠加“前景色/背景色”或“背景页”。
- **字模**：8×8 小字模在固件 flash 内（0x5A58–0x5B00）；32×64 ASCII 与 16×16 GB2312 字模由主机写进外部 flash，按页读。
- **窗口/填充**：`lcd_set_window` 发 CASET/RASET/RAMWR（0x2A/0x2B/0x2C）；纯色填充与图像块都用 DMA 喂 SPI1（CH3 TX），1-bit 图像在 CPU 里展开成 16 位色写进**同一块** DMA 缓冲。

---

## 7. 缺陷清单

分五级：**A 会挂死 / B 会造成错误显示 / C 协议与架构 / D 功能缺失与浪费 / E 工程质量**。每条的地址都能在 `decompiled/` 或 `data/listing7.txt` 里直接对上，逐函数的证据在 `reports/` 下对应报告。

同一份清单的机器可读版本在 [data/defects.csv](data/defects.csv)（编号、等级、标题、证据地址、影响、修复建议），便于筛选与跟踪。

### A. 会挂死（主机一帧数据就能做到）

| # | 缺陷 | 证据 |
|---|---|---|
| A1 | **`lcd_stream` 长度不设限且无超时**：长度来自主机 16 位字段，直接写进 DMA CH3 CNTR；`len == 0` 时 DMA 根本不启动，函数却死等 TCIF3 → 永久卡住（PA9/PC3 停在低电平） | 0x30C6，0x30FE 取长度，0x3114–0x311C 轮询 |
| A2 | **`lcd_stream_end` 等一个按定义永远不会被置起的标志**：单独发“结束流”命令就挂死 | 0x3148（0x314C） |
| A3 | **`usart_poll_write` 死等 USART2 TC，无超时**：19 字节的调试字符串只要 UART 没接/被占用，整个固件停在这里；而它被 `printf` 调用 | 0x447E，0x449A `li a1,0x40` / 0x44A4 `beqz` 回跳 |
| A4 | **`lcd_blit_colour` 的 `w*h` 是 32 位无符号乘**：主机给 `w=0xFFFF,h=0xFFFF` 时 `w*h ≥ 2^31`，循环上界变成负数，`i != n` 永不成立 → 永久挂起（不是“什么都不画”） | 0x2A9C，0x2AFC `mul`，0x2B2C `srai` |
| A5 | **`0x2EA4` 是按页从 flash 贴彩色图的函数，不是纯色填充**：上界 `page + (w*h)>>7`，`w*h` 用 32 位乘后右移，主机给 0x100×0x100 就会从起始页连读 512 页（131 072 B）全灌进窗口，`page` 也不掩码 → 越过图像区照读（纯色填充其实是 0x2A9C，见 A4） | 0x2EA4，0x2EE0 `((a2*a3 & 0x7fffffff) >> 7) + a4`，0x2EF8 `flash_fast_read(page<<8,0x100)` |
| A6 | **`lcd_image_with_bg` / `lcd_mono_image` 的页号、索引都不掩码**：`base + index` 越界后照读，读回的是别的图像数据 | 0x367C、0x33B8 |
| A7 | **HardFault 与未定义向量都是 2 字节 `j self`**，全固件没有任何看门狗访问（0x40003000 一次都没出现）→ 任何异常都是静默死机，只能拔电 | 0x0E48 / 0x0E4A |
| A8 | **flash 擦除范围是 16 位算的**：`P+N` 超过 0xFFFF 就回绕，会在页面 0 附近擦掉固件不该碰的区域（范围内对齐本身是对的，问题只在回绕） | 0x250E，掩码在 0x255E/0x256C/0x258A/0x25A4 |
| A9 | **LCD 重新初始化时 D/C 脚悬空**：`lcd_init`(0x2778) 从不配置 PA6（那是 flash 的 MISO），而所有 flash 命令都会把 PA6 抢回去且不还；主机先读一次 flash、再发一次 class2/sub3/opA 的“重初始化”，屏就在 D/C 悬空下收命令 | 0x2778，PA6 归属切换见 0x1BC6/0x1CFC/0x1F9E/0x2512 |
| A10 | **`SET_REPORT` 可以让主机写出 SRAM**：64 字节一包往固定地址 0x2000102C 之后连续追加，累加器是 16 位、会回绕，越界到 0x3FD3 之外也没人管；这是本固件最危险的一条（主机可写任意 RAM 内容） | 0x134A/0x1350（目的地址）、0x1368（累加）、0x1544（入口） |

### B. 会画错

| # | 缺陷 | 证据 |
|---|---|---|
| B1 | **DMA 与 CPU 抢同一块缓冲、无同步**：单色图展开时 CH3 已在 0x3372 使能，CPU 随后在 0x3386 往同一缓冲区继续写像素 | 0x37D4 / 0x36B8，MADDR 0x3368，CNTR 0x336E |
| B2 | **行距两套**：`lcd_image_with_bg` 用 0x140（=320 B=160 px），`lcd_mono_image_mix` 用 0x1E0（=480 B=240 px）。同一模块里混了两个面板宽度 | 0x3460/0x3464/0x346C 对 0x386A/0x3878 |
| B3 | **文本按 240 px 换行**：`x+8 > 0xE8` 才换行（30 列），y 步进 10 px，而屏只有 160 px 宽 → 右边的字直接看不到/被裁到下一行 | 0x2E42 |
| B4 | **`LCD_Photo_wb_MIX` 的背景页参数根本没传**：主循环把“背景色”写进了“背景页”寄存器（`g_bg_page` 0x200000B0 与 `g_page_a` 0x200000B2 混用） | 0x250 case 2/3 |
| B5 | **坐标系不一致**：`lcd_set_window` 统一把 y 加 0x18（24 行），唯独 `lcd_draw_glyph8` 不加 → 图像与文字不在一个坐标系 | 0x2A2A/0x2A30 对 0x2DA2 |
| B6 | **SPI 快读的收/发计数不相等**：`flash_fast_read`(0x2060) 给 CH3 写 `len+5`、给 CH2 写 `len+6`，数据落在 `buf+5`（命令在 `buf[1]`），而上层却从 `buf+6` 取像 → 整体错 1 字节并带一字节脏尾巴；`flash_fast_read_dma`(0x20F8) 更糟：两个通道都按 `len+5` 装，却把 RGB565 **扩写进正在被读取的 RX 缓冲** | 0x20B0 / 0x20B4 / 0x2F84 |
| B7 | **SPI1 跑 24 MHz**：Master / 8 位 / 模式 3（CPOL=1, CPHA=2Edge）/ 软件 NSS / 分频 2，PCLK2=48 MHz ⇒ SCK 24 MHz；ST7735S 的写周期下限约 66 ns（≈15 MHz），已明显超规格；且软件 NSS + 从不置 SSOE，靠 GPIO 手动拉片选 | 0x4138（CTLR1≈0x0307）、0x4184（CTLR2=0x03） |
| B8 | **ST7735S 初始化时序不对**：SLPOUT 后只等 10 ms（手册要 120 ms），DISPON 后不等；`0x36` 在旋转值 ∉ {0,1} 时**不带参数**发出 | 0x2778（0x29BC→0x29C8 跳过了参数写） |

### C. 协议与并发

| # | 缺陷 | 证据 |
|---|---|---|
| C1 | **“6 字节记录”与“24 字节帧”两套语义混用**：`cmd_dispatch` 从 `p[0]` 一路读到 `p[0x14]`，队列却按 6 字节步长推进；只因一帧恰好 24 字节（4 条记录）才凑巧对齐，换一种帧长立刻错位 | 0x250 与 0x59A |
| C2 | **16 位索引被拆成两个字节手工拼**：`(u8[+1]<<8)|u8[+0]`，ISR 与主循环并发读写同一对字节，没有原子性、没有内存屏障 | 0x1110（写）与 0x59A（读） |
| C3 | **USB 描述符是 WCH 官方示例**：VID 0x1A86 / PID 0xFE0C，字符串 “wch.cn”“CH32x035”“0123456789”，声明 CDC-ACM + CDC 数据 + HID 三个接口，实际只用了 EP2 OUT / EP3 IN，HID 报告描述符形同虚设 | 0x5C74 / 0x5CE0 / 0x5CF4 / 0x5D18 |
| C4 | **应答无状态码**：执行成功、参数非法、硬件没响应，回给主机的都是同样的 6 字节回显 | 0x250 尾部 |
| C5 | **流式命令不检查状态**：重复进入 `lcd_stream` 就死锁（见 A1/A2） | 0x250 case 2/3 op 8,9 |
| C6 | **USBFS 中断用“关中断—干活—开中断”当临界区**：`0xE000E184 = 0x2000` / `0xE000E104 = 0x2000` 成对出现在主循环每一圈，靠关中断掩盖 C1/C2 的竞态 | 0x59A |
| C7 | **厂商请求（vendor request）根本不存在，也从不 STALL**：标准请求表只有 GET_STATUS / CLEAR_FEATURE / STALL / SET_FEATURE / SET_ADDRESS / GET_DESCRIPTOR / GET_CONFIGURATION / SET_CONFIGURATION / GET_INTERFACE / SET_INTERFACE，其余一律“静默接受”，主机看到的是成功 | 0x1566 的请求分派表 |
| C8 | **vendor IN 从没初始化过的“光标”读数据**：`gp-0x780`（0x20000100）从未被写过，于是每次 vendor IN 都从 flash 0x0 起按 64 字节往后走，主机拿到的是固件镜像本身 | 0x1660 → 0x1566 |
| C9 | **长度表与索引错位**：ISR 把长度写在 `gp+0x76A+2*P`（P ≤ 15），而 `usb_ep_flags_clear`(0xC3A) 把 0x772 起当成另一张 16 项表；消费者清的是 `0x772+2*C`，等于在清 **C+4 号槽**的长度 | 0x13A6–0x13B2 对 0x9A6–0x9AA |
| C10 | **生产/消费完全没有互斥**：唯一的背压是“计数 > 13 时回 ACK 并置一个没人读的标志”，主机连发 >16 包就会覆盖还没处理的槽 | 0x1426–0x144A |
| C11 | **字符串 0（LANGID）从没写过的 RAM 0x200000A8 读**，长度也是 0 → 主机拿到的语言 ID 描述符是空的 | 0x5D18 之前的分派 |
| C12 | **EP 0x81 被使能却从不装载**、**EP 0x84 只能发 ZLP**（TX_LEN 只在 0x1472 被写成 0）、**EP4 OUT 处理 0x1454 是不可达的死代码**、EP5–7 完全没配 | UEP4_1_MOD=0x4C / UEP2_3_MOD=0x48 |
| C13 | **多包 OUT 传输会被提前收尾**：0x117E 对所有 class/vendor 数据阶段把 `wLength` 清零，第二包起就发 status IN | 0x117E |

### D. 功能缺失与浪费

| # | 缺陷 | 证据 |
|---|---|---|
| D1 | **CDC↔UART 通路是死代码**：`uart_dma_rx_setup(0xCC8)` 从不写 CNTR（两个通道计数恒为 0），USB 一包数据一个字节也不会转发到 UART；而这段代码仍挂在 USB 中断里每包执行一次 | 0x0CC8、0x0D74 |
| D2 | **newlib 全量 printf 编进固件**（37 个函数、近 4 KB），只用于上电那两行调试输出；`%f/%e/%g` 消费参数后**什么都不打印** | 0x4598–0x56C4，格式分派 0x4D1A |
| D3 | **未知格式符原样回显**、尾部单个 `%` 会继续扫过字符串结尾（越界读）；宽度/精度不设上限（可要求 2e9 字节的补位循环） | 0x4D0E / 0x4B46 / 0x4AE4 |
| D4 | **授权校验形同虚设**：16 组字节的异或+乘法比较，失败就把整块数据 XOR 0xA5 抹掉；没有签名、没有密钥、没有绑定芯片 UID | 0x1A86 |
| D5 | **`SystemCoreClockUpdate`(0x3B74) 在 main 里调、SystemInit 里不调**，顺序与 WCH 模板相反；`RCC_GetClocksFreq`(0x4064) 不看 PPRE，直接把 48 MHz 抄给三个时钟域 | 0x3B32 / 0x3B74 / 0x4064 |
| D6 | **只用内部 48 MHz RC**：不启 PLL、不启 HSE、不写 SW/PPRE，USB 全速靠 RC 的出厂校准硬撑；flash 给 2 个等待周期（48 MHz 只需要 1 个） | 0x3B32 三条语句 |

### E. 工程质量

| # | 缺陷 | 证据 |
|---|---|---|
| E1 | 全局变量全部是 `gp±N` 手工偏移，没有结构体、没有大小检查、没有命名，类型靠约定 | 全固件 |
| E2 | 同一个功能有多个副本：`spi1_gpio_init` 0x1BC6 与 `spi1_gpio_init2` 0x1CFC、`flash_page_program` 0x2202 与 `flash_page_program2` 0x2306、`lcd_mono_image_a/b` 0x367C/0x3698 | 见符号表 |
| E3 | 魔法数到处硬编码：999（延时）、4999（超时）、0xEF2（颜色）、0x1C200（波特率）、0x18（行偏移），没有一处有注释或宏 | 见 `decompiled/` |
| E4 | `lcd_write_cmd`/`lcd_write_data8` 每次都要重新配 CS/DC 引脚方向（因为 PA6 一脚两用），是 B/A 级缺陷的根因 | 0x2668–0x2714 |

> 结论：这不是“写错了几行”的固件，而是**没有一层抽象**——引脚、总线、缓冲、协议全部在调用点手工处理，任何一处漏配都直接把错误暴露到硬件上。它的功能集（USB 命令 → 屏 + flash）本身很简单，重写时用 HAL 分层 + 一套参数校验 + 一个超时机制，代码量可以减半、稳定性完全不是一个量级。

### 修复优先级（如果要改这版固件）

1. **先堵住 A10**（SET_REPORT 目标地址加 `min(acc, 0x400)` 或直接改成环形缓冲）——这是唯一一条能让人从主机端改写固件的缺陷。
2. **给所有等待加超时**：`usart_poll_write`、`lcd_stream`、`lcd_stream_end`、`flash_wait_busy` 统一用一个 `wait_flag(fn, mask, timeout_us)`，超时返回错误码而不是死等（A1–A3）。
3. **所有主机参数入口做校验**：页号 `< 0x1000`、`w*h <= 0xA0*0x50`、`len > 0`、颜色数按格式算；校验失败回一个真正带错误码的应答（A4/A6/A8、C4）。
4. **把 PA6 的归属收进一个 `bus_select(FLASH|LCD)` 里**，每次切换前后都配一次，`lcd_init` 也必须先 `bus_select(LCD)`（A9）。
5. **队列改成“长度 + 偏移”两张表由同一索引驱动**，生产/消费各一个 u16 原子读改写，或者在临界区里做（C1/C2/C10）。
6. **描述符换成自己的 VID/PID、删掉没实现的 CDC/HID 接口**，主机侧就不用再猜（C3）。
7. 显示几何统一成一份 `panel_cfg { width, height, x_off, y_off, stride }`，顺手修掉 B2/B3/B5（行距、换行宽度、y 偏移）。

---

## 8. 复现方法

### 8.1 让 Ghidra 认识这颗核

三个障碍：

1. **`mret` 进 main**：自动分析顺着 `j 0x3BB4` 走，到 `mepc = 0x39B4; mret` 就断了，`main` 及其 2.3 KB 调用树全部没被识别。解决：用一个 preScript（`Main.java`，未随本仓库发布）先 `disassemble(toAddr(0x39B4))` 再 `createFunction(a,"main")`。同类问题还有 `0x240..0xB33` 那段 `switch` 分支体——它由 0x56D4 的 17 项跳转表索引，没被 `main` 调用前是空白，识别 main 后自动补齐。
2. **`-msave-restore` 的存取桩**：`0x100/0x108/0x116/0x130` 是 `__riscv_save_N`，末尾是 `c.jr t0`，Ghidra 把它当“未知跳转 → 当成 call”，导致每个函数都多出一堆 `extraout_a4/a5` 伪变量。解决：用 preScript（`Patch.java`，未随本仓库发布）把那几条 `c.jr t0` 改成 `ret`（必须先 `clearListing` 才能改字节）。注意 `extraout_*` 其实就是“进入函数时还活着的参数寄存器”，读 C 的时候要按这个理解，不要当成新变量。
3. **WCH 青稞的自定义 RVC 编码**（最麻烦）。它把 RVC 里浮点 `c.fld/c.fsd`（象限 0）和 `c.fldsp/c.fsdsp`（象限 2）的编码位挪用成**字节/半字 load-store**：
   - 象限 0（0x6000/0xA000 掩码）：`lbu rd', off(rs1')` / `sb`，`rd' = x8+(b4:b2)`，`rs1' = x8+(b9:b7)`，`off = b12 + 2*b5 + 4*b6`（0..7）；
   - 象限 2（0x2000/0xA000 掩码，即 `c.fldsp/c.fsdsp`）：**16 位** `lhu/sh`，`rd' = x8+(b4:b2)`，`rs1' = x8+(b9:b7)`，`off = 8*(b12:b10) + 2*(b6:b5)`（偶数，0..62），位 5..6 恒为 `0b01`。
   Ghidra 的 RISC-V 语言定义按上述编码修改（`riscv.table.sinc` 加 `clbuimm` 表、`riscv.rvc.sinc` 用 `c.lbu/c.sb` 替换 `c.fld/c.fsd`），再用 `sleigh.exe riscv.ilp32d.slaspec` 重新编译语言；补丁未随本仓库发布，做法即本节描述。象限 2 的槽位在 Sleigh 12.2 里**无法**用一个新构造器表达（任何附加定义都让 `sleigh` 静默退出 1），所以那 179 个点保留成无操作数标记指令，由事后标注脚本按上面的公式还原，并在生成源码时作为每个函数的头部注释列出来。

### 8.2 工具链

```text
data/listing7.txt       全镜像反汇编（9 963 行，ram:ADDR 字节 助记符），唯一权威数据源
data/q2sites.txt        179 个象限 2 自定义指令的真实语义
data/symbols.tsv        地址 → 符号名 / 模块
data/regmap.json        858 个外设寄存器名与地址
data/calls.tsv          调用关系（caller → callee）
decompiled/             带符号名与模块拆分的源码（由生成脚本产出）
reports/                六个方向的逐函数验证报告（缺陷与证据）
```

生成流程分四步：先用符号表把地址映射成名字；再把 `FUN_ram_XXXX`、`_DAT_ram_XXXXXXXX`、`&DAT_ram_00005xxx` 全部换成符号、寄存器名与字符串名，并按模块拆文件；寄存器偏移由 WCH 头文件的结构体定义计算；象限 2 指令按 §8.1 的公式还原成 `lhu`/`sh` 并写入每个函数的头部注释。上述脚本均未随本仓库发布。

### 8.3 已知的残留问题

- `extraout_*` 需要读者自己按调用约定还原（见 8.1 第 2 条）。
- `0x4FD6`（`__swrite`）与 `0x504E` 只有两字节 `c.j` 的尾部跳转，目标操作数在这个镜像里无法解码，只能按整条 syscall 链推断。
- 图像/字模数据在片外 flash 里，固件内没有它的副本；§7 的 B6（+5 字节相位）需要一份真正的 flash 导出才能最终确认。
- ST7735S 的时序上限按手册记忆值（≈15.15 MHz、SLPOUT 120 ms）判断，未经在线复核。

---

## 9. 交付物

| 路径 | 内容 |
|---|---|
| [`decompiled/index.md`](decompiled/index.md) | 12 个模块、165 个函数的地址/名字/模块/注释总表 |
| `decompiled/*.c` | 反编译源码：`crt.c boot.c main.c cmd.c uart.c usb.c flash.c lcd.c draw.c hal.c printf.c lib.c` |
| `decompiled/registers.h` | 858 个外设寄存器符号（从 `ch32x035.h` 结构体算出来的偏移） |
| `decompiled/strings.h` | 14 个字符串 + 8 个数据表（USB 描述符、字模表） |
| [`reports/g1_usb.md`](reports/g1_usb.md) | USBFS 中断、端点、12 类标准请求、SET_REPORT 越界 |
| [`reports/g2_plumbing.md`](reports/g2_plumbing.md) | 启动链、时钟、TIM3、USART2、DMA、存取桩 |
| [`reports/g3_flash_lcd.md`](reports/g3_flash_lcd.md) | SPI1 配置、P25D80 命令集、ST7735S 初始化、擦除规划 |
| [`reports/g4_drawing.md`](reports/g4_drawing.md) | 窗口/RAMWR、颜色与单色两条通路、几何与挂死条件 |
| [`reports/g5_hal.md`](reports/g5_hal.md) | GPIO/DMA/USART/SPI/TIM 访问器与 WCH 库行为 |
| [`reports/g6_printf.md`](reports/g6_printf.md) | newlib printf 全链、格式符行为、syscall 桩 |
| [`data/defects.csv`](data/defects.csv) | 缺陷清单的机器可读版本：编号、等级、证据地址、影响、修复建议 |
| [`data/symbols.tsv`](data/symbols.tsv) | 165 个函数的地址、符号名、模块、备注 |
| [`data/regmap.json`](data/regmap.json) | 858 个外设寄存器名与地址 |
| `data/q2sites.txt` | 179 处象限 2 自定义指令的语义还原结果 |
| `data/listing7.txt` | 全镜像反汇编（唯一权威数据源，686 888 B） |
| [`docs/METHOD.md`](docs/METHOD.md) | 方法学：私有指令补丁、可达性修复、验证手段、复现步骤（脚本未随仓库发布） |
| [`docs/FINGERPRINT.md`](docs/FINGERPRINT.md) | 固件指纹、分区校验值与自验方法 |
| [`docs/diagrams/`](docs/diagrams/) | 启动链、状态机、命令分派、USB 数据通路四张结构图 |
| [`NOTICE.md`](NOTICE.md) | 第三方材料与商标说明 |

模块规模（函数数）：hal 43、printf 37、flash 22、draw 17、usb 9、boot 9、lcd 8、lib 8、uart 7、cmd 3、main 1、crt 1。

**建议阅读顺序**：`main.c` → `boot.c`（启动/时钟）→ `usb.c`（中断与端点）→ `cmd.c`（协议分派）→ `flash.c` + `lcd.c` + `draw.c`（真正干活的部分）→ `hal.c`（WCH 库）。每个函数头部都标了原始地址、调用者，以及它用到的象限 2 自定义指令的真实语义。

## 附：触摸通道 9 的采样方式

`adc_init`（0x2BCC）在末尾置 `ADC1->CTLR1` 的 bit 24（`ADC_TKENABLE`）；`adc_read`（0x2C58）写 `ADC1->IDATAR1 = 0x70`、`ADC1->RDATAR = 0x40`，再轮询 `ADC1->STATR` 的 EOC/JEOC，返回 `RDATAR` 的低 16 位。

触摸电极接 PB1（ADC 通道 9）。`cmd.c` 中 0x12、0x13 分支与 0x14–0x37 纯色分支都调用 `adc_read(9)`：0x13 分支对读数取平均后减 0x7D，0x14–0x37 与 0x38 分支把新读数与 `g_adc_value` 比较后更新 `gp-0x7C4` 处的标志。
