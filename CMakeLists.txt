cmake_minimum_required(VERSION 3.13)

project(belmoor
        VERSION 0.1.0.0
        DESCRIPTION ""
        LANGUAGES CXX C ASM)


#find_package(Protobuf REQUIRED)


set(CMAKE_EXPORT_COMPILE_COMMANDS On)
#set(CMAKE_CXX_CLANG_TIDY clang-tidy -p ${PROJECT_BINARY_DIR})
#set(CMAKE_CXX_CPPCHECK cppcheck)


if (NOT (${CMAKE_SYSTEM_PROCESSOR} STREQUAL ${CMAKE_HOST_SYSTEM_PROCESSOR}))

    add_compile_options(-flto -ffunction-sections -fdata-sections -Os)


    add_subdirectory(lib/stm32f072c8tx)
    add_subdirectory(lib/stm32-ssd1306)

endif ()


add_compile_options(
        -Werror -Wall -Wextra -pedantic -pedantic-errors
        -Wreturn-type -Wreturn-local-addr -Wswitch
        -Wconversion -Wsign-conversion)


add_executable(belmoor)

target_compile_features(belmoor PUBLIC
        cxx_std_20)

if (${CMAKE_SYSTEM_PROCESSOR} STREQUAL ${CMAKE_HOST_SYSTEM_PROCESSOR})

    target_sources(belmoor PRIVATE
            src/test.cpp
            src/m90e26_test.cpp
            src/pack_test.cpp
            src/relay_test.cpp
            src/spi_test.cpp
            src/usb.cpp
            src/usb_test.cpp)

    target_include_directories(belmoor PRIVATE
            src/fake
            lib/stm32f072c8tx/Core/Inc)

else ()

    set_target_properties(belmoor PROPERTIES OUTPUT_NAME "belmoor.elf")

    target_sources(belmoor PRIVATE
            src/app.cpp
            src/display.hpp
            src/m90e26.hpp
            src/nvm.cpp
            src/nvm.hpp
            src/pack.hpp
            src/print.hpp
            src/relay.hpp
            src/spi.hpp
            src/sys_tick.hpp
            src/usb.cpp
            src/usb.hpp
            src/utils.hpp)

    target_link_libraries(belmoor PRIVATE
            stm32f072c8tx ssd1306)

    target_link_options(belmoor PRIVATE
            LINKER:--gc-sections
            LINKER:-print-memory-usage
            --specs=nosys.specs --specs=nano.specs)

endif ()
