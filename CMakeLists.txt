cmake_minimum_required(VERSION 2.6.4)
project(CFS_CS C)

include_directories(fsw/mission_inc)
include_directories(fsw/platform_inc)

aux_source_directory(fsw/src APP_SRC_FILES)

# Create the app module
add_cfe_app(cs ${APP_SRC_FILES})

include_directories(fsw/src)
aux_source_directory(fsw/tables APP_TABLE_FILES)
add_cfe_tables(cs ${APP_TABLE_FILES})
