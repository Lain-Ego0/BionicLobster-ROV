# BionicLobster-ROV

[简体中文](./README.md) | [English](./README.en.md)

<p align="center">
  <img src="Images/V2.png" alt="BionicLobster-ROV overview" width="82%">
</p>

BionicLobster-ROV is a biomimetic underwater robot project built around dual `STM32F103C8` controllers. This repository contains the embedded firmware layer for actuation, sensing, board-to-board communication, and dorsal quad-thruster stabilization.

Mechanical design and mechanical structure author: `zhenjiazhou`.

The vision / perception side is maintained in a separate repository: <https://github.com/zhizhizzzzzzz/OceanSphere>.

## Highlights

- Dual-board control architecture:
  - `firmware`: main board for manipulators, tail servos, 6x `AS5600`, `MS5837`, remote link, and command dispatch.
  - `firmware_quad`: coprocessor for 1 head thruster, 4 tail thrusters, 4 dorsal quad motors, and `JY901S` IMU processing.
- STM32 Standard Peripheral Library based project with plain C, timer PWM, USART interrupts, and software I2C.
- Local Linux-friendly toolchain flow with repo-scoped scripts for build, flash, and OpenOCD.

## Architecture

| Firmware | Role | Main peripherals |
| --- | --- | --- |
| `firmware` | Main control board | 2x 180-degree elbow servos, 2x claw servos, 4x arm continuous servos, 2x tail servos, 6x `AS5600`, `MS5837`, `USART1` board link, `USART2` remote / host link |
| `firmware_quad` | Quad-thruster and propulsion board | 4x dorsal quad motors, 4x tail thrusters, 1x head thruster, `JY901S` IMU, `USART1` board link, `USART2` IMU link |

## Repository Layout

- `firmware/`: main-board firmware
- `firmware_quad/`: propulsion / quad-thruster coprocessor firmware
- `Images/`: project images and reference figures
- `scripts/`: toolchain setup, build, flash, and debug helpers
- `cmake/`: ARM GCC toolchain configuration
- `openocd/`: OpenOCD target/interface configuration
- `ld/`: linker scripts

## Build

### 1. Set up the local toolchain

```bash
./scripts/setup-local-toolchain.sh
```

If you only need the compiler first:

```bash
./scripts/setup-local-toolchain.sh --arm-only
```

### 2. Build both firmware images

```bash
./scripts/build.sh
```

Build outputs:

- `build/firmware.elf`
- `build/firmware.hex`
- `build/firmware.bin`
- `build/firmware_quad.elf`
- `build/firmware_quad.hex`
- `build/firmware_quad.bin`

### 3. Flash

Flash the main board:

```bash
./scripts/flash.sh build/firmware.elf
```

Flash the propulsion / quad board:

```bash
./scripts/flash.sh build/firmware_quad.elf
```

### 4. Notes

- `CMakePresets.json` is included for newer CMake versions.
- On older Linux distributions, the scripts are the most reliable entry point.
- The default flashing workflow assumes `DAPLink (CMSIS-DAP)` plus OpenOCD.

## Firmware Notes

- The main board currently exposes named `AS5600` channels for:
  - `left_arm_upper_360`
  - `left_arm_lower_360`
  - `right_arm_upper_360`
  - `right_arm_lower_360`
  - `tail_servo1`
  - `tail_servo2`
- The quad board now uses a lightweight closed-loop flight controller based on `JY901S` angle and gyro data, including roll/pitch stabilization, yaw hold, command slew limiting, and IMU failover.
- The repository is focused on low-level control. Higher-level autonomy and vision are intentionally kept separate.

## Media

<p align="center">
  <img src="Images/V3-1.png" alt="Project image 1" width="31%">
  <img src="Images/V3-2.png" alt="Project image 2" width="31%">
  <img src="Images/V3-3.png" alt="Project image 3" width="31%">
</p>

<p align="center">
  <img src="Images/F103C8t6.png" alt="STM32F103C8 reference board" width="72%">
</p>

## Credits

- Mechanical design and mechanical structure: `zhenjiazhou`、`iowqi`
- Vision / perception repository: `zhizhi`

## License

This project is released under the [MIT License](./LICENSE).
