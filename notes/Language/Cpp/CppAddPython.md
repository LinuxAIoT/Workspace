# 1.

```cpp
首先#inlcude "Python.h"
Py_Initialize();//对python进行初始化，无返回值。使用py_IsInitialized检查系统是否初始化成功。
PyRun_SimpleString("import sys");//sys模块包含了与Python解释器和它的环境有关的函数。
PyRun_SimpleString("sys.path.append('./')");//添加模块路径，其中PyRun_SimpleString表示执行一段Python代码返回0表示成功，-1表示有错。
PyObject *pModule= PyUnicode_FromFormat("new");//载入python脚本
pFunc= PyObject_GetAttrString(pModule, "add");   //这里是要调用的函数名
if(!pFunc|| !PyCallable_Check(pFunc))   {printf("can't find function [add]");getchar();return -1;}//检查是否找到该函数
-------------------------------------------------------------------------------------
创建参数:  
PyObject *pArgs = PyTuple_New(2); //函数调用的参数传递均是以元组的形式打包的,2表示参数个数  
PyTuple_SetItem(pArgs, 0, Py_BuildValue("i", 5));//0---序号  i表示创建int型变量  
PyTuple_SetItem(pArgs, 1, Py_BuildValue("f", 2.5));//1---序号  f表示创建float型变量
//常用的格式有s 表示字符串，i 表示整型变量，f 表示浮点数，O 表示一个Python对象。
PyObject *pDict = PyDict_New();   //创建字典类型变量
PyDict_SetItemString(pDict,"Name", Py_BuildValue("s", "ww"));//往字典类型变量中填充数据  
PyDict_SetItemString(pDict,"Age", Py_BuildValue("i", 25)); //往字典类型变量中填充数据  
PyTuple_SetItem(pArgs, 0, pDict);//0---序号  将字典类型变量添加到参数元组中
--------------------------------------------------------------------------------------
//调用Python函数及其参数
PyObject_CallObject(pFunc, pArgs);//无返回值时
PyObject *pReturn = NULL; //当需要返回值时 
pReturn = PyEval_CallObject(pFunc, pArgs); //调用函数  
int result;     PyArg_Parse(pReturn, "i", &result); //i表示转换成int型变量  
cout << "pFunc=" << result << endl;  
-----------------------------------------------------------------------------------
以上两部分也可用PyRun_SimpleString直接执行python函数并传入参数如下：
PyRun_SimpleString("import new");//此方法无返回值，因为PyRun_SimpleString只返回0或1
PyRun_SimpleString("new.add(3,4)");
PyRun_SimpleString("new.foo(4)");
 
Py_Finalize();// 关闭Python

```

```c
cmake_minimum_required( VERSION 2.8 )
project(CtoPY)
IF(NOT CMAKE_BUILD_TYPE)
SET(CMAKE_BUILD_TYPE Release)
ENDIF()
set( CMAKE_CXX_FLAGS "-std=c++11" )
 
include_directories(/usr/include/python3.4)#添加头文件目录，相当于g++ -I参数
link_directories(/usr/lib/python3.4)#动态链接库或静态链接库的搜索路径，相当于gcc的-L参数
add_executable(python python.cc)#生成可执行文件
target_link_libraries(python python3.4m)#添加链接库,相同于指定gcc -l参数
```
## 1.1执行
直接在命令行运行C++代码： g++ python.cpp -o python -I/usr/include/python3.4 -L/usr/lib/python3.4 -lpython3.4m，其中-I(大写i)编译程序按照-I指定的路进去搜索头文件;-L参数跟着的是库文件所在的目录名;-l(小写L)参数就是用来指定程序要链接的库，后面是python的库名;编译程序到系统默认路进搜索，如果找不到，到当前目录，如果当前目录找不到，则到LD_LIBRARY_PATH等环境变量置顶的路进去查找，如果还找不到，那么编译程序提示找不到库。


# 2.
```cpp
#include <Python.h>
int main(int argc, char *argv[]) {
    Py_Initialize();

    //判断初始化是否成功
    if(!Py_IsInitialized())
    {
        printf("Python init failed!\n");
        return -1;
    }
    PyRun_SimpleString("import sys");
    //PyRun_SimpleString("sys.path.append('./')");//若python文件在c++工程下
    // 添加python文件所在的位置
    PyRun_SimpleString("sys.path.append('/home/jinye/projects/D3Feat')");
    PyObject* pModule = NULL;
    PyObject* pFunc = NULL;

	//导入python文件
    pModule = PyImport_ImportModule("forcpp");
    if (!pModule) {
        printf("Can not open python file!\n");
        return -1;
    }

    pFunc = PyObject_GetAttrString(pModule, "printHello");
    //PyEval_CallObject(pFunc, NULL);
    PyObject_CallObject(pFunc, NULL);

    Py_DECREF(pFunc);
    Py_DECREF(pModule);
    Py_Finalize();
    return 0;
}
```
# 2.1 CMakeLists.txt

```cpp
cmake_minimum_required(VERSION 2.8)
project(python_test)
include_directories(
    /home/jinye/anaconda3/envs/tensorflow-gpu/include/python3.7m
)
add_executable(test2
    test2.cpp)
target_link_libraries(test2
    /home/jinye/anaconda3/envs/tensorflow-gpu/lib/libpython3.7m.so
)

```
```py
import tensorflow as tf
import sys

def printHello():
    print(sys.version)
    print('tf -V: ', tf.__version__)
    print("hello world!")
```
报错：

Could not find platform independent libraries <prefix>
Could not find platform dependent libraries <exec_prefix>
Consider setting $PYTHONHOME to <prefix>[:<exec_prefix>]
Fatal Python error: initfsencoding: Unable to get the locale encoding
ModuleNotFoundError: No module named 'encodings'

Current thread 0x00007f63049dc700 (most recent call first):
The program has unexpectedly finished.
The process was ended forcefully.

配置项目时设置可以设置CMAKE_PREFIX_PATH变量为虚拟环境的根目录:
```c
cmake .. "-DCMAKE_PREFIX_PATH=/path/to/anaconda/envs/env_name"
```

# 3.

```cpp

#include "python/Python.h"
#include <iostream>
using namespace std;

int main()
{
    Py_Initialize();    // 初始化

    // 将Python工作路径切换到待调用模块所在目录，一定要保证路径名的正确性
    string path = "~/test";
    string chdir_cmd = string("sys.path.append(\"") + path + "\")";
    const char* cstr_cmd = chdir_cmd.c_str();
    PyRun_SimpleString("import sys");
    PyRun_SimpleString(cstr_cmd);

    // 加载模块
    PyObject* moduleName = PyString_FromString("test"); //模块名，不是文件名
    PyObject* pModule = PyImport_Import(moduleName);
    if (!pModule) // 加载模块失败
    {
        cout << "[ERROR] Python get module failed." << endl;
        return 0;
    }
    cout << "[INFO] Python get module succeed." << endl;

    // 加载函数
    PyObject* pv = PyObject_GetAttrString(pModule, "test_add");
    if (!pv || !PyCallable_Check(pv))  // 验证是否加载成功
    {
        cout << "[ERROR] Can't find funftion (test_add)" << endl;
        return 0;
    }
    cout << "[INFO] Get function (test_add) succeed." << endl;

    // 设置参数
    PyObject* args = PyTuple_New(2);   // 2个参数
    PyObject* arg1 = PyInt_FromLong(4);    // 参数一设为4
    PyObject* arg2 = PyInt_FromLong(3);    // 参数二设为3
    PyTuple_SetItem(args, 0, arg1);
    PyTuple_SetItem(args, 1, arg2);

    // 调用函数
    PyObject* pRet = PyObject_CallObject(pv, args);

    // 获取参数
    if (pRet)  // 验证是否调用成功
    {
        long result = PyInt_AsLong(pRet);
        cout << "result:" << result;
    }

    Py_Finalize();      ## 释放资源

    return 0;
}
```
## 3.1C++ 向 Python 传递参数
Python 的参数实际上是元组，因此传参实际上就是构造一个合适的元组：
1. 使用 PyTuple_New 创建元组， PyTuple_SetItem 设置元组值

```c
PyObject* args = PyTuple_New(3);
PyObject* arg1 = Py_BuildValue("i", 100); // 整数参数
PyObject* arg2 = Py_BuildValue("f", 3.14); // 浮点数参数
PyObject* arg3 = Py_BuildValue("s", "hello"); // 字符串参数
PyTuple_SetItem(args, 0, arg1);
PyTuple_SetItem(args, 1, arg2);
PyTuple_SetItem(args, 2, arg3);
```
2. 直接使用Py_BuildValue构造元组

```cpp
PyObject* args = Py_BuildValue("(ifs)", 100, 3.14, "hello");
PyObject* args = Py_BuildValue("()"); // 无参函数
```
[参数类型：](https://docs.python.org/release/1.5.2p2/ext/buildValue.html)

## 4.C++对Pythonlist的操作函数
```cpp
#ifndef Py_LISTOBJECT_H
#define Py_LISTOBJECT_H
#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    PyObject_VAR_HEAD
    PyObject **ob_item;
    Py_ssize_t allocated;
} PyListObject;

PyAPI_DATA(PyTypeObject) PyList_Type;

#define PyList_Check(op) \
    PyType_FastSubclass(Py_TYPE(op), Py_TPFLAGS_LIST_SUBCLASS)
#define PyList_CheckExact(op) (Py_TYPE(op) == &PyList_Type)

PyAPI_FUNC(PyObject *) PyList_New(Py_ssize_t size);
PyAPI_FUNC(Py_ssize_t) PyList_Size(PyObject *);
PyAPI_FUNC(PyObject *) PyList_GetItem(PyObject *, Py_ssize_t);
PyAPI_FUNC(int) PyList_SetItem(PyObject *, Py_ssize_t, PyObject *);
PyAPI_FUNC(int) PyList_Insert(PyObject *, Py_ssize_t, PyObject *);
PyAPI_FUNC(int) PyList_Append(PyObject *, PyObject *);
PyAPI_FUNC(PyObject *) PyList_GetSlice(PyObject *, Py_ssize_t, Py_ssize_t);
PyAPI_FUNC(int) PyList_SetSlice(PyObject *, Py_ssize_t, Py_ssize_t, PyObject *);
PyAPI_FUNC(int) PyList_Sort(PyObject *);
PyAPI_FUNC(int) PyList_Reverse(PyObject *);
PyAPI_FUNC(PyObject *) PyList_AsTuple(PyObject *);
PyAPI_FUNC(PyObject *) _PyList_Extend(PyListObject *, PyObject *);

#define PyList_GET_ITEM(op, i) (((PyListObject *)(op))->ob_item[i])
#define PyList_SET_ITEM(op, i, v) (((PyListObject *)(op))->ob_item[i] = (v))
#define PyList_GET_SIZE(op)    Py_SIZE(op)

#ifdef __cplusplus
}
#endif
#endif /* !Py_LISTOBJECT_H */
```