[TOC]
## 0.最小cpp系统构成
最小cpp系统构成,只需要一个`CMakeLists.txt`文件和一个`main.cpp`;
```cpp
//CMakeLists.txt
cmake_minimum_required(VERSION 3.0)
project(addlib)
add_executable(test main.cpp)

//所以最小cpp系统只需要三句cmake,如果需要添加自定义的头文件,和库文件,则需要将其包含在add_executable();此部分见##1.

```

## 1.不使用用自定义库文件
在Linux中需要使用CMake进行系统文件的构建,如果不生成自定库文件时,**需要将所有源文件都添加到cmake的add_executable()中**,否则编译出错,只能执行`main.cpp`,调用其他自定义的`.h`和`.cpp`中的函数,都会出现编译错误;例如,现有一下源文件:
```cpp

//main.cpp 主程序
#include<iostream>
#include"MyHead.h"
using namespace std;
int main()
{
    myprint("head test");
    return 0;
}
//Myhead.hpp
#ifndef MYHEAD_H
#define MYHEAD_H
#include<string>
#include<iostream>
void myprint(std::string str);
#endif
//Myhead.cpp
#include"MyHead.h"
void myprint(std::string str)
{
    std::cout<<"this my head test"<<std::endl;
    std::cout<<"content is :"<<str<<std::endl;
}
```
此时需要在将新添加的`Myhead.hpp`与`MyHead.cpp`,需要在`CMakeList.txt`文件中`add_executable()`包含进去,告诉编译器,有两个个可执行文件,分别为`main.cpp`,`MyHead.cpp`;否则,编译无法知道`Myhead.hpp`中的`void myprint(std::string str);`是如何执行的,从而导致编译出错;
**所以此时的`CMakeList.txt`如下:**
```cpp
cmake_minimum_required(VERSION 3.0)
project(addlib)
add_executable(test main.cpp MyHead.cpp) ## 将MyHead.cpp也添加为可执行文件
                                         ## main.cpp 中的myprint()是从MyHead.cpp中解析;
```

## 2.使用库文件
通常我们使用的第三方库都是非开源的,我们只要获得第三方库的**库文件**和**头文件**,就可以使用;而库文件,就是将`.cpp`文件转换成二进制文件,所以此时,我们是无法知道头文件中的某个函数是如何定义,即这个函数的实现方法,**即所有库都是一些函数打包的集合**;为了使用这个库,我们需要知道这个库里面有什么东西,所以此时需要一本`说明`,而这个说明就是`头文件`.所以为了使用第三方库,我们至少需要两个文件**库文件**和**头文件**,本节目的就是实现**生成一个库文件和使用一个库文件**;
在Linux中库文件分为**静态库**和**共享库**;此处我们只讨论**linux下的静态库与共享库**;

静态库是以.a作为后缀名,共享库是.so为后缀的,**所有库都是一些函数打包的集合,差别在于静态库每次调用都会生成一个副本,而共享库则只生成一个副本.**

## 2.1 生成静态库
我们可以将自定义的源程序生成**库文件**,此时,我们就可以不用将这个源程序添加到`CMakeLists.txt`中的`add_executable()`,为了使用这个库文件(即`MyHead.cpp`生成的二进制代码),我们需要在`CMakeList.txt`中新添加一句命令用来将这个库文件和我们的`main.cpp`链接起来;
我们只需要改变`CMakeLists.txt`为:
```cpp
cmake_minimum_required(VERSION 3.0)
project(addlib)
add_executable(test main.cpp)

add_library(mylib MyHead.cpp) ## 将MyHead.cpp打包生成静态库,即生成.a文件
target_link_libraries(test mylib) ## 将可执行程序链接到该库上,此时main的myprint()函数才能执行
                                  ## 此时该函数是从生成的.a文件中解析myprint() 
```
## 2.2 生成共享库
生成共享库的方法和静态库的方法类似,只需要在`add_library() `添加关键字`SHARED`:
```cpp
cmake_minimum_required(VERSION 3.0)
project(addlib)
add_executable(test main.cpp)

add_library(mylib SHARED MyHead.cpp) ## 将MyHead.cpp打包生成共享库,即.so
target_link_libraries(test mylib) ##将可执行程序链接到该库上,此时main的myprint()函数才能执行
```

## 3.使用第三库
由前面知道,为了使用第三库,我们需要获取其库文件的和头文件,此处,我们使用自己生成的库文件和头文件;
为了使用第三方库,我们需要指定第三库库文件与头文件对应的位置,最后将源文件与库文件进行链接
文件结构:
├── CMakeLists.txt
├── include
│   └── MyHead.h
├── lib
│   ├── libmylib.a
│   └── libmylib.so
└── main.cpp

## 3.1 使用第三库-共享链接库的使用
```cpp
//CMakeLists.txt
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
add_library(mylib SHARED IMPORTED)

#指定所添加依赖库的导入路径
set_target_properties(mylib PROPERTIES IMPORTED_LOCATION ${LIBRARY_PATH_MY}/libmylib.so)

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
//main.cpp
#include<iostream>
#include<MyHead.h>//#include"MyHead.h"两者都可以
using namespace std;
int main()
{
    myprint("test my link library");
    return 0;
}
```

## 3.2 使用第三库-静态库的使用

静态库的添加与动态库相同,只需要将下面两条语句进行修改:
```cpp
add_library(mylib SHARED IMPORTED)==>add_library(mylib STATIC IMPORTED) ##修改关键字:SHARED==>STATIC
set_target_properties(mylib PROPERTIES IMPORTED_LOCATION ${LIBRARY_PATH_MY}/libmylib.so)==>
==> set_target_properties(mylib PROPERTIES IMPORTED_LOCATION ${LIBRARY_PATH_MY}/libmylib.a) ## 指向静态库

```

```cpp
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
# IMPORTED表示是引入已经存在的静态库
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
```