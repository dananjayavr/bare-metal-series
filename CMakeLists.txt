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

        # For reference, following files have copies of their own in each execution target
        #BSP/Core/Inc/stm32f4xx_hal_conf.h
        #BSP/Core/Inc/stm32f4xx_it.h
        #BSP/Core/Src/stm32f4xx_it.c
        #BSP/Core/Src/system_stm32f4xx.c
        #BSP/Core/Inc/main.h
        #BSP/Core/Src/main.c
        #BSP/Core/Src/stm32f4xx_hal_msp.c
        #BSP/startup_stm32f401xe.s
)

# To disable compiler warnings, etc. for certain files (3rd party files mainly)
set_source_files_properties(
        BSP/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_exti.c
        BSP/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_flash_ex.c
        BSP/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_flash.c
        BSP/Drivers/STM32F4xx_HAL_Driver/Src/stm32f4xx_hal_pwr.c
        PROPERTIES
        COMPILE_OPTIONS  "-Wno-unused-parameter;-Wno-redundant-decls"
)

# Settings for app
set(EXECUTABLE_APP ${PROJECT_NAME}-app.out)

add_executable(${EXECUTABLE_APP} ${STM32CUBEMX_GENERATED_FILES}
        source/app/retarget.c
        source/app/application.c
        source/app/firmware.c

        source/app/stm32f4xx_hal_conf.h
        source/app/stm32f4xx_it.h
        source/app/stm32f4xx_it.c
        source/app/system_stm32f4xx.c

        source/app/startup_stm32f401xe.s

        source/app/bootloader_code.c
)

target_compile_definitions(${EXECUTABLE_APP} PRIVATE
        -DUSE_HAL_DRIVER
        -DSTM32F401xE
)

target_include_directories(${EXECUTABLE_APP} SYSTEM PRIVATE # adding system will also discard warnings from header files
        #BSP/Core/Inc # replicated elsewhere
        BSP/Drivers/STM32F4xx_HAL_Driver/Inc
        BSP/Drivers/CMSIS/Device/ST/STM32F4xx/Include
        BSP/Drivers/CMSIS/Include
)

target_include_directories(${EXECUTABLE_APP} PRIVATE
        source/app
)

target_compile_options(${EXECUTABLE_APP} PRIVATE
        -mcpu=cortex-m4
        -mthumb
        -mfpu=fpv4-sp-d16
        -mfloat-abi=hard

        -fdata-sections
        -ffunction-sections

        -Wall
        -Wextra
        -pedantic

        -Wshadow
        -Wimplicit-function-declaration
        -Wredundant-decls
        -Wmissing-prototypes
        -Wstrict-prototypes
        -fno-common
        -ffunction-sections
        -fdata-sections


        #-fno-exceptions # C++ specific flag
        #-fno-rtti # C++ specific flag
        $<$<COMPILE_LANGUAGE:CXX>:@${CMAKE_SOURCE_DIR}/gcc-options-cxx.txt> # add C++ options (including the two above)

        $<$<CONFIG:Debug>:-Og> #  If CONFIG is Debug, then add option -Og, otherwise add nothing
        )

target_link_options(${EXECUTABLE_APP} PRIVATE
        -T${CMAKE_SOURCE_DIR}/source/app/STM32F401RETx_FLASH.ld
        -mcpu=cortex-m4
        -mthumb
        -mfpu=fpv4-sp-d16
        -mfloat-abi=hard
        -specs=nano.specs
        -specs=nosys.specs # added to fix _sbrk not found issue
        -lc
        -lm
        -lnosys
        -Wl,-Map=${PROJECT_NAME}-app.map,--cref
        -Wl,--gc-sections
        )

# Print executable size
add_custom_command(TARGET ${EXECUTABLE_APP}
        POST_BUILD
        COMMAND arm-none-eabi-size ${EXECUTABLE_APP})

# Create hex file
add_custom_command(TARGET ${EXECUTABLE_APP}
        POST_BUILD
        COMMAND arm-none-eabi-objcopy -O ihex ${EXECUTABLE_APP} ${PROJECT_NAME}-app.hex
        COMMAND arm-none-eabi-objcopy -O binary ${EXECUTABLE_APP} ${PROJECT_NAME}-app.bin
        COMMAND arm-none-eabi-objcopy -O binary ${EXECUTABLE_APP} ${PROJECT_NAME}-app.elf
)

# Improve clean target
set_target_properties(${EXECUTABLE_APP} PROPERTIES ADDITIONAL_CLEAN_FILES
        "${PROJECT_NAME}-app.bin;${PROJECT_NAME}-app.hex;${PROJECT_NAME}-app.map")

##############################################################################################################
##############################################################################################################
##############################################################################################################

# Settings for bootloader
set(EXECUTABLE_BOOTLOADER ${PROJECT_NAME}-bootloader.out)

add_executable(${EXECUTABLE_BOOTLOADER} ${STM32CUBEMX_GENERATED_FILES}
        source/bootloader/firmware.c
        source/bootloader/firmware.h
        source/bootloader/retarget.c
        source/bootloader/retarget.h

        source/bootloader/stm32f4xx_hal_conf.h
        source/bootloader/stm32f4xx_it.h
        source/bootloader/stm32f4xx_it.c
        source/bootloader/system_stm32f4xx.c

        source/bootloader/startup_stm32f401xe.s
)

target_compile_definitions(${EXECUTABLE_BOOTLOADER} PRIVATE
        -DUSE_HAL_DRIVER
        -DSTM32F401xE
)

target_include_directories(${EXECUTABLE_BOOTLOADER} SYSTEM PRIVATE # adding system will also discard warnings from header files
        #BSP/Core/Inc # replicated elsewhere
        BSP/Drivers/STM32F4xx_HAL_Driver/Inc
        BSP/Drivers/CMSIS/Device/ST/STM32F4xx/Include
        BSP/Drivers/CMSIS/Include
)

target_include_directories(${EXECUTABLE_BOOTLOADER} PRIVATE
        source/bootloader
)

target_compile_options(${EXECUTABLE_BOOTLOADER} PRIVATE
        -mcpu=cortex-m4
        -mthumb
        -mfpu=fpv4-sp-d16
        -mfloat-abi=hard

        -fdata-sections
        -ffunction-sections

        -Wall
        -Wextra
        -pedantic

        -Wshadow
        -Wimplicit-function-declaration
        -Wredundant-decls
        -Wmissing-prototypes
        -Wstrict-prototypes
        -fno-common
        -ffunction-sections
        -fdata-sections

        #-fno-exceptions # C++ specific flag
        #-fno-rtti # C++ specific flag
        $<$<COMPILE_LANGUAGE:CXX>:@${CMAKE_SOURCE_DIR}/gcc-options-cxx.txt> # add C++ options (including the two above)

        $<$<CONFIG:Debug>:-Og> #  If CONFIG is Debug, then add option -Og, otherwise add nothing
)

target_link_options(${EXECUTABLE_BOOTLOADER} PRIVATE
        -T${CMAKE_SOURCE_DIR}/source/bootloader/STM32F401RETx_FLASH.ld
        -mcpu=cortex-m4
        -mthumb
        -mfpu=fpv4-sp-d16
        -mfloat-abi=hard
        -specs=nano.specs
        -specs=nosys.specs # added to fix _sbrk not found issue
        -lc
        -lm
        -lnosys
        -Wl,-Map=${PROJECT_NAME}-bootloader.map,--cref
        -Wl,--gc-sections
)

# Print executable size
add_custom_command(TARGET ${EXECUTABLE_BOOTLOADER}
        POST_BUILD
        COMMAND arm-none-eabi-size ${EXECUTABLE_BOOTLOADER})

# Create hex file
add_custom_command(TARGET ${EXECUTABLE_BOOTLOADER}
        POST_BUILD
        COMMAND arm-none-eabi-objcopy -O ihex ${EXECUTABLE_BOOTLOADER} ${PROJECT_NAME}-bootloader.hex
        COMMAND arm-none-eabi-objcopy -O binary ${EXECUTABLE_BOOTLOADER} ${PROJECT_NAME}-bootloader.bin
        COMMAND arm-none-eabi-objcopy -O binary ${EXECUTABLE_BOOTLOADER} ${PROJECT_NAME}-bootloader.elf
)

# Run padding script
add_custom_command(TARGET ${EXECUTABLE_BOOTLOADER}
        POST_BUILD
        COMMAND python ../source/pad-bootloader.py
        COMMENT "Add padding to bootloader.bin"
)

# Improve clean target
set_target_properties(${EXECUTABLE_BOOTLOADER} PROPERTIES ADDITIONAL_CLEAN_FILES
        "${PROJECT_NAME}-bootloader.bin;${PROJECT_NAME}-bootloader.hex;${PROJECT_NAME}-bootloader.map")