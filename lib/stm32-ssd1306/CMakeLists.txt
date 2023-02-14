add_library(ssd1306 OBJECT)

target_sources(ssd1306 PRIVATE
        ssd1306/ssd1306.c
        ssd1306/ssd1306_fonts.c
        ssd1306/ssd1306_tests.c)

target_include_directories(ssd1306 SYSTEM INTERFACE ssd1306/ PRIVATE ssd1306/)

target_link_libraries(ssd1306 PRIVATE stm32f072c8tx)
