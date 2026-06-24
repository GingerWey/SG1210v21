# SG1210 银网科技 — 固件项目

SG1210 智能电力仪表固件，基于 STM32F407 + FreeRTOS + emWin。

## 技术栈

### 嵌入式目标 (MCU)
- **MCU**: STM32F407VGTx (Cortex-M4F, 168MHz, 1MB Flash, 192KB SRAM)
- **RTOS**: FreeRTOS Kernel V10.3.1 (via CMSIS-RTOS v2 wrapper)
- **HAL**: STM32F4xx HAL Driver (STMicroelectronics)
- **GUI**: emWin V6.56 (SEGGER)
- **图像编解码**: CSG (Compact Scalable Graphic) — 自研格式，位于 Middlewares/emWin/csgimage/
- **编译器**: ARM Compiler V6.22 (ARMCLANG)
- **IDE**: Keil MDK (uVision)
- **编程语言**: C++17 / C11

### 模拟器 (Windows)
- **IDE**: Visual Studio 2022 (v145 toolset)
- **平台**: Win32 (Debug/Release)
- **编程语言**: C++20 / C17
- **GUI模拟**: emWin Simulation Library (GUISim.lib)

## 项目结构

```
SG1210v21/
├── Application/          # 应用层 — 入口、任务、GUI、通信、设备逻辑
│   ├── main.cpp          # 入口：HAL_Init → Clock → osKernel → Tasks_Init → osKernelStart
│   ├── COM/              # 通信栈：UART Socket、Modbus RTU从站、协议层、环形缓冲
│   ├── Config/           # 设备配置宏、FreeRTOSConfig.h、OS端口配置
│   ├── Device/           # 设备逻辑：线圈控制、DFT(64/80点)、数值处理、采样、录波
│   ├── GUI/              # GUI 表单系统：主界面、菜单、启动画面、仪表盘、事件浏览器等
│   │   └── Graphics/     # 自研 x128 图形驱动
│   ├── Mics/             # 杂项：AES128、CRC16/32、中值平均滤波、RAM堆分配、随机数
│   ├── System/           # 核心系统：设备类型、寄存器、缓冲区、时钟、调试、事件管理、多语言字符串
│   └── Tasks/            # RTOS 任务：AppTask、CtrlTask、HMITask、UARTTask
├── Drivers/              # 驱动层
│   ├── CMSIS/            # CMSIS 5：core_cm4、STM32F4xx 设备头文件、启动代码
│   ├── Cube/             # STM32CubeMX 生成代码：ADC/CRC/DMA/FSMC/GPIO/IWDG/RTC/SPI/TIM/USART
│   ├── HAL_Driver/       # STM32F4 HAL 库头文件(Inc/)和源文件(Src_/)
│   └── Peripherals/      # 外设驱动：ADC管理器、板控、DIO、Flash接口、I2C、INA226、LED、
│                          键盘、MB85RS FRAM、MCP401x、NvRAM、SD3077 RTC、SPI通道、UART通道
├── Middlewares/          # 中间件
│   ├── FreeRTOS/         # FreeRTOS 内核 + CMSIS-RTOS v2 适配层 + CM4F 移植
│   └── emWin/            # emWin GUI 库 + CSG图像编解码器 + 字体 + 用户配置
│       ├── csgimage/     # CSG编解码：BitPacker、CSGCodec、CSGDecoder、CSGCommon
│       │   └── Compress/ # 压缩算法：RLE、Huffman、MiniLZ77
│       ├── fonts/        # 字体：ASCII(6/16/24)、中文(16/24)、Digital(32/40/44)、Monaco16
│       └── user/         # emWin集成：GUIConf、LCDConf、ST7789S驱动、位图资源
├── Project/              # Keil MDK 工程
│   ├── SG1210_H20v21.uvprojx   # Keil 工程文件
│   ├── SG1210_H20v21.uvoptx    # Keil 选项
│   ├── DebugConfig/            # 调试配置
│   └── Objects/                # 编译产物 (.o, .d, .map, .axf, .sct)
├── Sim/                  # Visual Studio 模拟器工程
│   ├── SG1210Sim.sln           # VS 解决方案
│   ├── SimulationTrial.vcxproj # VS 项目 (Win32, C++20)
│   ├── Simulation/             # 模拟器入口：WinMain.cpp、GUIMain.cpp、GUISim.lib
│   ├── Config/                 # 模拟器 GUIConf、LCDConf、SIMConf
│   ├── GUI/Include/            # 模拟器版 emWin 头文件
│   └── Tool/                   # UI设计 PSD 源文件
└── Doc/                  # 设计文档
    └── SG1210v21技术参考.md    # 完整目录树
```

## 常用命令

### MCU 编译
- 在 Keil MDK 中打开 `Project/SG1210_H20v21.uvprojx`
- Target 选择 `APP`，点击 Build (F7)
- 输出：`Project/Objects/SG1210_H21v{version}.axf`

### 模拟器编译
- Visual Studio 2026安装在 `d:/apps/vs2026`
- 在 Visual Studio 中打开 `Sim/SG1210Sim.sln`
- 配置选择 Debug | Win32，点击生成
- 输出：`Sim/Build/SG1210D.exe`
- 运行 `Sim/CleanUp.bat` 清理编译产物

## 编码规范

### 命名风格
- **类型/结构体/枚举**: PascalCase — `TUARTConfig`、`TGUIState`、`GWinForm`
- **公共函数**: PascalCase — `Tasks_Init()`、`BoardCtrl_Init()`、`CreateHMITask()`
- **文件内部函数**: camelCase 前缀 `_` — `_updatePage1()`、`_showForm()`、`_OnTick()`
- **变量**: 匈牙利前缀 — `uTick` (uint32_t)、`uwKey` (uint16_t)、`ucFlag` (uint8_t)、`pBuf` (pointer)、`rValue` (float)
- **宏/常量**: MACRO_CASE 或 PascalCase — `DEV_TYPES_H`、`STATE_TRUE`、`NUM_ADC_CHANNELS`

### 头文件保护
```c
#ifndef DEV_TYPES_H
#define DEV_TYPES_H
// ...
#endif
```

### 文件头注释
```cpp
//-----------------------------------------------------------------------------
/*
 File        : DevTypes.h
 Version     : V1.10
 By          : 银网科技

 Description : 描述

 Date       : 2023.12.05
*/
//-----------------------------------------------------------------------------
```

### 注释语言
- **Application/ 和 Drivers/Peripherals/**: 中文注释（简体中文）
- **Middlewares/ (FreeRTOS/emWin/CSG)**: 英文注释
- **统一使用 UTF-8 编码**

### 缩进与格式
- 使用空格缩进（不用 Tab），2空格缩进
- 函数大括号独占一行
- 分隔线：`//-----------------------------------------------------------------------------` (77个短横线)

### extern "C" 模式
所有需要 C 链接的头文件必须包裹：
```c
#ifdef __cplusplus
extern "C" {
#endif
// ...
#ifdef __cplusplus
}
#endif
```

### 变体文件 (`_` 后缀)
部分文件存在 `_` 后缀变体，用于不同硬件版本：
- `CoilCtrl.cpp` / `CoilCtrl_.cpp` — 不同线圈控制逻辑
- `GPMainForm.cpp` / `GPMainForm_.cpp` — 不同主界面渲染
- `RamHeap.cpp` / `RamHeap_.cpp` — 不同内存分配策略
- 在 Keil 工程中通过排除编译开关选择具体变体

## 核心架构

### RTOS 任务 (4个任务)
| 任务 | 入口函数 | 优先级 | 堆栈 | 职责 |
|------|----------|--------|------|------|
| appTask | TaskApp | Normal | 2048B | 主调度：启动设备、创建其他任务、周期计算、LED/WDT |
| HMITask | TaskHMI | Normal | 8192B | GUI：初始化 emWin、运行消息循环 |
| CTRLTask | TaskCtrl | High | 8192B | 控制：线圈逻辑、继电器扫描、板卡控制 |
| UARTTask | TaskUart | — | — | 串口通信处理 |

### 看门狗
协作式窗口看门狗：各任务在每个循环置位 `RSTSrc` 对应位，TIM9 1ms 中断检查所有任务位在时间窗口内是否被置位后才喂狗。

### GUI 架构 (emWin)
- 自研表单系统 `GWinForm`：每个界面=4个函数指针(Init/Show/Close/MsgProc)
- `FormManager` 单例管理表单注册、切换、消息分发
- 窗口ID体系：`WID_SplashForm(100)`、`WID_MenuForm(101)`、`WID_MainForm(102)` 等
- 自定义消息系统 `GM_MESSAGE` (MsgId + Param + Data联合体)
- 显示驱动：ST7789S via FSMC

### 设备寄存器模型
- 所有设备状态存储在编号"寄存器"中（类似 Modbus 寄存器模型）
- `TDevRegInfoItem` 描述每个寄存器的元数据（名称、范围、比例、单位、事件、属性）
- `TDevRegListClass` 枚举寄存器分组（保护、测量、事件、配置等）
- 访问接口：`_GetRealReg()`、`_SetRealReg()`、`GetHWFault()`、`SetRSTSrc()` 等

### 通信栈
- `TCOMSocket` 抽象基类 → `SocketUART` 实现
- `TCOMProtocol` → `proModbusRTUSlave` / `EchoSvr` / `InspSvr`
- `COMRingBuf` / `COMBuffer` 环形缓冲队列
- `COMDevIntf` 桥接通信层与设备寄存器系统

### 平台适配
- MCU 与模拟器通过 `#ifdef __vmSIMULATOR__` 分支
- 模拟器：`<windows.h>` + Win32 API
- MCU：STM32 HAL 外设头文件 (`gpio.h`、`rtc.h` 等)

## 注意事项

- 允许在 `D:\Works\SilverGrid\SG1210\Firmware\SG1210v21` 及其子目录中读写文件
- 允许在 `D:\Works\SilverGrid\SG1210\Firmware\SG1210v21` 及其子目录中执行 Bash 命令
- MCU 固件面向 ARM Compiler V6 编译
- 模拟器面向 MSVC (Visual Studio 2022) 编译
- 中文注释主要在 Application 和 Drivers/Peripherals 层
- CSG 图像编解码器代码遵循 Google C++ 规范（英文注释）
