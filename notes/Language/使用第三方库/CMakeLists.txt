cmake_minimum_required(VERSION 3.0)
project(testlib)

#设置第三库的头文件路径与库文件路径
set(INCLUDE_DIRECTORIES ${PROJECT_SOURCE_DIR}/include)
set(LIBRARY_PATH_MY ${PROJECT_SOURCE_DIR}/lib)

message("incude dir:" ${INCLUDE_DIRECTORIES})
message("lib dir:" ${LIBRARY_PATH_MY})

#+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
#添加库

#对于find_package找不到的外部依赖库，可以用add_library添加
# SHARED表示添加的是动态库==>(共享库)
# STATIC表示添加的是静态库
# IMPORTED表示是引入已经存在的动态库
add_library(mylib STATIC IMPORTED)

#指定所添加依赖库的导入路径
set_target_properties(mylib PROPERTIES IMPORTED_LOCATION ${LIBRARY_PATH_MY}/libmylib.a)

# 添加头文件路径到编译器的头文件搜索路径下，多个路径以空格分隔
include_directories( ${INCLUDE_DIRECTORIES} )

# 添加库文件路径到编译器的库文件搜索路径下，多个路径以空格分隔
link_directories(${LIBRARY_PATH_MY})
#+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

add_executable(${PROJECT_NAME} main.cpp)
target_link_libraries(${PROJECT_NAME} )


#+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
##链接库
target_link_libraries(${PROJECT_NAME} mylib)
#+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++