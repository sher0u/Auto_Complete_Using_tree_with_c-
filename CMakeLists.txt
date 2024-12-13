cmake_minimum_required(VERSION 3.29)
project(Autocompleter_in_c_master C)

set(CMAKE_C_STANDARD 11)

include_directories(.)

add_executable(Autocompleter_in_c_master
    autocompleter.c
    autocompleter.h
    main.c
    utils.c
    utils.h
)
