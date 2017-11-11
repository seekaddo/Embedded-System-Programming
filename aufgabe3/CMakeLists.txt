cmake_minimum_required(VERSION 3.8)
project(Lab101)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS}  -Wall  -Wstrict-prototypes  \
        -pedantic -Wwrite-strings -fno-common -O3 -g -std=gnu11 ")

set(SOURCE_FILES main.c tm4c1294ncpdt_startup_ccs.c)
#add_executable(Lab101 main.c tm4c1294ncpdt_startup_ccs.c)

add_executable(Lab101 ${SOURCE_FILES})
include_directories(/Users/addodennis/ti/TivaWare_C_Series-2.1.4.178)
