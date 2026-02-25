[TOC]
# 1 方法1
## 1.1 背景
随着机器学习/深度学习这几年的的火热，python成了当红炸子鸡，使用python训练机器学习模型则成了开发人员们最喜欢的方法，但是由于过往调度系统一般都是用C++来开发的，因此我们只有两种方法来调用python脚本，一种是使用[上篇](https://zhuanlan.zhihu.com/p/80637788)中提到的子进程的方法，另外一种则是直接使用C++/python进行混合编程。
## 1.2 基本使用方法
python 提供了一套 C API库，使得开发者能很方便地从C/ C++ 程序中调用 Python 模块。具体的文档参考[官方指南](https://link.zhihu.com/?target=https%3A//docs.python.org/2/extending/embedding.html)

至于要使用这个套API，我们需要引入一个头文件和一个库文件，以linux系统自带的python 2.7为例，python 的头文件位于/usr/include/python2.7，库文件为 libpython2.7.so ，在makefile中需要增加
```c
 -I /usr/include/python2.7 -l python2.7
```
我们现在知道了python api的头文件和库文件了，那这些接口是怎样用的呢？下面我们按照不同的主题来拆解下。
## 1.2.1 初始化Python解释器环境
python是一门解释语言，没有解释器是没法运行的。因此，我们需要为python提供一个解释器环境，和环境相关的接口如下
```cpp
void Py_Initialize()：       
    初始化python解释器.C/C++中调用Python之前必须先初始化解释器
int Py_IsInitialized()：
    返回python解析器的是否已经初始化完成，如果已完成，返回大于0，否则返回0
void Py_Finalize() ：
    撤销Py_Initialize()和随后使用Python/C API函数进行的所有初始化，
    并销毁自上次调用Py_Initialize()以来创建并为被销毁的所有子解释器。
```
### 1.1.2 调用Python脚本
在C++中要使用要调用python脚本，python api提供了几种方式，下面拿两个api为例，具体的参考官方文档[这里](https://docs.python.org/2/c-api/veryhigh.html#c.PyRun_SimpleString)和[这里](https://docs.python.org/2/c-api/veryhigh.html#c.PyRun_SimpleFile)
```cpp
int PyRun_SimpleString(const char*) :
    执行一个简单的执行python脚本命令的函数

int PyRun_SimpleFile(FILE *fp, const char *filename)：
    从fp中把python脚本的内容读取到内容中并执行，filename应该为fp对应的文件名
```
举个简单的例子
```cpp
#include "python2.7/Python.h"

int main()
{
    Py_Initialize();    ## 初始化

    PyRun_SimpleString("print 'hello'");

    Py_Finalize();      ## 释放资源
}

```
在上面例子中，我们把python代码当作一个字符串传给解释器来执行，这咋一看貌似没有什么问题，但是，既然我们选择了使用python，却把python脚本写到C++代码里面，这似乎有点买椟还珠。同时，不管是字符串还是文件，都只能用于c++不需要像python传参，同时python不会向c++返回值的情况，只执行固定脚本的场景，但是，实际的场景中是必然存在C++向python传参，python返回结果的，这个需求下我们要怎样做呢？别急，我们一步步来说。
**动态加载python模块并执行函数**
先来说说这个问题的解法吧。python api提供了动态加载模块并且执行函数的能力，具体会涉及到下面几个api
```cpp
//加载模块
PyObject* PyImport_ImportModule(char *name)

PyObject* PyImport_Import(PyObject *name)
PyObject* PyString_FromString(const char*)
    上面两个api都是用来动态加载python模块的。区别在于前者一个使用的是C的字符串，而后者的name是一个python对象，
这个python对象需要通过PyString_FromString(const char*)来生成，其值为要导入的模块名


//导入函数相关
PyObject* PyModule_GetDict( PyObject *module)
    PyModule_GetDict()函数可以获得Python模块中的函数列表。PyModule_GetDict()函数返回一个字典。字典中的关键字为函数名，值为函数的调用地址。
字典里面的值可以通过PyDict_GetItemString()函数来获取，其中p是PyModule_GetDict()的字典，而key则是对应的函数名

PyObject* PyObject_GetAttrString(PyObject *o, char *attr_name)
     PyObject_GetAttrString()返回模块对象中的attr_name属性或函数，相当于Python中表达式语句：o.attr_name

//调用函数相关
PyObject* PyObject_CallObject( PyObject *callable_object, PyObject *args)
PyObject* PyObject_CallFunction( PyObject *callable_object, char *format, ...)
    使用上面两个函数可以在C程序中调用Python中的函数。callable_object为要调用的函数对象，也就是通过上述导入函数得到的函数对象，
而区别在于前者使用python的tuple来传参，后者则使用类似c语言printf的风格进行传参。
如果不需要参数，那么args可能为NULL。返回成功时调用的结果，或失败时返回NULL。
这相当于Python表达式 apply(callable_object, args) 或 callable_object(*args)
```
api知道了，但是怎样用起来呢？我们先来看一个简单的例子。假设我们有这样一个python脚本
```py
#cat script/sayHello.py
def say():
	print("hello")
```
我们可以像下面这样去加载模块并且调用指定的函数
```cpp
#include <python2.7/Python.h>
#include <iostream>

using namespace std;

int main(){
	Py_Initialize();
	if( !Py_IsInitialized()){
		cout << "python init fail" << endl;
		return 0;
	}
	PyRun_SimpleString("import sys");
	PyRun_SimpleString("sys.path.append('./script')");

	PyObject* pModule = PyImport_ImportModule("sayHello");
	if( pModule == NULL ){
		cout <<"module not found" << endl;
		return 1;
	}

	PyObject* pFunc = PyObject_GetAttrString(pModule, "say");
	if( !pFunc || !PyCallable_Check(pFunc)){
		cout <<"not found function add_num" << endl;
		return 0;
	}

	 PyObject_CallObject(pFunc, NULL );

	Py_Finalize();
	return 0;
}
```
但是，同学们应该看到，上面的函数是没有参数而且没有返回值的，如果有参数和返回值要怎样做呢？
**调用参数**
在C/C++中，所有的Python类型都被声明为PyObject型，为了能够让C++能够操作python的数据，python提供了python各种数据类型和C语言数据类型的转换操作，具体的使用方法如下:
1. **数字与字符串**
在Python/C API中提供了Py_BuildValue()函数对数字和字符串进行转换处理，使之变成Python中相应的数据类型。其函数原型如下所示:
```c
PyObject* Py_BuildValue( const char *format, ...)
    Py_BuildValue()提供了类似c语言printf的参数构造方法，format是要构造的参数的类型列表，函数中剩余的参数即要转换的C语言中的整型、浮点型或者字符串等。
其返回值为PyObject型的指针。
```
format对应的类型列表如下:
```c
s(str或None)[char *]
使用'utf-8'编码将以null结尾的C字符串转换为Python str对象。如果C字符串指针为NULL，则表示None。

s＃(str或None)[char *，int]
使用'utf-8'编码将C字符串及其长度转换为Python str对象。如果C字符串指针为NULL，则忽略长度返回None。

y(字节)[char *]
这会将C字符串转换为Python字节对象。如果C字符串指针为NULL，则返回None。

y＃(字节)[char *，int]
这会将C字符串及其长度转换为Python对象。如果C字符串指针为NULL，则返回None。

z(str或None)[char *]
与s相同。

z＃(str或None)[char *，int]
与s＃相同。

u(str)[Py_UNICODE *]
将Unicode(UCS-2或UCS-4)数据的以null结尾的缓冲区转换为Python Unicode对象。如果Unicode缓冲区指针为NULL，则返回None。

u＃(str)[Py_UNICODE *，int]
将Unicode(UCS-2或UCS-4)数据缓冲区及其长度转换为Python Unicode对象。如果Unicode缓冲区指针为NULL，则忽略长度并返回None。

U(str或None)[char *]
与s相同。

U＃(str或None)[char *，int]
与s＃相同。

i(int)[int]
将普通的C int转换为Python整数对象。

b(int)[char]
将纯C char转换为Python整数对象。

h(int)[short int]
将普通的C short int转换为Python整数对象。

l(int)[long int]
将C long int转换为Python整数对象。

B(int)[unsigned char]
将C unsigned char转换为Python整数对象。

H(int)[unsigned short int]
将C unsigned short int转换为Python整数对象。

I(int)[unsigned int]
将C unsigned int转换为Python整数对象。

k(int)[unsigned long]
将C unsigned long转换为Python整数对象。

L(int)[long long]
将C long long转换为Python整数对象。

K(int)[unsigned long long]
将C unsigned long long转换为Python整数对象。

n(int)[Py_ssize_t]
将C Py_ssize_t转换为Python整数。

c(长度为1的字节)[char]
将表示字节的C int转换为长度为1的Python字节对象。

C(长度为1的str)[int]
将表示字符的C int转换为长度为1的Python str对象。

d(float) [double] 
将C double转换为Python浮点数。

f(float) [float] 
将C float转换为Python浮点数。

D(complex) [Py_complex *]
将C Py_complex结构转换为Python复数。

O(object) [PyObject *]
不改变Python对象的传递(引用计数除外，它增加1)。如果传入的对象是NULL指针，则假定这是因为产生参数的调用发现错误并设置了异常。
因此，Py_BuildValue()将返回NULL但不会引发异常。如果尚未引发异常，则设置SystemError。

S(object) [PyObject *]
与O相同

N((object) [PyObject *]
与O相同，但不会增加对象的引用计数。通过调用参数列表中的对象构造函数创建对象时很有用。

O＆(object) [converter, anything] 
通过转换器函数将任何内容转换为Python对象。该函数被调用任何东西(应与void *兼容)作为其参数，并应返回“新”Python对象，如果发生错误则返回NULL。

(items) (tuple) [matching-items] 
将一系列C值转换为具有相同项目数的Python元组。

[items](list) [matching-items]
将一系列C值转换为具有相同项目数的Python列表。

{items}(dict) [matching-items] 
将一系列C值转换为Python字典。每对连续的C值将一个项添加到字典中，分别用作键和值。
如果格式字符串中存在错误，则设置SystemError异常并返回NULL。
```
2. **列表**
```c
PyObject* PyList_New( Py_ssize_t len)
    创建一个新的Python列表，len为所创建列表的长度

int PyList_SetItem( PyObject *list, Py_ssize_t index, PyObject *item)
    向列表中添加项。当列表创建以后，可以使用PyList_SetItem()函数向列表中添加项。 list：要添加项的列表。 index：所添加项的位置索引。 item：所添加项的值。

PyObject* PyList_GetItem( PyObject *list, Py_ssize_t index)
    获取列表中某项的值。list：要进行操作的列表。index：项的位置索引。

Py_ssize_t PyList_Size(PyObject * list)
    返回列表中列表对象的长度;这相当于列表对象上的 len(list) 。

int PyList_Append( PyObject *list, PyObject *item)
int PyList_Sort( PyObject *list)
int PyList_Reverse( PyObject *list)
    Python/C API中提供了与Python中列表操作相对应的函数。例如
列表的append方法对应于PyList_Append()函数。
列表的sort方法对应于PyList_Sort()函数。
列表的reverse方法对应于PyList_Reverse()函数。

```
3. **元组**
```c
PyObject* PyTuple_New( Py_ssize_t len) 
    PyTuple_New()函数返回所创建的元组。其函数原型如下所示。len：所创建元组的长度。 

int PyTuple_SetItem( PyObject *p, Py_ssize_t pos, PyObject *o) 
    当元组创建以后，可以使用PyTuple_SetItem()函数向元组中添加项。p：所进行操作的元组，pos：所添加项的位置索引，o：所添加的项值。

PyObject* PyTuple_GetItem( PyObject *p, Py_ssize_t pos)
    可以使用Python/C API中PyTuple_GetItem()函数来获取元组中某项的值。p：要进行操作的元组，pos：项的位置索引

Py_ssize_t PyTuple_Size(PyObject * p)
    获取指向元组对象的指针，并返回该元组的大小。

int _PyTuple_Resize( PyObject **p, Py_ssize_t newsize) 
    当元组创建以后可以使用_PyTuple_Resize()函数重新调整元组的大小。其函数原型如下所示。p：指向要进行操作的元组的指针，newsize：新元组的大小
```
4. **字典**
```c
PyObject* PyDict_New() 
    PyDict_New()函数返回所创建的字典。


int PyDict_SetItem( PyObject *p, PyObject *key, PyObject *val) 
int PyDict_SetItemString( PyObject *p, const char *key, PyObject *val) 
    当字典创建后，可以使用PyDict_SetItem()函数和PyDict_SetItemString()函数向字典中添加项。 其参数含义如下。
p：要进行操作的字典。key：添加项的关键字，
对于PyDict_SetItem()函数其为PyObject型，
对于PyDict_SetItemString()函数其为char型，val：添加项的值。 
 

PyObject* PyDict_GetItem( PyObject *p, PyObject *key) 
PyObject* PyDict_GetItemString( PyObject *p, const char *key) 
    使用Python/C API中的PyDict_GetItem()函数和PyDict_GetItemString()函数来获取字典中某项的值。它们都返回项的值。
其参数含义如下。p：要进行操作的字典，key：添加项的关键字，
对于PyDict_GetItem()函数其为PyObject型
对于PyDict_GetItemString()函数其为char型。 

PyObject* PyDict_Items( PyObject *p) 
PyObject* PyDict_Keys( PyObject *p) 
PyObject* PyDict_Values( PyObject *p) 
    在Python/C API中提供了与Python中字典操作相对应的函数。例如
    字典的item方法对应于PyDict_Items()函数。
    字典的keys方法对应于PyDict_Keys()函数。
    字典的values方法对应于PyDict_Values()函数。
其参数p：要进行操作的字典。
```
但是上面所有操作都是基于C语言的，至于像c++的string以及各种容器类型，则需要自己通过封装来实现了。例如下面这样:
```cpp
// PythonParam.h
#ifndef __PYTHON_CPP_PYTHON_PARAM_H__
#define __PYTHON_CPP_PYTHON_PARAM_H__

#include <map>
#include <string>
#include <vector>

#include <python2.7/Python.h>

class PythonParamBuilder{
public:
    PythonParamBuilder();
    ~PythonParamBuilder();
    PyObject* Build();

    bool AddString( const std::string& );
    bool AddList( const std::vector<std::string>& params );
    bool AddMap( const std::map<std::string, std::string>& mapParam);
    template<typename T>
    bool AddCTypeParam( const std::string& typeStr, T v ){
		PyObject * param = Py_BuildValue( typeStr, v );
		return AddPyObject( param );
    }
private:
    bool AddPyObject( PyObject* obj );
    PyObject* ParseMapToPyDict(const std::map<std::string, std::string>& mapParam);
    PyObject* ParseListToPyList(const std::vector<std::string>& params);
    PyObject* ParseStringToPyStr( const std::string& str );
private:
    PyObject* mArgs;
    std::vector<PyObject*> mTupleObjects;
};

#endif


//PythonParam.cpp
#include "PythonParam.h"


PythonParamBuilder::PythonParamBuilder()
:mArgs(nullptr){

}

PythonParamBuilder::~PythonParamBuilder(){
    for ( std::vector<PyObject*>::iterator it = mTupleObjects.begin();
        it != mTupleObjects.end();
	 ++it ){
            if( *it )
		Py_DECREF( *it );
	}

    if ( mArgs )
        Py_DECREF(mArgs);
}

PyObject* PythonParamBuilder::Build(){
    if ( mTupleObjects.empty() )
        return nullptr;

    mArgs = PyTuple_New( mTupleObjects.size() );

    for ( int i=0; i<mTupleObjects.size(); i++){
         PyTuple_SetItem(mArgs, i, mTupleObjects[i] );
    }

    return mArgs;
}

bool PythonParamBuilder::AddPyObject( PyObject* obj ){
     if ( nullptr == obj )
        return false;
     mTupleObjects.push_back(obj);
     return true;
}

bool PythonParamBuilder::AddString( const std::string& str ){
     return AddPyObject( ParseStringToPyStr( str ) );
}

bool PythonParamBuilder::AddList( const std::vector<std::string>& params ){
    return AddPyObject( ParseListToPyList(params) );
}

bool PythonParamBuilder::AddMap( const std::map<std::string, std::string>& mapParam){
    return AddPyObject( ParseMapToPyDict( mapParam ) );
}

PyObject* PythonParamBuilder::ParseMapToPyDict(const std::map<std::string, std::string>& mapParam){
    PyObject* pyDict = PyDict_New();
    for (std::map<std::string, std::string>::const_iterator it = mapParam.begin();
          it != mapParam.end();
          ++it) {
              PyObject* pyValue = PyString_FromString(it->second.c_str());
              if (pyValue == NULL) {
                   printf( "Parse param:[%s] to PyStringObject failed.", it->second.c_str());
		   return NULL;
	      }
	      if (PyDict_SetItemString(pyDict, it->first.c_str(), pyValue) < 0) {
		   printf( "Parse key:[%s] value:[%s] failed.", it->first.c_str(), it->second.c_str());
		   return NULL;
	      }
    } 
    return pyDict;
}

PyObject* PythonParamBuilder::ParseListToPyList(const std::vector<std::string>& params){
    size_t size = params.size();
    PyObject* paramList =  PyList_New(size);
    for (size_t i = 0; i < size; i++) {
	PyObject* pyStr = PyString_FromStringAndSize(params[i].data(), params[i].size());
	if (pyStr == NULL) {
  	    printf( "Parse param:[%s] to PyStringObject failed.", params[i].c_str());
	    break;
	}
	if (PyList_SetItem(paramList, i, pyStr) != 0) {
	    printf( "param:[%s] append to PyParamList failed.", params[i].c_str());
	    break;
	}
    }
    return paramList;
}

PyObject* PythonParamBuilder::ParseStringToPyStr( const std::string& str ){
    return PyString_FromStringAndSize( str.data(), str.size() );
}

```
**返回值**
python函数的返回值也是PyObject类型，因此，在python脚本返回到C/C++之后，需要解构Python数据为C的类型，这样C/C++程序中才可以使用Python里的数据。但是，由于python的返回值有多种数据结构类型，因此，我们需要为每个类型进行转换。不过由于篇幅问题，我们只是介绍简单的整形和字符串类型的处理，其他类型的返回见文末的github链接，总体思路都是根据类型逐个从值从PyObject中提取。python提供了下面函数来完成这个功能:
```c
int PyArg_Parse( PyObject *args, char *format, ...)
     根据format把args的值转换成c类型的值，format接受的类型和上述Py_BuildValue()的是一样的
```
**释放资源**
Python使用引用计数机制对内存进行管理，实现自动垃圾回收。在C/C++中使用Python对象时，应正确地处理引用计数，否则容易导致内存泄漏。在Python/C API中提供了Py_CLEAR()、Py_DECREF()等宏来对引用计数进行操作。
当使用Python/C API中的函数创建列表、元组、字典等后，就在内存中生成了这些对象的引用计数。在对其完成操作后应该使用Py_CLEAR()、Py_DECREF()等宏来销毁这些对象。其原型分别如下所示:
```cpp
void Py_CLEAR(PyObject *o)
void Py_DECREF(PyObject *o)
其中，o的含义是要进行操作的对象。
对于Py_CLEAR()其参数可以为NULL指针，此时，Py_CLEAR()不进行任何操作。而对于Py_DECREF()其参数不能为NULL指针，否则将导致错误。
```
好了，把各个细节都说了一遍了，下面是另外一个简单的例子，在这个例子中，我们会有输入参数和返回值:
```py
/*
cat script/Py2Cpp.py

def add_num(a,b):
	return a+b

*/
```


```cpp

#include <python2.7/Python.h>
#include <iostream>

using namespace std;

int main(){
	Py_Initialize();
	if( !Py_IsInitialized()){
		cout << "python init fail" << endl;
		return 0;
	}
	PyRun_SimpleString("import sys");
	PyRun_SimpleString("sys.path.append('./script')");


	PyObject* moduleName = PyString_FromString("Py2Cpp");
	PyObject* pModule = PyImport_Import(moduleName);
	if( pModule == NULL ){
		cout <<"module not found" << endl;
		return 1;
	}

	PyObject* pFunc = PyObject_GetAttrString(pModule, "add_num");
	if( !pFunc || !PyCallable_Check(pFunc)){
		cout <<"not found function add_num" << endl;
		return 0;
	}

	PyObject* args = Py_BuildValue("(ii)", 28, 103);
	PyObject* pRet = PyObject_CallObject(pFunc, args );
        Py_DECREF(args);

	int res = 0;
	PyArg_Parse(pRet, "i", &res );
        Py_DECREF(pRet);
	cout << res << endl;

	Py_Finalize();
	return 0;
}

```
C++调用python的过程中，好多地方都需要用到资源的释放，包括解释器的创建和销毁，资源的计数减少等等，这些可以借助C++的RAII来把事情做的更好，具体的封装类见这里。

[Python 2.7.5 中文文档](https://link.zhihu.com/?target=https%3A//www.docs4dev.com/docs/zh/python/2.7.15/all/extending-extending.html%23%25E4%25BB%258Ec%25E8%25B0%2583%25E7%2594%25A8python%25E5%2587%25BD%25E6%2595%25B0
)

# 2.传参Mat
在C++调用python过程中，不避免需要为其传参，上面main函数也有展示，这里有些参考博客；可以看到，传一些数字，字符串等比较简单
但我们常常需要传入图像，如Mat对象，就有点麻烦了；对于不熟悉C++和Python混编的我，差点吐了；最终找到一篇文档，才完美解决，直接写一个转换函数，**将Mat图像转化为PyObject对象**

```cpp
PyObject* convertImage(const cv::Mat& image) {
	//2D image with 3 channels.
	npy_intp dimensions[3] = { image.rows, image.cols, image.channels() };

	//image.dims = 2 for a 2D image, so add another dimension for channels.
	return  PyArray_SimpleNewFromData(image.dims + 1, (npy_intp*)&dimensions, NPY_UINT8, (void *)image.data);
}
```