cmake_minimum_required (VERSION 2.6)

project(mcpelaunchernhc LANGUAGES C)

set(CMAKE_CXX_STANDARD 17)

find_library(log-lib log)


add_library(mcpelaunchernhc SHARED src/main.c)
target_link_libraries(mcpelaunchernhc PUBLIC ${log-lib})
