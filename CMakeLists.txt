cmake_minimum_required(VERSION 3.15.3)

project(bare-metal-series)

# C stuff
enable_language(C ASM)
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

# C++ stuff
#enable_language(CXX C ASM)
#set(CMAKE_CXX_STANDARD 17)
#set(CMAKE_CXX_STANDARD_REQUIRED ON)
#set(CMAKE_CXX_EXTENSIONS OFF)

find_program(CCACHE_PROGRAM ccache)

if (CCACHE_PROGRAM)
        message(NOTICE "ccache is enabled (found here: ${CCACHE_PROGRAM})")
        set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE "\"${CCACHE_PROGRAM}\"")
        set_property(GLOBAL PROPERTY RULE_LAUNCH_LINK "\"${CCACHE_PROGRAM}\"")
else ()
        message(FATAL_ERROR "ccache has not been found")
endif ()

set(STM32CUBEMX_GENERATED_FILES

        BSP/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_tim.c
        BSP/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_tim_ex.c
        BSP/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_uart.c
        BSP/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_rcc.c
        BSP/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_rcc_ex.c
        BSP/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_flash.c
        BSP/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_flash_ex.c
        BSP/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_flash_ramfunc.c
        BSP/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_gpio.c
        BSP/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_dma_ex.c
        BSP/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_dma.c
        BSP/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_pwr.c
        BSP/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_pwr_ex.c
        BSP/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_cortex.c
        BSP/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal.c
        BSP/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_exti.c
        BSP/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_pcd.c
        BSP/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_pcd_ex.c
        BSP/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_ll_usb.c

        BSP/Core/Inc/main.h
        BSP/Core/Inc/stm32f4xx_hal_conf.h
        BSP/Core/Inc/stm32f4xx_it.h

        BSP/Core/Src/main.c
        BSP/Core/Src/stm32f4xx_hal_msp.c
        BSP/Core/Src/stm32f4xx_it.c
        BSP/Core/Src/system_stm32f4xx.c
        BSP/startup_stm32f401xe.s

)

# To disable compiler warnings, etc. for certain files (3rd party files mainly)
set_source_files_properties(
        BSP/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_exti.c
        BSP/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_flash_ex.c
        BSP/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_pwr.c
        PROPERTIES
        COMPILE_FLAGS -Wno-unused-parameter
)

set(EXECUTABLE ${PROJECT_NAME}.out)

add_executable(${EXECUTABLE} ${STM32CUBEMX_GENERATED_FILES}
        source/retarget.c
        source/application.c
)

target_compile_definitions(${EXECUTABLE} PRIVATE
        -DUSE_HAL_DRIVER
        -DSTM32F401xE
)

target_include_directories(${EXECUTABLE} SYSTEM PRIVATE # adding system will also discard warnings from header files
        BSP/Core/Inc
        BSP/Drivers/STM32F4xx_HAL_Driver/Inc
        BSP/Drivers/CMSIS/Device/ST/STM32F4xx/Include
        BSP/Drivers/CMSIS/Include
)

target_include_directories(${EXECUTABLE} PRIVATE
        source
)

target_compile_options(${EXECUTABLE} PRIVATE
        -mcpu=cortex-m4
        -mthumb
        -mfpu=fpv4-sp-d16
        -mfloat-abi=hard

        -fdata-sections
        -ffunction-sections

        -Wall
        -Wextra
        -pedantic

        #-fno-exceptions # C++ specific flag
        #-fno-rtti # C++ specific flag
        $<$<COMPILE_LANGUAGE:CXX>:@${CMAKE_SOURCE_DIR}/gcc-options-cxx.txt> # add C++ options (including the two above)

        $<$<CONFIG:Debug>:-Og> #  If CONFIG is Debug, then add option -Og, otherwise add nothing
        )

target_link_options(${EXECUTABLE} PRIVATE
        -T${CMAKE_SOURCE_DIR}/BSP/STM32F401RETx_FLASH.ld
        -mcpu=cortex-m4
        -mthumb
        -mfpu=fpv4-sp-d16
        -mfloat-abi=hard
        -specs=nano.specs
        -specs=nosys.specs # added to fix _sbrk not found issue
        -lc
        -lm
        -lnosys
        -Wl,-Map=${PROJECT_NAME}.map,--cref
        -Wl,--gc-sections
        )

# Print executable size
add_custom_command(TARGET ${EXECUTABLE}
        POST_BUILD
        COMMAND arm-none-eabi-size ${EXECUTABLE})

# Create hex file
add_custom_command(TARGET ${EXECUTABLE}
        POST_BUILD
        COMMAND arm-none-eabi-objcopy -O ihex ${EXECUTABLE} ${PROJECT_NAME}.hex
        COMMAND arm-none-eabi-objcopy -O binary ${EXECUTABLE} ${PROJECT_NAME}.bin
)

# Improve clean target
set_target_properties(${EXECUTABLE} PROPERTIES ADDITIONAL_CLEAN_FILES
        "${PROJECT_NAME}.bin;${PROJECT_NAME}.hex;${PROJECT_NAME}.map")