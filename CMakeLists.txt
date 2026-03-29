cmake_minimum_required(VERSION 3.16)

project(TIMER C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(PROJECT_ROOT "${CMAKE_CURRENT_SOURCE_DIR}")
set(FIRMWARE_ROOT "${PROJECT_ROOT}/Boston Big Guy V1")
set(LINKER_SCRIPT "${PROJECT_ROOT}/ld/stm32f103c8.ld")

set(FIRMWARE_SOURCES
    "${FIRMWARE_ROOT}/USER/main.c"
    "${FIRMWARE_ROOT}/USER/stm32f10x_it.c"
    "${FIRMWARE_ROOT}/USER/system_stm32f10x.c"
    "${FIRMWARE_ROOT}/USER/Action.c"
    "${FIRMWARE_ROOT}/HARDWARE/led.c"
    "${FIRMWARE_ROOT}/HARDWARE/AS5600.c"
    "${FIRMWARE_ROOT}/HARDWARE/MS5837.c"
    "${FIRMWARE_ROOT}/HARDWARE/JY901S.c"
    "${FIRMWARE_ROOT}/HARDWARE/A4988.c"
    "${FIRMWARE_ROOT}/HARDWARE/Servo.c"
    "${FIRMWARE_ROOT}/HARDWARE/Motor.c"
    "${FIRMWARE_ROOT}/BSP/GPIO.c"
    "${FIRMWARE_ROOT}/BSP/stmflash.c"
    "${FIRMWARE_ROOT}/BSP/timer.c"
    "${FIRMWARE_ROOT}/BSP/IIC.c"
    "${FIRMWARE_ROOT}/SYSTEM/delay/delay.c"
    "${FIRMWARE_ROOT}/SYSTEM/sys/sys.c"
    "${FIRMWARE_ROOT}/SYSTEM/usart/usart.c"
    "${FIRMWARE_ROOT}/SYSTEM/usart/syscalls_gcc.c"
    "${FIRMWARE_ROOT}/CORE/startup_stm32f103xb_gcc.c"
    "${FIRMWARE_ROOT}/STM32F10x_FWLib/src/misc.c"
    "${FIRMWARE_ROOT}/STM32F10x_FWLib/src/stm32f10x_gpio.c"
    "${FIRMWARE_ROOT}/STM32F10x_FWLib/src/stm32f10x_rcc.c"
    "${FIRMWARE_ROOT}/STM32F10x_FWLib/src/stm32f10x_tim.c"
    "${FIRMWARE_ROOT}/STM32F10x_FWLib/src/stm32f10x_usart.c"
    "${FIRMWARE_ROOT}/STM32F10x_FWLib/src/stm32f10x_flash.c"
    "${FIRMWARE_ROOT}/STM32F10x_FWLib/src/stm32f10x_adc.c"
)

add_executable(${PROJECT_NAME}.elf ${FIRMWARE_SOURCES})

set_target_properties(${PROJECT_NAME}.elf PROPERTIES OUTPUT_NAME ${PROJECT_NAME})

target_include_directories(${PROJECT_NAME}.elf PRIVATE
    "${FIRMWARE_ROOT}/SYSTEM/delay"
    "${FIRMWARE_ROOT}/SYSTEM/sys"
    "${FIRMWARE_ROOT}/SYSTEM/usart"
    "${FIRMWARE_ROOT}/USER"
    "${FIRMWARE_ROOT}/STM32F10x_FWLib/inc"
    "${FIRMWARE_ROOT}/CORE"
    "${FIRMWARE_ROOT}/BSP"
    "${FIRMWARE_ROOT}/HARDWARE"
)

target_compile_definitions(${PROJECT_NAME}.elf PRIVATE
    STM32F10X_MD
    USE_STDPERIPH_DRIVER
)

target_compile_options(${PROJECT_NAME}.elf PRIVATE
    -mcpu=cortex-m3
    -mthumb
    -ffunction-sections
    -fdata-sections
    -fno-common
    -Wall
    -Wextra
    -Wno-unused-parameter
    -Wno-sign-compare
    -Wno-implicit-fallthrough
)

set(MAP_FILE "${CMAKE_BINARY_DIR}/${PROJECT_NAME}.map")

target_link_options(${PROJECT_NAME}.elf PRIVATE
    -mcpu=cortex-m3
    -mthumb
    -T${LINKER_SCRIPT}
)

set_target_properties(${PROJECT_NAME}.elf PROPERTIES
    LINK_FLAGS "--specs=nano.specs -Wl,--gc-sections -Wl,-Map=${MAP_FILE} -Wl,--print-memory-usage -Wl,--no-warn-rwx-segments"
)

find_program(CMAKE_OBJCOPY arm-none-eabi-objcopy REQUIRED)
find_program(CMAKE_SIZE arm-none-eabi-size REQUIRED)

add_custom_command(TARGET ${PROJECT_NAME}.elf POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -O ihex $<TARGET_FILE:${PROJECT_NAME}.elf> "${CMAKE_BINARY_DIR}/${PROJECT_NAME}.hex"
    COMMAND ${CMAKE_OBJCOPY} -O binary $<TARGET_FILE:${PROJECT_NAME}.elf> "${CMAKE_BINARY_DIR}/${PROJECT_NAME}.bin"
    COMMAND ${CMAKE_SIZE} --format=berkeley $<TARGET_FILE:${PROJECT_NAME}.elf>
    BYPRODUCTS
        "${CMAKE_BINARY_DIR}/${PROJECT_NAME}.hex"
        "${CMAKE_BINARY_DIR}/${PROJECT_NAME}.bin"
    VERBATIM
)
