# BionicLobster-ROV

<p align="center">
  <a href="./README.md">简体中文</a> · <a href="./README.en.md">English</a> · <a href="https://github.com/iowqi/ShrimpROV">机械部分开源项目</a> · <a href="https://github.com/zhizhizzzzzzz/OceanSphere">视觉 / 感知开源项目</a>
</p>

<p align="center">
  <img src="Images/V2.png" alt="BionicLobster-ROV 项目总览" width="82%">
</p>

<p align="center">
  <strong>面向仿生龙虾水下机器人的双 STM32 固件仓库</strong><br/>
  负责执行器驱动、传感器采集、板间通信，以及背部四旋翼稳定控制。
</p>

## 项目速览

| 项目 | 说明 |
| --- | --- |
| 主控架构 | 双 `STM32F103C8` |
| 固件分工 | `firmware` 主板固件 / `firmware_quad` 推进与四旋翼从板固件 |
| 语言与框架 | C 语言 + STM32 标准外设库 |
| 构建方式 | CMake + 本地 ARM GCC 工具链 |
| 烧录方式 | `DAPLink (CMSIS-DAP)` + OpenOCD |
| 仓库定位 | 底层嵌入式控制，不包含高层视觉与自主策略 |

> 仓库内的脚本会优先复用 `.tools/` 下的本地工具链，适合在 Linux 环境中快速搭建一套可复现的构建与烧录流程。

## 控制架构

| 固件 | 板卡角色 | 负责内容 | 关键外设 |
| --- | --- | --- | --- |
| `firmware` | 主控制板 | 机械臂、夹爪、尾部舵机、压力与角度采集、遥控/上位机通信、向从板下发动作指令 | 6x `AS5600`、`MS5837`、`USART1`、`USART2` |
| `firmware_quad` | 推进与稳定从板 | 头部推进、尾部推进、背部四旋翼电机、姿态解算与轻量级闭环控制 | `JY901S`、`USART1`、`USART2` |

主板执行器分布：

- 2 个肘部 180 度舵机
- 2 个夹爪舵机
- 4 个手臂连续旋转舵机
- 2 个尾部舵机

从板执行器分布：

- 1 个头部推进电机
- 4 个尾部推进电机
- 4 个背部四旋翼电机

## 快速开始

### 1. 初始化本地工具链

```bash
./scripts/setup-local-toolchain.sh
```

如果暂时只想安装 ARM 编译器：

```bash
./scripts/setup-local-toolchain.sh --arm-only
```

### 2. 编译两份固件

```bash
./scripts/build.sh
```

编译产物位于 `build/`：

| 文件 | 说明 |
| --- | --- |
| `build/firmware.elf` | 主板调试镜像 |
| `build/firmware.hex` | 主板烧录镜像 |
| `build/firmware.bin` | 主板二进制镜像 |
| `build/firmware_quad.elf` | 从板调试镜像 |
| `build/firmware_quad.hex` | 从板烧录镜像 |
| `build/firmware_quad.bin` | 从板二进制镜像 |

### 3. 烧录

烧录主板：

```bash
./scripts/flash.sh build/firmware.elf
```

烧录推进 / 四旋翼从板：

```bash
./scripts/flash.sh build/firmware_quad.elf
```

### 4. 可选：载入当前 shell 环境

```bash
source ./scripts/env.sh
```

补充说明：

- `build.sh` 会在找不到 ARM GCC 时自动补装本地编译器。
- `flash.sh` 会在目标 `ELF` 不存在时自动触发一次编译。
- 仓库保留了 `CMakePresets.json`，较新的 CMake 可直接配合使用。
- 当前默认烧录链路为 `DAPLink (CMSIS-DAP) + OpenOCD`。

## 仓库结构

| 路径 | 说明 |
| --- | --- |
| `firmware/` | 主板固件源码 |
| `firmware_quad/` | 推进 / 四旋翼从板固件源码 |
| `scripts/` | 工具链安装、构建、烧录、调试脚本 |
| `cmake/` | ARM GCC 工具链配置 |
| `openocd/` | OpenOCD 接口与目标配置 |
| `ld/` | STM32 链接脚本 |
| `Images/` | 项目图片与参考图 |

## 当前固件说明

- 主板已经提供 6 路具名 `AS5600` 通道。
- 从板实现了一套轻量级闭环飞控，基于 `JY901S` 角度与陀螺数据完成 `roll/pitch` 稳定、`yaw` 保持、指令斜坡和 IMU 失效降级。
- 本仓库聚焦底层控制，高层自主策略与视觉能力在其他仓库维护。

<details>
  <summary>展开查看 6 路 <code>AS5600</code> 通道名称</summary>

  - `left_arm_upper_360`
  - `left_arm_lower_360`
  - `right_arm_upper_360`
  - `right_arm_lower_360`
  - `tail_servo1`
  - `tail_servo2`
</details>

## 致谢与关联项目

- 机械部分：`真甲咒`、`小白`
- 视觉 / 感知：`大道寺知世`
- 机械部分开源项目：<https://github.com/iowqi/ShrimpROV>
- 视觉 / 感知开源项目：<https://github.com/zhizhizzzzzzz/OceanSphere>

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
