目前需要进行以下修改：

由于引脚不够所以开了两个f103c8t6主控，分别为：
/home/lain/Documents/GitHub/BionicLobster-ROV/firmware
/home/lain/Documents/GitHub/BionicLobster-ROV/firmware_quad

易得：firmware_quad是处理机器人新构型的背部四旋翼使用的


注意：核心关注引脚分配，其他有冲突的自行判断。以firmware为主板 firmware_quad为四旋翼以及其他电机推进的从板。主要的控制逻辑放在firmware

目前新引脚分配如下：
对于/home/lain/Documents/GitHub/BionicLobster-ROV/firmware
用于读取5个AS5600编码器 也就是手臂360度舵机一手2个两手4个加上尾部放在第一个线驱关节处一个
控制6个360度舵机 肘部两个180舵机 夹取两个180舵机
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

### 2. 串口、I2C 与步进电机分布

| 外设 | 引脚 / 资源 | 用途 | 当前状态 |
| --- | --- | --- | --- |
| `USART2_TX` | `PA2` | 上位机 / 遥控串口发送 | 
| `USART2_RX` | `PA3` | 上位机 / 遥控串口接收 | 
| `USART1_TX` | `PB6` | 板间串口发送 |  
| `USART1_RX` | `PB7` | 板间串口接收 | 


你再创建4组并读取AS5600数据就行，不占用此芯片已有的引脚
| 软件 I2C `SCL` | `PB4` | MS5837 / AS5600 时钟线 | 已使用 |
| 软件 I2C `SDA` | `PB3` | MS5837 / AS5600 数据线 | 已使用 |

### 3. 传感器与驱动接入情况

| 模块 | 驱动文件 | 接口 | 启用情况 |
| --- | --- | --- | --- |
| `MS5837` 压力 / 水深传感器 | `HARDWARE/MS5837.c` | 软件 I2C | |
| `AS5600` 磁编码器 | `HARDWARE/AS5600.c` | 软件 I2C |  

## 控制逻辑

### 1. 主循环行为

`main.c` 中通过 `key`、`hand_OC`、`Left_Move`、`Right_Move` 四个状态字决定机器人动作：

- 全部为 `0x00` 时执行 `Start()`，回到待机 / 停止状态
- `key` 控制基础运动：前后左右、升降、转向
- `hand_OC` 控制夹爪开合
- `Left_Move`、`Right_Move` 控制机械臂舒展与浮力机构动作

动作函数集中定义在 `firmware/USER/Action.c`。

### 2. 串口数据帧

USART2 接收使用中断方式，关键字节定义在 `firmware/SYSTEM/usart/usart.h`：

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

基础运动控制来自 `firmware/USER/main.h`：

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





对于/home/lain/Documents/GitHub/BionicLobster-ROV/firmware_quad
控制头部一个电机 背部4个电机 尾部4个电机 其中背部4个电机为飞控四旋翼算法（数据从IMU取）
### 1. PWM / 执行器分布
（这里的函数是错乱的，以外设名为准）
| 定时器通道 | 引脚 | 外设 | 控制接口 | 说明 |
| --- | --- | --- | --- | --- |
| `TIM3_CH4` | `PA8` | 头部推进电机 | `R_Motor1()` |  
| `TIM1_CH1` | `PA8` |  左上| `HeadMotor()` | 
| `TIM1_CH2` | `PA9` | 右上 | `L_Motor2()` | 
| `TIM1_CH3` | `PA10` | 左下 | `Tail_Motor3()` | 
| `TIM1_CH4` | `PA11` | 右下 | `Tail_Motor2()` | 
| `TIM2_CH1` | `PA0` | 尾部推进1| `R_Claw()` | 
| `TIM2_CH2` | `PA1` | 尾部推进2 | `R_Elbow()` | 
| `TIM2_CH3` | `PB10` | 尾部推进3 | `R_Upper_360()` | 
| `TIM2_CH4` | `PB11` | 尾部推进4 | `R_Lower_360()` | 

### 2. 串口、I2C 与步进电机分布

| 外设 | 引脚 / 资源 | 用途 | 当前状态 |
| --- | --- | --- | --- |
| `USART1_TX` | `PB6` | 板间串口发送 |  
| `USART1_RX` | `PB7` | 板间串口接收 | 
| `USART2_TX` | `PA2` | IMU发送 | 
| `USART2_RX` | `PA3` | IMU接收 | 

### 3. 传感器与驱动接入情况

| 模块 | 驱动文件 | 接口 | 启用情况 |
| --- | --- | --- | --- |
| `JY901S` 姿态传感器 | `HARDWARE/JY901S.c` | 串口方案预留 | 已有驱动框架，但当前接收解析基本被注释 |

## 控制逻辑

### 1. 主循环行为

`main.c` 中通过 `key`、`hand_OC`、`Left_Move`、`Right_Move` 四个状态字决定机器人动作：

- 全部为 `0x00` 时执行 `Start()`，回到待机 / 停止状态
- `key` 控制基础运动：前后左右、升降、转向
- `hand_OC` 控制夹爪开合
- `Left_Move`、`Right_Move` 控制机械臂舒展与浮力机构动作

动作函数集中定义在 `firmware/USER/Action.c`。

### 2. 串口数据帧

USART2 接收使用中断方式，关键字节定义在 `firmware/SYSTEM/usart/usart.h`：

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

基础运动控制来自 `firmware/USER/main.h`：

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