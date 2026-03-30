set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR cortex-m3)
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

set(_repo_root "${CMAKE_CURRENT_LIST_DIR}/..")
set(_toolchain_root "")

if(DEFINED ENV{ARM_GNU_TOOLCHAIN_DIR} AND NOT "$ENV{ARM_GNU_TOOLCHAIN_DIR}" STREQUAL "")
    file(TO_CMAKE_PATH "$ENV{ARM_GNU_TOOLCHAIN_DIR}" _toolchain_root)
else()
    file(GLOB _toolchain_candidates LIST_DIRECTORIES true "${_repo_root}/.tools/arm-gnu-toolchain-*")
    if(_toolchain_candidates)
        list(SORT _toolchain_candidates)
        foreach(_toolchain_candidate IN LISTS _toolchain_candidates)
            if(IS_DIRECTORY "${_toolchain_candidate}")
                set(_toolchain_root "${_toolchain_candidate}")
            endif()
        endforeach()
    endif()
endif()

if(_toolchain_root AND EXISTS "${_toolchain_root}/bin")
    set(_toolchain_bin_dir "${_toolchain_root}/bin")
endif()

find_program(ARM_NONE_EABI_GCC
    NAMES arm-none-eabi-gcc
    HINTS "${_toolchain_bin_dir}"
    REQUIRED
)

find_program(ARM_NONE_EABI_OBJCOPY
    NAMES arm-none-eabi-objcopy
    HINTS "${_toolchain_bin_dir}"
    REQUIRED
)

find_program(ARM_NONE_EABI_SIZE
    NAMES arm-none-eabi-size
    HINTS "${_toolchain_bin_dir}"
    REQUIRED
)

set(CMAKE_C_COMPILER "${ARM_NONE_EABI_GCC}" CACHE FILEPATH "ARM GCC C compiler" FORCE)
set(CMAKE_ASM_COMPILER "${ARM_NONE_EABI_GCC}" CACHE FILEPATH "ARM GCC assembler" FORCE)
set(CMAKE_OBJCOPY "${ARM_NONE_EABI_OBJCOPY}" CACHE FILEPATH "ARM GCC objcopy" FORCE)
set(CMAKE_SIZE "${ARM_NONE_EABI_SIZE}" CACHE FILEPATH "ARM GCC size" FORCE)

set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)
