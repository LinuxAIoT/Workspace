**extern "C"的主要作用就是为了能够正确实现C++代码调用其他C语言代码。加上extern "C"后，会指示编译器这部分代码按C语言（而不是C++）的方式进行编译。** 由于C++支持函数重载，因此编译器编译函数的过程中会将函数的参数类型也加到编译后的代码中，而不仅仅是函数名；而C语言并不支持函数重载，因此编译C语言代码的函数时不会带上函数的参数类型，一般只包括函数名。
这个功能十分有用处，因为在C++出现以前，很多代码都是C语言写的，而且很底层的库也是C语言写的，为了更好的支持原来的C代码和已经写好的C语言库，需要在C++中尽可能的支持C，而extern "C"就是其中的一个策略。
>C++ 支持函数的重载
C++ 支持函数的重载，是因为 C++ 会在程序的编译阶段对函数的函数名进行“再次重命名”，例如：
```cpp
void Swap(int a, int b) //会被重命名为_Swap_int_int；
void Swap(float x, float y) //会被重命名为_Swap_float_float。
```
显然通过重命名，可以有效避免编译器在程序链接阶段无法找到对应的函数。
但是，**C 语言是不支持函数重载的**，它不会在编译阶段对函数的名称做较大的改动。仍以 `void Swap(int a, int b) `和 `void Swap(float x, float y) `为例，若以 C 语言的标准对它们进行编译，两个函数的函数名将都是`_Swap`。 
**这个功能主要用在下面的情况：**
* C++代码调用C语言代码
* 在C++的**头文件中**使用
* 在多个人协同开发时，可能有的人比较擅长C语言，而有的人擅长C++，这样的情况下也会有用到
> 方式1：
```cpp
//moduleA.h
//只针对c部分进行声明
#pragma once
#ifndef _MODULEA_
#define _MODULEA_
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus
//其他代码...
void display();
#ifdef __cplusplus
}
#endif // __cplusplus
#endif // !_MODULEA_

//单句链接指示
//#ifdef __cplusplus
//extern "C" void display(); 
//#else 
//void display();
//#endif // __cplusplus
```
```cpp
//moduleA.c
#include "moduleA.h"
void display()
{
	printf("c code ;\n");
}

//main.cpp
#include<iostream>
#include "moduleA.h"
using namespace std;
int main()
{
	display();
	system("pause");
}
```
> 方式2：
```cpp
//moduleA.h 
//c中不做任何变化，引用是调用extern "C"
#pragma once
#ifndef _MODULEA_
#define _MODULEA_
#include <stdio.h>
void display();
#endif // !_MODULEA_

//moduleA.c
#include "moduleA.h"
void display()
{
	printf("c code ;\n");
}

//main.cpp
#include<iostream>
extern "C" {            
#include "moduleA.h"    //1.应用于整个头文件，头文件中的所有普通函数的声明都会被认为是由链接指示的语言进行编写；
                        //2.链接指示可以嵌套，因此如果头文件包含自带链接指示的函数，则该函数的链接指示是不会改变的；
}
using namespace std;
int main()
{
	display();
	system("pause");
}
```
通过extern +""的方式称为链接指示，c++不仅可以通过与c通过链接指示进行混合编程，还可以与其他语言进行不过需要两者之间的编译器相互兼容。