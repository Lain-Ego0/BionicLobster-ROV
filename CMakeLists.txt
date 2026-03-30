cmake_minimum_required(VERSION 3.16)

project(BionicLobsterROV C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(PROJECT_ROOT "${CMAKE_CURRENT_SOURCE_DIR}")
set(LINKER_SCRIPT "${PROJECT_ROOT}/ld/stm32f103c8.ld")

find_program(CMAKE_OBJCOPY arm-none-eabi-objcopy REQUIRED)
find_program(CMAKE_SIZE arm-none-eabi-size REQUIRED)

function(add_stm32_target target_name firmware_root)
    add_executable(${target_name}.elf ${ARGN}
        "${firmware_root}/STM32F10x_FWLib/src/misc.c"
        "${firmware_root}/STM32F10x_FWLib/src/stm32f10x_gpio.c"
        "${firmware_root}/STM32F10x_FWLib/src/stm32f10x_rcc.c"
        "${firmware_root}/STM32F10x_FWLib/src/stm32f10x_tim.c"
        "${firmware_root}/STM32F10x_FWLib/src/stm32f10x_usart.c"
    )

    set_target_properties(${target_name}.elf PROPERTIES
        OUTPUT_NAME ${target_name}
        SUFFIX ".elf"
    )

    target_include_directories(${target_name}.elf PRIVATE
        "${firmware_root}/SYSTEM/delay"
        "${firmware_root}/SYSTEM/sys"
        "${firmware_root}/SYSTEM/usart"
        "${firmware_root}/USER"
        "${firmware_root}/STM32F10x_FWLib/inc"
        "${firmware_root}/CORE"
        "${firmware_root}/BSP"
        "${firmware_root}/HARDWARE"
    )

    target_compile_definitions(${target_name}.elf PRIVATE
        STM32F10X_MD
        USE_STDPERIPH_DRIVER
    )

    target_compile_options(${target_name}.elf PRIVATE
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

    set(map_file "${CMAKE_BINARY_DIR}/${target_name}.map")

    target_link_options(${target_name}.elf PRIVATE
        -mcpu=cortex-m3
        -mthumb
        -T${LINKER_SCRIPT}
    )

    set_target_properties(${target_name}.elf PROPERTIES
        LINK_FLAGS "--specs=nano.specs -Wl,--gc-sections -Wl,-Map=${map_file} -Wl,--print-memory-usage -Wl,--no-warn-rwx-segments"
    )

    add_custom_command(TARGET ${target_name}.elf POST_BUILD
        COMMAND ${CMAKE_OBJCOPY} -O ihex $<TARGET_FILE:${target_name}.elf> "${CMAKE_BINARY_DIR}/${target_name}.hex"
        COMMAND ${CMAKE_OBJCOPY} -O binary $<TARGET_FILE:${target_name}.elf> "${CMAKE_BINARY_DIR}/${target_name}.bin"
        COMMAND ${CMAKE_SIZE} --format=berkeley $<TARGET_FILE:${target_name}.elf>
        BYPRODUCTS
            "${CMAKE_BINARY_DIR}/${target_name}.hex"
            "${CMAKE_BINARY_DIR}/${target_name}.bin"
        VERBATIM
    )
endfunction()

set(FIRMWARE_ROOT "${PROJECT_ROOT}/firmware")
add_stm32_target(firmware "${FIRMWARE_ROOT}"
    "${FIRMWARE_ROOT}/USER/main.c"
    "${FIRMWARE_ROOT}/USER/stm32f10x_it.c"
    "${FIRMWARE_ROOT}/USER/system_stm32f10x.c"
    "${FIRMWARE_ROOT}/USER/Action.c"
    "${FIRMWARE_ROOT}/HARDWARE/AS5600.c"
    "${FIRMWARE_ROOT}/HARDWARE/MS5837.c"
    "${FIRMWARE_ROOT}/HARDWARE/Servo.c"
    "${FIRMWARE_ROOT}/BSP/GPIO.c"
    "${FIRMWARE_ROOT}/BSP/stmflash.c"
    "${FIRMWARE_ROOT}/BSP/timer.c"
    "${FIRMWARE_ROOT}/BSP/IIC.c"
    "${FIRMWARE_ROOT}/SYSTEM/delay/delay.c"
    "${FIRMWARE_ROOT}/SYSTEM/sys/sys.c"
    "${FIRMWARE_ROOT}/SYSTEM/usart/usart.c"
    "${FIRMWARE_ROOT}/SYSTEM/usart/syscalls_gcc.c"
    "${FIRMWARE_ROOT}/CORE/startup_stm32f103xb_gcc.c"
    "${FIRMWARE_ROOT}/STM32F10x_FWLib/src/stm32f10x_flash.c"
)

set(FIRMWARE_QUAD_ROOT "${PROJECT_ROOT}/firmware_quad")
add_stm32_target(firmware_quad "${FIRMWARE_QUAD_ROOT}"
    "${FIRMWARE_QUAD_ROOT}/USER/main.c"
    "${FIRMWARE_QUAD_ROOT}/USER/stm32f10x_it.c"
    "${FIRMWARE_QUAD_ROOT}/USER/system_stm32f10x.c"
    "${FIRMWARE_QUAD_ROOT}/USER/Action.c"
    "${FIRMWARE_QUAD_ROOT}/USER/FlightControl.c"
    "${FIRMWARE_QUAD_ROOT}/HARDWARE/JY901S.c"
    "${FIRMWARE_QUAD_ROOT}/HARDWARE/Motor.c"
    "${FIRMWARE_QUAD_ROOT}/HARDWARE/Servo.c"
    "${FIRMWARE_QUAD_ROOT}/BSP/GPIO.c"
    "${FIRMWARE_QUAD_ROOT}/BSP/timer.c"
    "${FIRMWARE_QUAD_ROOT}/SYSTEM/delay/delay.c"
    "${FIRMWARE_QUAD_ROOT}/SYSTEM/sys/sys.c"
    "${FIRMWARE_QUAD_ROOT}/SYSTEM/usart/usart.c"
    "${FIRMWARE_QUAD_ROOT}/SYSTEM/usart/syscalls_gcc.c"
    "${FIRMWARE_QUAD_ROOT}/CORE/startup_stm32f103xb_gcc.c"
)
