cmake_minimum_required(VERSION 3.29)
project(AutoComplete_master)

set(CMAKE_CXX_STANDARD 14)

include_directories(.)

add_executable(AutoComplete_master
    DictionaryTrie.cpp
    DictionaryTrie.hpp)
