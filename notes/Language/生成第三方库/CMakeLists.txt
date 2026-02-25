cmake_minimum_required(VERSION 3.0)
project(addlib)
add_executable(test main.cpp)

add_library(mylib SHARED MyHead.cpp) ## 将MyHead.cpp打包生成共享库,即.so
target_link_libraries(test mylib) ##将可执行程序链接到该库上,此时main的myprint()函数才能执行
