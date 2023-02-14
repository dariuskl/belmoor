add_library(stm32f072c8tx OBJECT)

target_compile_definitions(stm32f072c8tx PUBLIC STM32F072xB)

target_compile_options(stm32f072c8tx
    PUBLIC
        -mcpu=cortex-m0 -mthumb)

target_sources(stm32f072c8tx
    PRIVATE
        startup_stm32f072xb.s
        Core/Src/gpio.c
        Core/Src/i2c.c
        Core/Src/main.c
        Core/Src/stm32f0xx_hal_msp.c
        Core/Src/stm32f0xx_it.c
        Core/Src/system_stm32f0xx.c
        Core/Src/tim.c
        Core/Src/usart.c
        Drivers/STM32F0xx_HAL_Driver/Src/stm32f0xx_hal.c
        Drivers/STM32F0xx_HAL_Driver/Src/stm32f0xx_hal_cortex.c
        Drivers/STM32F0xx_HAL_Driver/Src/stm32f0xx_hal_dma.c
        Drivers/STM32F0xx_HAL_Driver/Src/stm32f0xx_hal_exti.c
        Drivers/STM32F0xx_HAL_Driver/Src/stm32f0xx_hal_flash.c
        Drivers/STM32F0xx_HAL_Driver/Src/stm32f0xx_hal_flash_ex.c
        Drivers/STM32F0xx_HAL_Driver/Src/stm32f0xx_hal_gpio.c
        Drivers/STM32F0xx_HAL_Driver/Src/stm32f0xx_hal_i2c.c
        Drivers/STM32F0xx_HAL_Driver/Src/stm32f0xx_hal_i2c_ex.c
        Drivers/STM32F0xx_HAL_Driver/Src/stm32f0xx_hal_pcd.c
        Drivers/STM32F0xx_HAL_Driver/Src/stm32f0xx_hal_pcd_ex.c
        Drivers/STM32F0xx_HAL_Driver/Src/stm32f0xx_hal_pwr.c
        Drivers/STM32F0xx_HAL_Driver/Src/stm32f0xx_hal_pwr_ex.c
        Drivers/STM32F0xx_HAL_Driver/Src/stm32f0xx_hal_rcc.c
        Drivers/STM32F0xx_HAL_Driver/Src/stm32f0xx_hal_rcc_ex.c
        Drivers/STM32F0xx_HAL_Driver/Src/stm32f0xx_hal_tim.c
        Drivers/STM32F0xx_HAL_Driver/Src/stm32f0xx_hal_tim_ex.c
        Drivers/STM32F0xx_HAL_Driver/Src/stm32f0xx_hal_uart.c
        Drivers/STM32F0xx_HAL_Driver/Src/stm32f0xx_hal_uart_ex.c
        Drivers/STM32F0xx_HAL_Driver/Src/stm32f0xx_ll_usb.c
        Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Src/usbd_cdc.c
        Middlewares/ST/STM32_USB_Device_Library/Core/Src/usbd_core.c
        Middlewares/ST/STM32_USB_Device_Library/Core/Src/usbd_ctlreq.c
        Middlewares/ST/STM32_USB_Device_Library/Core/Src/usbd_ioreq.c
        USB_DEVICE/App/usb_device.c
        USB_DEVICE/App/usbd_cdc_if.c
        USB_DEVICE/App/usbd_desc.c
        USB_DEVICE/Target/usbd_conf.c)

target_include_directories(stm32f072c8tx
    SYSTEM INTERFACE
        Core/Inc/
        Drivers/CMSIS/Core/Include
        Drivers/CMSIS/Device/ST/STM32F0xx/Include
        Drivers/STM32F0xx_HAL_Driver/Inc
        Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc
        Middlewares/ST/STM32_USB_Device_Library/Core/Inc
        USB_DEVICE/App/
        USB_DEVICE/Target/
    PRIVATE
        Core/Inc/
        Drivers/CMSIS/Core/Include
        Drivers/CMSIS/Device/ST/STM32F0xx/Include
        Drivers/STM32F0xx_HAL_Driver/Inc
        Middlewares/ST/STM32_USB_Device_Library/Class/CDC/Inc
        Middlewares/ST/STM32_USB_Device_Library/Core/Inc
        USB_DEVICE/App/
        USB_DEVICE/Target/)

target_link_options(stm32f072c8tx
    PUBLIC
        -T${CMAKE_CURRENT_SOURCE_DIR}/STM32F072C8Tx_FLASH.ld
        -mcpu=cortex-m0 -mthumb)
