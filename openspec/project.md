# Project Context

## Project Overview
- **Project Name**: 1T相变双拼蒸汽发生器控制系统
- **Platform**: STM32F103VC (Cortex-M3)
- **Language**: C (Keil MDK-ARM)
- **Repository**: https://github.com/wmstianya/1T-Standalone-Phase-Change-Two-Piece-Project-v4.72.git

## Tech Stack
| Component | Technology |
|-----------|------------|
| MCU | STM32F103VC |
| IDE | Keil MDK-ARM v5 |
| Communication | Modbus RTU (USART2/3/4), Debug (UART5) |
| Peripherals | ADC, PWM, GPIO, RTC, Internal Flash, External Flash |

## Directory Structure
```
UART/
├── CORE/              # Cortex-M3 核心启动文件
├── HARDWARE/          # 外设驱动
│   ├── ADC/           # ADC采样
│   ├── Parallel_Serial/ # 并转串IO扩展
│   ├── PWM/           # PWM输出
│   ├── relays/        # 继电器控制
│   ├── USART2/        # LCD通信 (Modbus)
│   ├── USART3/        # 主从机通信 (Modbus)
│   ├── USART4/        # 从机通信 (Modbus)
│   ├── USART5/        # 调试输出
│   └── USR_C210/      # WiFi模块
├── SYSTEM/            # 系统功能模块
│   ├── system_control/ # ⚠️ 核心控制逻辑 (需重构)
│   ├── delay/         # 延时函数
│   ├── rtc/           # 实时时钟
│   ├── in_flash/      # 内部Flash存储
│   └── out_flash/     # 外部Flash存储
├── USER/              # 用户应用
│   ├── main.c         # 主函数
│   └── stm32f10x_it.c # 中断服务
└── tools/             # 调试工具
```

## Coding Conventions
1. **Naming**: camelCase for variables/functions, ALL_CAPS for constants
2. **No numeric suffixes**: Use semantic names (e.g., `uartMainInit` not `uart1Init`)
3. **Function size**: Max 20 lines per function
4. **Comments**: Detailed comments for complex logic
5. **No magic numbers**: Use `#define` or `const`
6. **Global variables**: Must be `static` unless cross-file access required

## Current Pain Points
1. `system_control.c` is **4694 lines** with **52 functions** - severe SRP violation
2. Functions are too long, some exceed 200+ lines
3. Mixed concerns: ignition, water control, pressure, errors all in one file
4. Hard to maintain and debug
5. No unit testability

## Refactoring Goals
- Split `system_control.c` into cohesive, single-responsibility modules
- Each module < 500 lines
- Each function < 20 lines
- Clear interfaces between modules
- Improved testability

