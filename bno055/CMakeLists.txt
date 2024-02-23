aux_source_directory(. DIR_BNO055_SRCS)

add_library(bno055 ${DIR_BNO055_SRCS})
target_link_libraries(bno055 PUBLIC hardware_i2c pico_stdlib)
