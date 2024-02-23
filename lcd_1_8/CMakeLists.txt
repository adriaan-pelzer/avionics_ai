aux_source_directory(. DIR_lcd_1_8_SRCS)

add_subdirectory(lib/Config)
add_subdirectory(lib/LCD)
add_subdirectory(lib/Fonts)
add_subdirectory(lib/GUI)

include_directories(./lib/Config)
include_directories(./lib/LCD)
include_directories(./lib/Fonts)
include_directories(./lib/GUI)

add_library(lcd_1_8 ${DIR_lcd_1_8_SRCS})
target_link_libraries(lcd_1_8 PUBLIC LCD GUI Fonts Config pico_stdlib hardware_spi)
