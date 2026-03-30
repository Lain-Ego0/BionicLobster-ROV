# BionicLobster-ROV

[简体中文](./README.md) | [English](./README.en.md)

<p align="center">
  <img src="Images/V2.png" alt="BionicLobster-ROV 项目总览" width="82%">
</p>

BionicLobster-ROV 是一个基于双 `STM32F103C8` 主控的仿生龙虾水下机器人项目。本仓库保存的是底层嵌入式固件，负责执行器驱动、传感器采集、板间通信以及背部四旋翼稳定控制。

## 致谢

- 机械部分：`真甲咒`、`小白`
- 视觉/感知：`大道寺知世`

机械部分开源： https://github.com/iowqi/ShrimpROV
视觉/感知部分开源：<https://github.com/zhizhizzzzzzz/OceanSphere>。

## 项目亮点

- 双板控制架构：
  - `firmware`：主板，负责机械臂、尾部舵机、6 路 `AS5600`、`MS5837`、遥控串口以及向从板下发指令。
  - `firmware_quad`：从板，负责头部推进、尾部 4 个推进电机、背部 4 个四旋翼电机以及 `JY901S` 姿态数据处理。
- 使用 STM32 标准外设库，基于 C 语言实现，包含定时器 PWM、串口中断和软件 I2C。
- 提供适合 Linux 的本地工具链、编译和烧录脚本，工具默认放在仓库内。

## 固件架构

| 固件 | 作用 | 主要外设 |
| --- | --- | --- |
| `firmware` | 主控制板 | 2 个肘部 180 度舵机、2 个夹爪舵机、4 个手臂连续旋转舵机、2 个尾部舵机、6 路 `AS5600`、`MS5837`、`USART1` 板间串口、`USART2` 上位机 / 遥控串口 |
| `firmware_quad` | 推进与四旋翼从板 | 4 个背部四旋翼电机、4 个尾部推进电机、1 个头部推进电机、`JY901S` IMU、`USART1` 板间串口、`USART2` IMU 串口 |

## 仓库结构

- `firmware/`：主板固件
- `firmware_quad/`：推进 / 四旋翼从板固件
- `Images/`：项目图片与参考图
- `scripts/`：工具链安装、编译、烧录、调试脚本
- `cmake/`：ARM GCC 工具链配置
- `openocd/`：OpenOCD 配置
- `ld/`：链接脚本

## 编译

### 1. 初始化本地工具链

```bash
./scripts/setup-local-toolchain.sh
```

如果你暂时只想先安装编译器：

```bash
./scripts/setup-local-toolchain.sh --arm-only
```

### 2. 编译两份固件

```bash
./scripts/build.sh
```

输出文件位于 `build/`：

- `build/firmware.elf`
- `build/firmware.hex`
- `build/firmware.bin`
- `build/firmware_quad.elf`
- `build/firmware_quad.hex`
- `build/firmware_quad.bin`

### 3. 烧录

烧录主板：

```bash
./scripts/flash.sh build/firmware.elf
```

烧录推进 / 四旋翼从板：

```bash
./scripts/flash.sh build/firmware_quad.elf
```

### 4. 说明

- 仓库中保留了 `CMakePresets.json`，适合较新的 CMake。
- 如果系统 CMake 版本较老，优先使用 `scripts/` 中的脚本。
- 当前默认烧录链路是 `DAPLink (CMSIS-DAP) + OpenOCD`。

## 当前固件说明

- 主板已经提供 6 路具名 `AS5600` 通道：
  - `left_arm_upper_360`
  - `left_arm_lower_360`
  - `right_arm_upper_360`
  - `right_arm_lower_360`
  - `tail_servo1`
  - `tail_servo2`
- 从板当前实现了一套轻量级闭环飞控，基于 `JY901S` 的角度与陀螺数据完成 `roll/pitch` 稳定、`yaw` 保持、指令斜坡与 IMU 失效降级。
- 本仓库聚焦底层控制，高层自主策略和视觉能力不放在这里维护。

## 图片

<p align="center">
  <img src="Images/V3-1.png" alt="项目图片 1" width="31%">
  <img src="Images/V3-2.png" alt="项目图片 2" width="31%">
  <img src="Images/V3-3.png" alt="项目图片 3" width="31%">
</p>

<p align="center">
  <img src="Images/F103C8t6.png" alt="STM32F103C8 开发板参考图" width="72%">
</p>

## License

本项目使用 [MIT License](./LICENSE)。
