[TOC]
## 1.lambda表达式:
一个lambda表达式表示一个可调用的代码单元,可以将其理解为一个未命名的内联函数.
lambda表达的格式如下:

**`[capture-lists](parameter-lists)->return type{function body}`**
capture-lists:捕获列表,可以用来捕获lambda表达式所在函数中定义的局部变量;<font color=blue>可以为空,不可以省略;</font>
parameter-lists:参数列表,同普通函数;<font color=blue>可以为空,也可以省略;</font>
return type:返回类型,一般可以省略,在lambda不能自动推算指定类型时,需要指定返回类型;<font color=blue>可以省略;</font>
function body:函数体,同一般函数体.

所以最简单的lambda表达式如下:
**`[capture-lists]{function body}`**

```cpp
int a=100;

//在此处调用一个可调用对象fun,接收一个参数,返回一个int类型值(此处->int 可以省略)
auto fun=[](int a)->int {   
  return a;
};//此处有';'

cout<<fun(a)<<endl;//lambda的调用方式和普通函数的调用方式相同,都是使用调用运算符;
```
### 1.1向lambda表达式传递参数
与普通函数传递形参的方式是相同的,调用一个lambda表达式时给定的`实参`被用来初始化lambda的`形参`,**<font color=blue>实参和形参类型要相同,但是,lambda表达式不能使用默认参数</font>**
### 1.2 使用捕获列表
虽然一个lambda表示可以出现在一个函数中,使用其局部变量,**但它只能使用那些明确指明的变量,一个lambda表达式通过将局部变量包含在其捕获列表中来指出将会使用的这些变量,捕获列表指引lambda表达式在其内部包含访问局部变量所需要的信息**;
```cpp
#include <iostream>
#include <functional>
using namespace std;
int main()
{
    int a = 100;
    //a是一个局部变量,按理说在其生命周期内,在{}是可以访问的,但是在lambda表达式是不允许的,
    //所以需要通过捕获列表将a传到lambda表示式中;
    {
        cout << a << endl; //合法
    }
    // auto func = []
    // {
    //     cout << a << endl; //非法
    // };

    //需要捕获a,值传递(复制),在{}中不可修改a的值;
    auto func = [a] {cout << a << endl;}; //合法
    func();
    //引用传递,在{}中可修改a的值
    auto func1 = [&a] {cout << a << endl;}; //合法
    func1();
    //或者通过参数列表
    auto func2 = [](int a) {cout << a << endl;}; //合法
    func2(a);
    return 0;
}
```
### 1.3 捕获形式:
1. 值捕获(显示):即采用实参复制到形参,**采用值捕获的前提是变量可以拷贝,与参数不同的是,被捕获的变量的值是在lambda创建时拷贝,而不是调用时拷贝**.值捕获参考上面代码.
2. 引用捕获(显示):同一般引用;
3. 隐式捕获(隐式):
   *  \[=]:采用值捕获所有变量;
   *  \[&]:采用引用捕获所有变量;
4. 隐式+显示:**混合使用隐式与显示捕获时,捕获列表中的第一个元素必须是一个=或&**
   
| 捕获格式|含义 |
|------------|-----------|
| [ ] |空捕获列表.lambda不会使用所在函数中的变量.一个lambda**只有捕获变量后才能使用他们**|
|[names]|names是一个用**逗号**分隔的名字列表,这些名字都是lambda表达式所在函数的**局部变量**.默认情况下,捕获列表中的变量都是被拷贝,名字前如果使用了&则采用引用捕获的方式|
|[&]|隐式捕获列表采用**引用捕获方式**.lambda表达式中所使用的来自所在函数的实体都采用引用方式|
|[=]|隐式捕获列表,采用**值捕获方式**,lambda体将拷贝所使用的来自所在函数的实体的值|
|[&,identifier_list]|identifier_list是一个逗号分隔开的列表,包含0个或多个来自所在函数的变量.这些变量采用值捕获方式,而任何隐藏式捕获的变量都采用引用方式捕获.identifier_list的**名字前面不能在使用**`&`|
|[=,identifier_list]|identifier_list的变量都采用引用方式捕获,而任何隐式捕获的变量都采用值捕获.identifier_list中的名字不能包含`this`,且这些名字前面必须使用`&`|

```cpp
//a,前面不能使用&
auto func3=[&,a](int x){cout<<a<<" "<<b<<endl;};
func3(b);

//a,前面必须使用&
auto func3=[=,&a](int x){cout<<a<<" "<<b<<endl;};
func3(b);
```
### 1.4 修改捕获值 mutable:
默认情况下,对于一个**值被拷贝**的变量,lambda不会改变其值.如果我们希望能改变一个被捕获的变量的值,就必须在参数的列表首加上关键字`mutable`
```cpp
//值被拷贝不能直接改变a的值
int a=100;
auto func4=[a](int b) mutable{
        a++;
        cout<<"a= "<<a<<" b="<<b<<endl;//
    };
    func4(b);//输出a=101
                        
    cout<<"[] :"<<a<<endl;//输出100
                            //由于采用的拷贝构造,所以原来的值不会被改变;
```
### 1.5 指定返回类型:
一般情况下,lambda表达式会推断出返回值,但是在某些时候,lambda表达式推断的类型与返回类型不符,此时就需要指定返回类型.默认情况下,如果一个lambda的体包含return之外的任何语句,则编译器假定此lambda返回void,与其他返回void的函数类似,**被推断返回void的lambda不能返回值**
```cpp
transform(vi.begin(),vi.end(),vi.begin(),[](int i){return i>0 ? i: -i;});
```
函数transform接受三个迭代器和一个可调用对象,前两个迭代器表示输入序列,第三个迭代器表示目的位置.算法对输入序列中每个元素调用对象,并将结果写到目的位置.如本例中,目的位置迭代器与表示输入序列开始位置的迭代器可以是相同的.当输入迭代器和目的迭代器相同时,transform将输入序列中每一个元素替换为可调用对象操作该元素得到的结果;
在本例中,我们传递个transform一个lambda,它返回其参数的绝对值.lambda体是单一的return语句,返回一个条件表达式的结果,我们无需指定返回类型,因为可以根据条件运算符的类型推断出来.
如果将上式等价转换:
```cpp
transform(vi.begin(),vi.end(),vi.begin(),[](int i){if(i<0)return -i;else return i;});;//编译出错;(在linux没有报错?)
```
**编译器推断这个版本lambda返回类型为void,但他返回一个int;**
所以我们需要指定返回类型
```cpp
transform(vi.begin(),vi.end(),vi.begin(),[](int i)->int {if(i<0)return -i;else return i;});
```
### 1.6 lambda主要使用场合:
1. 简单函数
2. lambda表达式作为回调函数;
```cpp
#include <iostream>
#include <array>
#include <algorithm>

int main()
{
    int x = 8;
    std::array<int, 999> arr;
    for (int i =1; i< 1000; i++)
    {
        arr[i] = i;
    }
    int cnt = std::count_if(arr.begin(), arr.end(), [x](int a){ return a%x == 0;});
    std::cout << "cnt=" << cnt << std::endl;
    return 0;
}

```
 这里很明显，我们指定了一个lambda表达式来作为一个条件，更多时候，是使用排序函数的时候，指定排序准则，也可以使用lambda表达式。

---
>此部分内容来自知乎:https://zhuanlan.zhihu.com/p/373745326

lambda表达式是C++11中引入的一项新技术，利用lambda表达式可以编写内嵌的匿名函数，用以替换独立函数或者函数对象，并且使代码更可读。

所谓函数对象(**仿函数**)，其实就是对operator()进行重载进而产生的一种行为，比如，我们可以在类中，重载函数调用运算符(),此时类对象就可以直接类似函数一样，直接使用()来传递参数，这种行为就叫做函数对象，同样的，它也叫做**仿函数**。

#### 1. lambda表达式基本使用

lambda表达式基本语法如下：

`[ 捕获 ] ( 形参 ) -> ret { 函数体 };`

lambda表达式一般都是以方括号[]开头，有参数就使用()，无参就`直接省略()`即可，最后结束于{}，其中的`ret表示返回类型`。

我们先看一个简单的例子，定义一个可以输出字符串的lambda表达式，完整的代码如下：
```cpp
#include <iostream>

int main()
{
    auto atLambda = [] {std::cout << "hello world" << std::endl;};
    atLambda();
    return 0;
}

```
上面定义了一个最简单的lambda表达式，没有参数。如果需要参数，那么就要像函数那样，放在圆括号里面，如果有返回值，返回类型则要放在->后面，也就是尾随返回类型，当然你也可以忽略返回类型，lambda会帮你自动推导出返回类型，下面看一个较为复杂的例子：
```cpp
#include <iostream>

int main()
{
    auto print = [](int s) {std::cout << "value is " << s << std::endl;};
    auto lambAdd = [](int a, int b) ->int { return a + b;};
    int iSum = lambAdd(10, 11);
    print(iSum);

    return 0;
}

```
lambAdd有两个入参a和b，然后它的返回类型是int，我们可以试一下把->int去掉，结果是一样的。

#### 2 lambda捕获块
##### 2.1 捕获的简单使用

在第1节中，我们展示了lambda的语法形式，后面的形参和函数体之类都好理解，那么方括号里面捕获是啥意思呢？

其实这里涉及到lambda表达式一个重要的概念，就是闭包。

这里我们需要先对lambda表达式的实现原理做一下说明：当我们定义一个lambda表达式后，编译器会自动生成一个匿名类，这个类里面会默认实现一个public类型的operator()函数，我们称为闭包类型。那么在运行时，这个lambda表达式就会返回一个匿名的闭包实例，它是一个右值。

所以，我们上面的lambda表达式的结果就是一个一个的闭包。闭包的一个强大之处是可以通过传值或者引用的方式捕获其封装作用域内的变量，前面的方括号就是用来定义捕获模式以及变量，所以我们把方括号[]括起来的部分称为捕获块。

看这个例子：
```cpp
#include <iostream>

int main()
{
    int x = 10;
    auto print = [](int s) {std::cout << "value is " << s << std::endl;};
    auto lambAdd = [x](int a) { return a + x;};
    auto lambAdd2 = [&x](int a, int b) { return a + b + x;};
    auto iSum = lambAdd(10);
    auto iSum2 = lambAdd2(10, 11);
    print(iSum);
    print(iSum2);

    return 0;
}

```
当lambda块为空时，表示没有捕获任何变量，不为空时，比如上面的lambAdd是以复制的形式捕获变量x，而lambAdd2是以引用的方式捕获x。那么这个复制或者引用到底是怎么体现的呢，我们使用gdb看一下lambAdd和lambAdd2的具体类型，如下：
```cpp
(gdb) ptype lambAdd
type = struct <lambda(int)> {
    int __x;
}
(gdb) ptype lambAdd2
type = struct <lambda(int, int)> {
    int &__x;
}
(gdb)
```
前面我们说过lambda实际上是一个类，这里得到了证明，在c++中struct和class除了有少许区别，其他都是一样的，所以我们可以看到复制形式捕获实际上是一个包含int类型成员变量的struct，引用形式捕获实际上是一个包含int&类型成员变量的struct，然后在运行的时候，会使用我们捕获的数据来初始化成员变量。

既然有初始化，那么必然有构造函数啊，然后捕获生成的成员变量，有operator()函数，暂时来讲，一个比较立体的闭包类型就存在于我们脑海中啦，对于lambda表达式类型具体组成，我们暂时放一放，接着说捕获。

捕获的方式可以是引用也可以是复制，但是到底有哪些类型的捕获呢？

捕获类型如下：

  *  [ ]：默认不捕获任何变量；
  *  \[=]：默认以复制捕获**所有变量**；
  *  \[&]：默认以引用捕获**所有变量**；
  *  [x]：仅以复制捕获x，其它变量不捕获；
  *  [x...]：以包展开方式复制捕获参数包变量；
  *  [&x]：仅以引用捕获x，其它变量不捕获；
  *  [&x...]：以包展开方式引用捕获参数包变量；
  *  [=, &x]：默认以复制捕获所有变量，但是x是例外，通过**必须引用**捕获；
  *  [&, x]：默认以引用捕获所有变量，但是x是例外，通过复制捕获；
  *  [this]：通过引用捕获当前对象（其实是复制指针）；
  *  [*this]：通过复制方式捕获当前对象；

可以看到，lambda是可以有多个捕获的，每个捕获之间以逗号分隔，另外呢，不管多少种捕获类型，万变不离其宗，要么以复制方式捕获，要么以引用方式捕获。

那么复制捕获和引用捕获到底有什么区别呢？

标准c++规定，默认情况下，在lambda表达式中，对于operator()的重载是const属性的，也就意味着如果以复制形式捕获的变量，是不允许修改的，看这段代码：
```cpp
#include <iostream>

int main()
{
    int x = 10;
    int y = 20;
    auto print = [](int s) {std::cout << "value is " << s << std::endl;};
    auto lambAdd = [x](int a) { 
    //  x++;  此处x是只读，不允许自增，编译会报错
        return a + x;
    };
    auto lambAdd2 = [&x](int a, int b) { 
        x = x+5;
        return a + b + x;
    };
    auto iSum = lambAdd(10);
    auto iSum2 = lambAdd2(10, 11);
    print(iSum);
    print(iSum2);

    return 0;
}

```
从代码可以看出，**复制捕获不允许修改变量值**，而**引用捕获则允许修改变量值**，为什么呢，这里我理解，&x实际上是一个int*类型的指针，所以我们可以修改x的值，因为我们只是对这个指针所指向的内容进行修改，并没有对指针本身进行修改，且与我们常规声明的引用类型入参一样，修改的值在lambda表达式外也是有效的。

**解决方法:那么如果我想使用复制捕获，又想修改变量的值呢**，这时我们就想起来有个关键字，叫做``mutable``，它允许在常成员函数中修改成员变量的值，所以我们可以给lambda表达式指定mutable关键字，如下：

```cpp
#include <iostream>

int main()
{
    int x = 10;
    int y = 20;
    auto print = [](int s) {std::cout << "value is " << s << std::endl;};
    auto lambAdd = [x](int a) mutable { 
        x++;
        return a + x;
    };
    auto iSum = lambAdd(10);
    print(iSum);
    print(x);

    return 0;
}

```
所以加上mutable以后就可以对复制捕获进行修改，但有一点，**它的修改出了lambda表达式以后就无效了**。
#### 2.4 捕获的作用

我再看lambda的捕获的时候一直很奇怪，初看的话，这个捕获跟传参数有什么区别呢，都是把一个变量值传入lambda表达式体供使用，但仔细思考的话，它是有作用的，假设有这么一个案例，一个公司有999名员工，每个员工的工号是从1~999，我们现在想找出工号是8的整数倍的所有员工，一个可行的代码如下：
```cpp
#include <iostream>
#include <array>

int main()
{
    int x = 8;
    auto t = [x](int i){
        if ( i % x == 0 )
        {
            std::cout << "value is " << i << std::endl;
        }
    };
    auto t2 = [](int i, int x){
        if ( i % x == 0 )
        {
            std::cout << "value is " << i << std::endl;
        }
    };
    for(int j = 1; j< 1000; j++)
    {
        t(j);
        t2(j, x);
    }
    return 0;
}
```
表达式t使用了捕获，而表达式t2没有使用捕获，从代码作用和量来看，它们其实区别不大，但有一点，对于表达式t，x的值只复制了一次，而对于t2表达式，每次调用都要生成一个临时变量来存放x的值，这其实是多了时间和空间的开销，不过，对于这段代码而言，这点消耗可以忽略不计呢，但一旦数据上了规模，那就会有比较大的区别了?

对于捕获，还是**尽量不要使用[=]或者[&]**这样全捕获的形式，因为不可控，你不能确保哪些变量会被捕获，容易发生一些不测的行为。


#### 4. lambda表达式赋值

lambda表达式既然生成了一个类对象，那么它是否可以像普通类对象那样，进行赋值呢？

我们写一段代码试一下：
```cpp
#include <iostream>
using namespace std;

int main()
{
    auto a = [] { cout << "A" << endl; };
    auto b = [] { cout << "B" << endl; };

    //a = b; // 非法，lambda无法赋值
    auto c(a); // 合法，生成一个副本
    auto d=b;  // 合法，生成一个副本
    
    a();
    b();
    c();
    d();
    return 0;
}
```
很显然赋值不可以，而拷贝则可以，结合编译器自动生成构造函数规则，很明显，赋值函数被禁用了，而拷贝构造函数则没有被禁用，所以不能用一个lambda表达式给另外一个赋值，但可以进行初始化拷贝。
---

## 2.正则表达式:
正则表达式(regular expression)描述了一种字符串匹配的模式（pattern），可以用来检查一个串是否含有某种子串、将匹配的子串替换或者从某个串中取出符合某个条件的子串等。
推荐使用该[网站](https://goregex.cn/):包含常见的格式:
