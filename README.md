# BionicLobster-ROV

本项目是仿生龙虾水下机器人控制固件，面向 STM32F103C8 平台，负责推进、机械臂、尾部执行器、浮力调节与基础传感器采集。当前仓库保存的是底层控制工程，视觉部分位于独立仓库：<https://github.com/zhizhizzzzzzz/OceanSphere>。

README 里的外设分布、控制映射与注意事项，均按当前代码实现整理，而不是按早期结构图或历史注释整理。

## 项目概览

- 主控芯片：`STM32F103C8`
- 固件架构：`主循环 + 串口中断接收 + 定时器 PWM 输出 + 软件 I2C`
- 主要功能：
  - 机械臂与夹爪控制
  - 尾部双舵机控制
  - 多路推进电机控制
  - A4988 步进电机驱动的浮力调节
  - MS5837 压力 / 水深传感器采集
  - 上位机 / 遥控端串口指令接收

主程序入口位于 `Boston Big Guy V1/USER/main.c`，初始化顺序如下：

1. `MYGPIO_Init()`：PWM 输出相关 GPIO 初始化
2. `PWM_ParameterConfiguration()`：定时器 PWM 初始化
3. `uart_init2(115200)`：USART2 串口初始化
4. `ESC_Init()`：推进电机电调初始化
5. `MOTOR_Init()`：A4988 步进电机接口初始化
6. `MS5837_init()`：水压传感器初始化

## 开发环境

- 当前主开发链：`Linux + Arm GNU Toolchain + CMake + OpenOCD`
- 下载器：`DAPLink (CMSIS-DAP)`
- 保留兼容：`Keil MDK-ARM / uVision`
- Keil 工程文件：`Boston Big Guy V1/USER/Boston Big Guy.uvprojx`
- 固件库：`STM32F10x Standard Peripheral Library`
- GNU 启动文件：`Boston Big Guy V1/CORE/startup_stm32f103xb_gcc.c`
- GNU 链接脚本：`ld/stm32f103c8.ld`

## 工程结构

- `Boston Big Guy V1/USER`
  - 主程序入口、动作逻辑、Keil 工程文件、中断与系统配置
- `Boston Big Guy V1/HARDWARE`
  - 舵机、推进电机、MS5837、AS5600、JY901S、A4988 等驱动
- `Boston Big Guy V1/BSP`
  - GPIO、定时器、软件 I2C、Flash 等板级支持代码
- `Boston Big Guy V1/SYSTEM`
  - 串口、延时、系统时钟等系统层代码
- `Boston Big Guy V1/STM32F10x_FWLib`
  - STM32F10x 标准外设库
- `scripts`
  - 本地工具链、构建、烧录、调试脚本
- `openocd`
  - 板级 OpenOCD 配置

## 外设分布

以下分布基于 `Boston Big Guy V1/BSP/GPIO.c`、`Boston Big Guy V1/BSP/timer.c`、`Boston Big Guy V1/HARDWARE/Servo.c`、`Boston Big Guy V1/HARDWARE/A4988.c`、`Boston Big Guy V1/SYSTEM/usart/usart.c`。

### 1. PWM / 执行器分布

| 定时器通道 | 引脚 | 外设 | 控制接口 | 说明 |
| --- | --- | --- | --- | --- |
| `TIM1_CH1` | `PA8` | 左夹爪 180° 舵机 | `L_Claw()` | 左爪开合 |
| `TIM1_CH2` | `PA9` | 左肘 180° 舵机 | `L_Elbow()` | 左臂肘关节 |
| `TIM1_CH3` | `PA10` | 左上臂 360° 舵机 | `L_Upper_360()` | 连续旋转 |
| `TIM1_CH4` | `PA11` | 左下臂 360° 舵机 | `L_Lower_360()` | 连续旋转 |
| `TIM2_CH1` | `PA0` | 右夹爪 180° 舵机 | `R_Claw()` | 右爪开合 |
| `TIM2_CH2` | `PA1` | 右肘 180° 舵机 | `R_Elbow()` | 右臂肘关节 |
| `TIM2_CH3` | `PB10` | 右上臂 360° 舵机 | `R_Upper_360()` | `TIM2 PartialRemap2` 后输出到 `PB10` |
| `TIM2_CH4` | `PB11` | 右下臂 360° 舵机 | `R_Lower_360()` | `TIM2 PartialRemap2` 后输出到 `PB11` |
| `TIM3_CH1` | `PA6` | 尾部上舵机 | `Tail_Servo1()` | 尾部上侧执行器 |
| `TIM3_CH2` | `PA7` | 尾部下舵机 | `Tail_Servo2()` | 尾部下侧执行器 |
| `TIM3_CH4` | `PB1` | 右上推进电机 | `R_Motor()` | 推进 / 升沉辅助 |
| `TIM4_CH1` | `PB6` | 头部推进电机 | `HeadMotor()` | 头部推进 |
| `TIM4_CH2` | `PB7` | 左上推进电机 | `L_Motor()` | 推进 / 升沉辅助 |
| `TIM4_CH3` | `PB8` | 尾部推进电机 3 | `Tail_Motor3()` | 尾部推进 |
| `TIM4_CH4` | `PB9` | 尾部推进电机 2 | `Tail_Motor2()` | 尾部推进 |

补充说明：

- `TIM1`、`TIM2` 的自动重装值为 `19999`，按 72 MHz 主频和 71 分频计算，PWM 周期约为 `20 ms`。
- `TIM3`、`TIM4` 的自动重装值为 `1999`，PWM 周期约为 `2 ms`。
- `Servo.h` 中保留了 `Tail_Motor1()` 声明，但当前工程没有对应实现，`Action.c` 中相关调用也被注释掉了。

### 2. 串口、I2C 与步进电机分布

| 外设 | 引脚 / 资源 | 用途 | 当前状态 |
| --- | --- | --- | --- |
| `USART2_TX` | `PA2` | 上位机 / 遥控串口发送 | 已使用 |
| `USART2_RX` | `PA3` | 上位机 / 遥控串口接收 | 已使用 |
| 软件 I2C `SCL` | `PB4` | MS5837 / AS5600 时钟线 | 已使用 |
| 软件 I2C `SDA` | `PB3` | MS5837 / AS5600 数据线 | 已使用 |
| `A4988_DIR` | `PA4` | 步进电机方向控制 | 已使用 |
| `A4988_STEP` | `PA5` | 步进电机脉冲输出 | 已使用 |

### 3. 传感器与驱动接入情况

| 模块 | 驱动文件 | 接口 | 启用情况 |
| --- | --- | --- | --- |
| `MS5837` 压力 / 水深传感器 | `HARDWARE/MS5837.c` | 软件 I2C | 已在 `main.c` 初始化 |
| `AS5600` 磁编码器 | `HARDWARE/AS5600.c` | 软件 I2C | 有驱动，当前 `main.c` 未初始化 |
| `JY901S` 姿态传感器 | `HARDWARE/JY901S.c` | 串口方案预留 | 已有驱动框架，但当前接收解析基本被注释 |

## 控制逻辑

### 1. 主循环行为

`main.c` 中通过 `key`、`hand_OC`、`Left_Move`、`Right_Move` 四个状态字决定机器人动作：

- 全部为 `0x00` 时执行 `Start()`，回到待机 / 停止状态
- `key` 控制基础运动：前后左右、升降、转向
- `hand_OC` 控制夹爪开合
- `Left_Move`、`Right_Move` 控制机械臂舒展与浮力机构动作

动作函数集中定义在 `Boston Big Guy V1/USER/Action.c`。

### 2. 串口数据帧

USART2 接收使用中断方式，关键字节定义在 `Boston Big Guy V1/SYSTEM/usart/usart.h`：

- 帧头：`0xA5`
- 帧尾：`0x7B`
- 缓冲区长度：`256`

当前 `process_data()` 实际解析的是 7 字节数据帧中的以下字段：

| 字节下标 | 含义 |
| --- | --- |
| `0` | 帧头 |
| `1` | `key` |
| `2` | `Left_Move` |
| `3` | `Right_Move` |
| `4` | `hand_OC` |
| `5` | 当前代码未使用 |
| `6` | 帧尾 |

### 3. 控制码定义

基础运动控制来自 `Boston Big Guy V1/USER/main.h`：

| 变量 | 码值 | 动作 |
| --- | --- | --- |
| `key` | `0x01` | 前进 |
| `key` | `0x02` | 后退 |
| `key` | `0x04` | 左移 |
| `key` | `0x08` | 右移 |
| `key` | `0x10` | 上升 |
| `key` | `0x20` | 下降 |
| `hand_OC` | `0x01` | 左夹爪打开 |
| `hand_OC` | `0x04` | 右夹爪打开 |
| `Left_Move` | `0x10` | 左臂舒展 |
| `Right_Move` | `0x10` | 右臂舒展 |
| `Left_Move` | `0x10` | 排水 |
| `Right_Move` | `0x20` | 吸水 |

这里要特别注意：当前代码里有几组控制码复用，文档保留“现状”而不做美化。

## 编译与烧录

1. 运行 `./scripts/setup-local-toolchain.sh` 下载仓库内本地工具链。
2. 运行 `./scripts/build.sh` 编译固件。
3. 使用 `./scripts/flash.sh` 通过 `DAPLink (CMSIS-DAP)` 烧录。
4. 如需调试，运行 `./scripts/openocd-gdb-server.sh` 或使用 VS Code `cortex-debug` 配置。

如果仍需使用 Keil，仓库中保留了 `Boston Big Guy V1/USER/Boston Big Guy.uvprojx`。

如果只想快速定位核心文件，优先看这些：

- `Boston Big Guy V1/USER/main.c`
- `Boston Big Guy V1/USER/Action.c`
- `Boston Big Guy V1/BSP/GPIO.c`
- `Boston Big Guy V1/BSP/timer.c`
- `Boston Big Guy V1/SYSTEM/usart/usart.c`

## Linux 工具链

仓库现在提供了尽量不影响系统环境的 Linux 构建链，默认把工具下载到仓库内的 `./.tools/`，不会写入系统目录。

### 1. 初始化本地工具链

```bash
./scripts/setup-local-toolchain.sh
```

- 默认下载：`Arm GNU Toolchain` 与 `xPack OpenOCD`
- 如果只想先编译、不下载 OpenOCD：

```bash
./scripts/setup-local-toolchain.sh --arm-only
```

### 2. 编译固件

```bash
./scripts/build.sh
```

构建输出位于 `build/`：

- `build/TIMER.elf`
- `build/TIMER.hex`
- `build/TIMER.bin`
- `build/TIMER.map`

### 3. 烧录

如果本地 OpenOCD 已下载，或系统里已有 `openocd` / `st-flash`，可以直接运行：

```bash
./scripts/flash.sh
```

默认按 `DAPLink(CMSIS-DAP) + STM32F1` 组合调用：

- `openocd/daplink-stm32f103c8.cfg`
- 其中内部引用 `interface/cmsis-dap.cfg` 与 `target/stm32f1x.cfg`

### 4. 调试

命令行启动 GDB Server：

```bash
./scripts/openocd-gdb-server.sh
```

如果使用 VS Code，可直接使用新增的 `tasks.json` / `launch.json`。`launch.json` 采用 `cortex-debug` 配置，需本机安装对应扩展。

### 5. 迁移时做的兼容修正

- 新增 GCC 启动入口：`Boston Big Guy V1/CORE/startup_stm32f103xb_gcc.c`
- 新增 GCC 链接脚本：`ld/stm32f103c8.ld`
- 新增 CMake 构建：`CMakeLists.txt`、`cmake/arm-none-eabi-gcc.cmake`
- 修复 Linux 下大小写敏感头文件引用：`MS5837.h`、`JY901S.h`
- 将 `SYSTEM/sys/sys.c` 中 `MSR_MSP()` 的 ARMCC 专用写法改为 GCC 兼容实现
- 新增 `SYSTEM/usart/syscalls_gcc.c`，为 newlib 提供最小系统调用封装

### 6. 说明

- 仓库里保留了 `Keil` 的核心工程文件 `Boston Big Guy V1/USER/Boston Big Guy.uvprojx`，但已移除大部分 Keil 生成物
- `CMakePresets.json` 提供了新版本 CMake 的 preset 配置；如果系统自带 CMake 较老，直接用 `./scripts/build.sh` 即可
- 当前默认调试下载器为 `DAPLink`；如后续更换探针，只需修改 `OPENOCD_CONFIG` 或 `openocd/` 下配置文件

## 已知注意事项

- `BSP/IIC.c` 中软件 I2C 的写引脚使用的是 `PB4/PB3`，但 `MyI2C_R_SDA()` 当前读取的是 `PA3`，这与 `USART2_RX` 共用引脚，接线和调试前需要重点核对。
- `MS5837.c` 内部注释仍写着 “PB8 PB9”，但实际软件 I2C 初始化代码是 `PB4/PB3`，应以代码实现为准。
- `main.h` 中存在控制码复用：
  - `Left` 与 `Left_Spin` 都是 `0x04`
  - `Right` 与 `Right_Spin` 都是 `0x08`
  - `Left_ARM_OPEN` 与 `water_push` 都是 `0x10`
- 由于上述复用，`main.c` 里部分 `if` 判断会在同一个码值下连续触发多个动作，这一点在联调时要特别注意。
- `BSP/PWM.c` 当前为空文件，PWM 真实初始化逻辑在 `BSP/timer.c`。
- `AS5600` 与 `JY901S` 驱动文件已加入工程，但当前主流程没有完整启用。

## License

本项目采用 [MIT License](./LICENSE)。
