set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR ARM)
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

set(_tool_root_default "${CMAKE_CURRENT_LIST_DIR}/../.tools")
set(_toolchain_dir "")

if(DEFINED ENV{ARM_GNU_TOOLCHAIN_DIR} AND NOT "$ENV{ARM_GNU_TOOLCHAIN_DIR}" STREQUAL "")
    set(_toolchain_dir "$ENV{ARM_GNU_TOOLCHAIN_DIR}")
else()
    file(GLOB _toolchain_candidates LIST_DIRECTORIES true "${_tool_root_default}/arm-gnu-toolchain-*")
    list(SORT _toolchain_candidates ORDER DESCENDING)
    foreach(_candidate IN LISTS _toolchain_candidates)
        if(IS_DIRECTORY "${_candidate}" AND EXISTS "${_candidate}/bin/arm-none-eabi-gcc")
            set(_toolchain_dir "${_candidate}")
            break()
        endif()
    endforeach()
endif()

if(NOT _toolchain_dir)
    message(FATAL_ERROR
        "ARM GNU Toolchain not found.\n"
        "Run ./scripts/setup-local-toolchain.sh first, or export ARM_GNU_TOOLCHAIN_DIR."
    )
endif()

set(CMAKE_C_COMPILER "${_toolchain_dir}/bin/arm-none-eabi-gcc" CACHE FILEPATH "" FORCE)
set(CMAKE_ASM_COMPILER "${_toolchain_dir}/bin/arm-none-eabi-gcc" CACHE FILEPATH "" FORCE)
set(CMAKE_AR "${_toolchain_dir}/bin/arm-none-eabi-ar" CACHE FILEPATH "" FORCE)
set(CMAKE_OBJCOPY "${_toolchain_dir}/bin/arm-none-eabi-objcopy" CACHE FILEPATH "" FORCE)
set(CMAKE_OBJDUMP "${_toolchain_dir}/bin/arm-none-eabi-objdump" CACHE FILEPATH "" FORCE)
set(CMAKE_RANLIB "${_toolchain_dir}/bin/arm-none-eabi-ranlib" CACHE FILEPATH "" FORCE)
set(CMAKE_SIZE "${_toolchain_dir}/bin/arm-none-eabi-size" CACHE FILEPATH "" FORCE)

set(CMAKE_FIND_ROOT_PATH "${_toolchain_dir}/arm-none-eabi" "${_toolchain_dir}")
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)
