[TOC]
> exit和abort函数，其功能都是终止程序的执行，将流程控制返回给操作系统。**<font color=blue>通常，exit用于正常终止程序的执行，abort用于异常终止程序的执行。</font>**

## 1.exit 
exit用于正常终止程序的执行。
```cpp
exit(表达式)；
```
其中，表达式的值只能是整型数。通常把表达式的值作为终止程序执行的原因。
执行该函数时，`将无条件地终止程序`的执行而不管该函数处于程序中的什么位置，并将控制返回给操作系统。通常表达式的取值为一个常数：<font colo=blue>用0表示正常退出，用其他整数值作为异常退出的原因</font>

**<font color=blue>当执行exit（）函数时，系统要做终止程序执行前的收尾工作，如关闭该程序打开的文件，释放变量所占用的存储空间（不包含动态分配的存储空间）；</font>**

```cpp
...
    exit(0);
...

//输出：...、cpp\Debug\cpp.exe (进程 4068)已退出，代码为 0。

...
    exit(1);
...

//输出：...\cpp\Debug\cpp.exe (进程 17704)已退出，代码为 1。

...
...
    exit(10);
...
//输出：...\cpp\Debug\cpp.exe (进程 17704)已退出，代码为 10。

```
所以可以设置不同的返回来判断程序执行过程中所出现的问题。

```cpp
#include<iostream>
using namespace std;
int main()
{
	int m = 0;
	cin >> m;
	switch (m)
	{
	case 1:
	{
		cerr << "错误1" << endl;
		exit(1);
		break;
	}
	case 2:
	{
		cerr << "错误2" << endl;
		exit(2);
		break;
	}
	default:
	{
		cout << "正常结束" << endl;
		exit(0);
		break;
	}
		
	}
	return 0;
}
```

## 2.abort()

调用该函数时，括号内不能有任何参数。其作用是向标准错误流（std::cerr)发送程序异常终止的消息，然后终止程序。
**<font color=blue>在执行该函数时，系统不做结束程序前的收尾工作，直接终止程序的执行。</font>**

```cpp
...

abort();
...
```
调用后弹出窗口：
![abort](abort.png)
## 3.conclusion
* 共同点：两者均是终止程序的执行。
* 异同点：
### exit():
1. exit带参，通常用0表示正常退出，其他整型表示异常退出。
2. exit用于正常终止程序的执行，abort用于异常终止程序的执行。
3. exit调用后将无条件地终止程序的执行,并且需要做结束程序前的收尾工作。
### abort():

1. abort不带参数；
2. abort系统不做结束程序前的收尾工作，直接终止程序的执行;
3. abort执行时，会弹出调用abort()被调用的弹窗。
