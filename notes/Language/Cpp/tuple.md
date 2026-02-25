[TOC]
## 1.tuple 类型
与pair类型相似,pair类型**只有两个成员**,但是tuple成员可以拥有**任意数量**的成员,可以将tuple看做成一个快速而随意的数据结构.


|实例|说明|
|------------|-----------|
|tuple<T1,T2,T3,...,Tn> t;|t是一个tuple,成员数为n,第i个成员的类型为Ti.所有成员都进行值初始化|
|tuple<T1,T2,T3,...,Tn> t(v1,v2,..,vn);|t是一个tuple,成员的类型为T1...Tn.每个成员用对应的初始值vi进行初始化.此构造函数是explicit类型|
|make_tuple(v1,v2,..,v3)|返回一个用给定初始值初始化的tuple.tuple类型从初始值的类型推断|
|t1==t2|当两个tuple**具有相同数量**的成员且成员对应相等时(包括类型相等),两个tuple相等,否则不相等|
|t1!=t2|当两个tuple**具有相同数量**的成员且成员对应相等时(包括类型相等),两个tuple相等,否则不相等|
|t1 relop t2|tuple的关系运算使用字典序,两个tuple必须具有相同数量的成员.使用<运算符比较t1的成员和t2中的对应成员|
|get<i>(t)|返回t的第i数据成员的**引用**;如果t是一个左值,结果返回一个左值**引用**;否则,结果是一个右值引用.tuple的所有成员都是**public**|

|tuple_size<tupleType>::value|一个类模板,可以通过一个tuple类型来初始化.它有一个名为value的public constexpr static 数据成员,类型size_t,表示给定tuple类型中成员的数量|
|tuple_size<i,tupleType>::type|一个类模板,可以通过一个整型和一个tuple类型来初始化,它有一个名为type的public成员,表示给定tuple类型中指定成员的类型|

## 2.定义与初始化tuple

定义一个tuple需要指出每一个成员的类型;
1. tuple<size_t,size_t,size_t> threeD;//三个成员都设置为0;
2. tuple<string,vector<double>,int,list<int> >someVal("constants",{3.14,2.718},42,{0,1,2,3});
3. tuple<size_t,size_t,size_t> threeD{1,2,3};   由于tuple的所以只能使用直接初始化;

    * tuple<size_t,size_t,size_t> threeD(1,2,3);   //正确
    * tuple<size_t,size_t,size_t> threeD{1,2,3};   //正确

    * tuple<size_t,size_t,size_t> threeD={1,2,3};   //错误

4. make_tuple:函数使用初始值的类型来推断tuple的类型;
   auto item=make_tuple("name:",3,20.00);

## 3.访问tuple的成员->get():返回指定成员的引用
一个pair总是有两个成员(first,second),但是tuple数目没有限制,**所以tuple的成员都是没有命名的;**
```cpp
tuple<size_t,size_t,size_t> tuple_;
     auto item=get<0>(tuple_);  //返回tuple_的第一个成员;
     auto item=get<1>(tuple_);  //返回tuple_的第二个成员;
```
* 获取tuple成员的数量和类型:
```cpp
  tuple<int,int,int> threenum(1,2,3);
    typedef decltype(threenum) trans; //decltype根据threenum的类型推导出类型

    // 返回trans 对象中成员的数量
	size_t sz=tuple_size<trans>::value ;// 3 

    //获取tuple中指定成员的类型

   
```
## 4.tuple_element:返回指定成员的类型
```cpp
tuple_element<1,decltype(str)>::type strt=get<1>(str); 	//strt 是一个string 类型
```

## 5.关系和相等运算
两个具有相同数量的tuple,才可以进行比较;
```cpp
#include <iostream>
#include <map>
#include <tuple>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
using namespace std;
using namespace cv;
int main(int agrc, char **agrv)
{

	tuple<int,int,int> threenum(1,2,3);
	typedef decltype(threenum) trans;
	size_t sz=tuple_size<trans>::value ;

	tuple_element<1,trans>::type cnt=get<1>(threenum); //cnt 是一个int 类型

	tuple<int,string> str (100,"test");

	//str中第二个元素的类型是string,所以strt也是string类型
	tuple_element<1,decltype(str)>::type strt=get<1>(str); 	//strt 是一个string 类型

	tuple_element<1,decltype(str)>::type str2="str string";
	
	//关系运算
	tuple<string,string> duo("1","2");
	tuple<size_t,size_t> twoD(1,2);

	bool b;
	b=(duo==twoD);//错误,两者之间的类型不一样.

	tuple<size_t,size_t,size_t> threeD(1,2,3);

	b=(twoD<threeD);//错误,数量不一样;

	tuple<size_t,size_t> origin(0,0);

	b=(origin<twoD);//正确

	
}
```
* 不同数据类型的读取方式;
```cpp
#include <iostream>
#include <tuple>
using namespace std;
int main(int agrc, char **agrv)
{

	vector<string> str;
	str.push_back("china");
	pair<string, int> pairdata("string", 100);
	tuple<string, vector<string>, pair<string, int>> data("string", str, pairdata);
	for (int i = 0; i < tuple_size<decltype(data)>::value; i++)
	{
		//注意不同类型的数据的读取方式,是不同的;
		cout << get<0>(data) << endl;
		cout << get<1>(data).at(0) << endl;
		cout << get<2>(data).first << " " << get<2>(data).second << endl;
	}
}
```

## 6.使用tuple返回多个值;
tuple从一个函数返回多个值.
```cpp
#include <iostream>
#include <tuple>

using namespace std;

tuple<string, int> function(int a, int b)
{
	tuple<string, int> data_(to_string(a+b) ,a - b);
	return data_;
}
int main(int agrc, char **agrv)
{
	tuple<string,int> result=function(100,50);
	cout<<get<0>(result)<<" "<<get<1>(result)<<endl;
}
```
采用tuple 返回多个值,具有一定的优势;
```cpp
#include <iostream>
#include <tuple>

using namespace std;
struct returnData
{
	string str;
	int num;
};

returnData function2(int a,int b)
{
	returnData temp;
	temp.str=to_string(a+b);
	temp.num=a-b;
	return temp;
}

tuple<string, int> function(int a, int b)
{
	tuple<string, int> data_(to_string(a+b) ,a - b);
	return data_;
}

void function3(int a, int b, string &outA, int &outB)
{
	outA = to_string(a + b);
	outB = a - b;
}
int main(int agrc, char **agrv)
{
	tuple<string,int> result=function(100,50);
	cout<<get<0>(result)<<" "<<get<1>(result)<<endl;

	//上面调用等价于==>
    //和结构体或者类,相比,更加简单.当然也可以通过引用的方式进行值传递
	returnData data=function2(200,100);
	cout<<data.str<<" "<<data.num<<endl;

	//采取引用的方式
	string strA;
	int intB;
	function3(1000,800,strA,intB);
	cout<<strA<<" "<<intB<<endl;
}
```