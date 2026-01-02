# 主从机联动逻辑分析

## 📋 概述

本文档分析锅炉控制系统的主从机联动机制，包括通信协议、功率分配、负载均衡和故障处理。

---

## 🏗️ 系统架构

```
┌──────────────────────────────────────────────────────────────────┐
│                         LCD 触摸屏                               │
│                    (USART2 - Modbus RTU)                         │
└──────────────────────────────────────────────────────────────────┘
                                │
                                ▼
┌──────────────────────────────────────────────────────────────────┐
│                      主机 (Master)                               │
│                  Address_Number = 0                              │
│  ┌────────────────────────────────────────────────────────────┐ │
│  │  核心数据结构:                                              │ │
│  │  - LCD10D: 本机LCD数据                                      │ │
│  │  - LCD10JZ[1-10]: 从机数据镜像                              │ │
│  │  - SlaveG[1-10]: 从机控制参数                               │ │
│  │  - UnionD: 联动控制状态                                     │ │
│  └────────────────────────────────────────────────────────────┘ │
│                                                                  │
│  ┌─────────┐  ┌─────────┐  ┌─────────┐  ┌─────────┐            │
│  │ USART2  │  │ USART3  │  │ USART4  │  │ USART5  │            │
│  │ LCD通信 │  │ 从机通信│  │本地通信 │  │ 调试口  │            │
│  └────┬────┘  └────┬────┘  └────┬────┘  └────┬────┘            │
└───────┼────────────┼────────────┼────────────┼───────────────────┘
        │            │            │            │
        │            ▼            │            │
        │   ┌────────────────┐    │            │
        │   │ RS485 从机总线 │    │            │
        │   └───┬───┬───┬───┘    │            │
        │       │   │   │        │            │
        ▼       ▼   ▼   ▼        ▼            ▼
      LCD    从机1 从机2 从机n  本地控制    调试输出
           (1-10号)
```

---

## 📊 核心数据结构

### 1. 从机控制结构 `SLAVE_GG` (SlaveG[1-10])

```c
typedef struct _SLAVE_G {
    /* 通信状态 */
    uint8 Alive_Flag;           // 在线状态 (1=在线, 0=离线)
    uint8 Ok_Flag;              // 无故障标志
    uint16 Send_Flag;           // 发送计数 (>30则判定离线)
    uint8 UnBack_time;          // 未回复时间

    /* 权限控制 */
    uint8 Key_Power;            // 使能权限 (1=允许运行, 0=禁止)

    /* 功率分配时间统计 */
    uint32 Big_time;            // 大需求时间计数 (满功率运行时长)
    uint32 Small_time;          // 小需求时间计数 (低功率运行时长)
    uint32 Zero_time;           // 零功率时间计数 (停机时长)
    uint32 Work_Time;           // 累计工作时间

    /* 命令标志 */
    uint8 Startclose_Sendflag;  // 启停命令发送标志 (3=待发送)
    uint8 Startclose_Data;      // 启停命令数据 (1=启动, 0=停止)

    /* 参数配置 */
    uint8 Fire_Power;           // 点火功率
    uint8 Max_Power;            // 最大功率
    uint8 Smoke_Protect;        // 排烟温度保护值
    uint16 Inside_WenDu_ProtectValue; // 炉内温度保护值

    /* 手动控制标志 */
    uint8 AirOpen_Flag;         // 风机开关命令
    uint8 PumpOpen_Flag;        // 水泵开关命令
    uint8 PaiWuOpen_Flag;       // 排污阀开关命令
    uint8 LianxuFa_Flag;        // 连续排污阀命令
    // ...
} SLAVE_GG;
```

### 2. 从机数据镜像 `LCD10_JZ_Struct` (LCD10JZ[1-10])

```c
typedef struct _LCDJZ_UNIS {
    uint16 Device_State;        // 设备状态: 0=离线, 1=待机, 2=运行, 3=手动
    uint16 Error_Code;          // 故障码
    uint16 Air_Power;           // 当前功率 (%)
    uint16 Flame_State;         // 火焰状态
    uint16 Water_State;         // 水位状态
    uint16 Air_Speed;           // 风速
    uint16 Max_Work_Power;      // 最大工作功率
    uint16 YunXu_Flag;          // 运行允许标志
    uint16 Work_Time;           // 累计运行时间
    float Steam_Pressure;       // 蒸汽压力
    float Smoke_WenDu;          // 排烟温度
    // ...
} LCD_JZ_GG;
```

### 3. 联动控制结构 `UNION_GG` (UnionD)

```c
typedef struct _UNION_FLAGS {
    uint8 UnionStartFlag;       // 联动启动标志: 0=关闭, 1/2=启动, 3=特殊
    uint8 Need_Numbers;         // 需要运行的机组数量
    uint8 Zhu_Need_Flag;        // 主机需要增载标志
    uint8 Cong_Need_Flag;       // 从机需要增载标志
    uint8 Zhu_Less_Flag;        // 主机需要减载标志
    uint8 Cong_Less_Flag;       // 从机需要减载标志
} UNION_FLAGS;
```

---

## 🔄 通信流程

### 主机轮询从机 (USART3)

```
┌─────────────────────────────────────────────────────────────┐
│                    轮询周期 (~100ms)                         │
├─────────────────────────────────────────────────────────────┤
│  1. 发送读取命令: ModBus3_RTU_Read03(address, 100, 18)      │
│     - 读取从机寄存器 100-117 (18个寄存器)                    │
│                                                             │
│  2. 从机响应数据解析:                                        │
│     RX_Data[4]  = Device_State  (设备状态)                  │
│     RX_Data[16] = Error_Code    (故障码)                    │
│     RX_Data[18] = Flame_State   (火焰状态)                  │
│     RX_Data[24] = Air_Power     (当前功率)                  │
│     ...                                                     │
│                                                             │
│  3. 更新 LCD10JZ[address] 数据镜像                          │
│                                                             │
│  4. 检查是否有命令待发送:                                    │
│     - Startclose_Sendflag > 0 → 发送启停命令                │
│     - AirPower_Flag > 0       → 发送功率调整命令            │
│     - ...                                                   │
└─────────────────────────────────────────────────────────────┘
```

### 离线检测机制

```c
// 每100ms检查一次
for(i = 1; i < 13; i++) {
    if(SlaveG[i].Send_Flag > 30) {  // 超过30次(3秒)未响应
        SlaveG[i].Alive_Flag = FALSE;
        LCD10JZ[i].DLCD.Device_State = 0;
        memset(&LCD10JZ[i].Datas, 0, sizeof(LCD10JZ[i].Datas));
    }
}
```

---

## ⚡ 功率分配逻辑

### 核心函数: `Union_MuxJiZu_Control_Function()`

#### 1. 统计在线机组

```c
for(Address = 1; Address <= 10; Address++) {
    if(LCD10JZ[Address].DLCD.YunXu_Flag &&     // 有运行权限
       SlaveG[Address].Alive_Flag &&           // 在线
       LCD10JZ[Address].DLCD.Error_Code == 0)  // 无故障
    {
        AliveOk_Numbres++;  // 可用机组数++
        
        // 找出待机机组中运行时间最短的 (下次优先启动)
        if(LCD10JZ[Address].DLCD.Device_State == 1) {
            if(Min_Time > LCD10JZ[Address].DLCD.Work_Time) {
                Min_Address = Address;
                Min_Time = LCD10JZ[Address].DLCD.Work_Time;
            }
        }
        
        // 统计运行中机组数
        if(LCD10JZ[Address].DLCD.Device_State == 2) {
            Already_WorkNumbers++;
        }
    }
}
```

#### 2. "大需求" 与 "小需求" 的含义

| 状态 | 条件 | 含义 | 对应变量 |
|------|------|------|----------|
| **大需求** | `Air_Power >= Max_Work_Power` | 机组满功率运行，供热不足 | `Big_time++` |
| **中等** | `45 < Air_Power < Max_Power` | 功率适中，清零计数器 | 清零所有计数 |
| **小需求** | `25 <= Air_Power <= 45` 且 `压力 >= 设定值` | 机组低功率运行，供热过剩 | `Small_time++` |
| **零功率** | `Air_Power == 0` | 停炉保压状态 | `Zero_time++` |

```c
// 大需求: 满功率运行
if(LCD10JZ[Address].DLCD.Air_Power >= LCD10JZ[Address].DLCD.Max_Work_Power) {
    SlaveG[Address].Big_time++;
    SlaveG[Address].Small_time = 0;
    SlaveG[Address].Zero_time = 0;
}

// 小需求: 低功率运行 (25-45%) 且压力达标
if(LCD10JZ[Address].DLCD.Air_Power <= 45 && 
   LCD10JZ[Address].DLCD.Air_Power >= 25 &&
   Temperature_Data.Pressure_Value >= sys_config_data.zhuan_huan_temperture_value) {
    SlaveG[Address].Small_time++;
    SlaveG[Address].Big_time = 0;
    SlaveG[Address].Zero_time = 0;
}
```

#### 3. 增减载决策

```
┌────────────────────────────────────────────────────────────────┐
│                    每15秒评估一次                              │
├────────────────────────────────────────────────────────────────┤
│                                                                │
│  【增载条件】                                                   │
│  ┌─────────────────────────────────────────────────────────┐  │
│  │ 1. 所有运行机组的 Big_time > Balance_Big_Time           │  │
│  │    (所有机组都满功率运行超过设定时间)                    │  │
│  │                                                         │  │
│  │ 2. 且有待机机组可用 (Already_Work < AliveOk)            │  │
│  │                                                         │  │
│  │ → 启动运行时间最短的待机机组 (Min_Address)              │  │
│  └─────────────────────────────────────────────────────────┘  │
│                                                                │
│  【减载条件】                                                   │
│  ┌─────────────────────────────────────────────────────────┐  │
│  │ 1. 有2台以上机组的 Small_time > Balance_Small_Time      │  │
│  │    或 Zero_time > 500 (停机超过8分钟)                   │  │
│  │                                                         │  │
│  │ 2. 且运行机组数 > 需要数量 (Already_Work > Need_Numbers)│  │
│  │                                                         │  │
│  │ → 关闭运行时间最长的机组 (Max_Address)                  │  │
│  └─────────────────────────────────────────────────────────┘  │
│                                                                │
└────────────────────────────────────────────────────────────────┘
```

---

## 📈 状态机流程图

```
                          ┌──────────────┐
                          │  系统启动    │
                          └──────┬───────┘
                                 │
                                 ▼
                    ┌────────────────────────┐
                    │ UnionD.UnionStartFlag  │
                    │       == 0?            │
                    └────────────┬───────────┘
                                 │
              ┌──────────────────┼──────────────────┐
              │ YES              │                  │ NO
              ▼                  │                  ▼
    ┌─────────────────┐          │      ┌─────────────────────┐
    │ 待机状态        │          │      │ 每秒轮询            │
    │ 定期关闭残留    │          │      │ Union_MuxJiZu_      │
    │ 运行中的从机    │          │      │ Control_Function()  │
    └─────────────────┘          │      └──────────┬──────────┘
                                 │                 │
                                 │                 ▼
                                 │      ┌─────────────────────┐
                                 │      │ 1. 统计在线机组     │
                                 │      │ 2. 统计运行机组     │
                                 │      │ 3. 累计时间计数器   │
                                 │      └──────────┬──────────┘
                                 │                 │
                                 │                 ▼
                                 │      ┌─────────────────────┐
                                 │      │ 每15秒评估          │
                                 │      │ 增载/减载决策       │
                                 │      └──────────┬──────────┘
                                 │                 │
                                 │    ┌────────────┴────────────┐
                                 │    │                         │
                                 │    ▼                         ▼
                                 │ ┌──────────┐           ┌──────────┐
                                 │ │ 增载     │           │ 减载     │
                                 │ │启动最短  │           │关闭最长  │
                                 │ │运行时间  │           │运行时间  │
                                 │ │的待机机组│           │的运行机组│
                                 │ └──────────┘           └──────────┘
                                 │
                                 └────────────────────────────────────
```

---

## ⚠️ 关键参数配置

| 参数 | 存储位置 | 默认值 | 说明 |
|------|----------|--------|------|
| `Balance_Big_Time` | `Sys_Admin` | - | 大需求时间阈值 (秒) |
| `Balance_Small_Time` | `Sys_Admin` | - | 小需求时间阈值 (秒) |
| `zhuan_huan_temperture_value` | `sys_config_data` | - | 压力切换阈值 |
| `Key_Power` | `SlaveG[x]` | 1 | 从机使能权限 |
| `Fire_Power` | `SlaveG[x]` | 30% | 点火功率 |
| `Max_Power` | `SlaveG[x]` | 85% | 最大功率 |

---

## 🔧 潜在优化点

### 1. 增减载滞后

**问题**: 增减载决策基于15秒周期评估，响应较慢。

**建议**: 可考虑引入压力变化率预测，提前启动增载。

### 2. 均衡运行时间

**问题**: 当前仅在增载时选择运行时间最短的机组。

**建议**: 可定期轮换运行机组，实现磨损均衡。

### 3. 故障恢复

**问题**: 故障机组恢复后无自动重新纳入联动。

**建议**: 增加故障恢复检测和自动重新使能逻辑。

---

## 📁 相关文件

| 文件 | 职责 |
|------|------|
| `HARDWARE/USART3/usart3.c` | 主从机通信、`Union_MuxJiZu_Control_Function()` |
| `HARDWARE/USART4/usart4.h` | `SLAVE_GG`, `UNION_GG` 结构体定义 |
| `HARDWARE/USART2/usart2.h` | `LCD10_JZ_Struct` 从机数据镜像定义 |
| `SYSTEM/water/water_control.c` | 双拼水位控制 `ShuangPin_Water_Balance_Function()` |
| `SYSTEM/config/sys_config.c` | 从机参数初始化/加载 |
| `SYSTEM/in_flash/Internal_flash.c` | 从机参数持久化存储 |

---

## 📝 变更提案状态

- [x] 分析完成
- [ ] 待确认是否需要优化
- [ ] 待实施

---

*文档创建时间: 2025-01-03*
*作者: AI Assistant*

