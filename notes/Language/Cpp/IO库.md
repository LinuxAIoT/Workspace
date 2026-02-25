[TOC]
## IO库
io库主要的io文件
|头文件|类型|
|---|---|
|iostream|istream,wistream,从**流**读取数据|
||ostream,wostream,向**流**写入数据|
||iostream,wiostream,读写**流**|
|ifstream|fstream,wifstream,从**文件**读取数据|
||ofstream,wofstream,向**文件**写入数据|
||fstream,wfstream,读写**文件**|
|sstream|isstringstream,wistringstream,从**string**读取数据|
||ostringstream,wostringstream,向**string**写入数据|
||stringstream,wstringstream,读写**string**|

## 1.文件的打开与关闭
### 1.1打开文件
文件的打开是读写文件前所做的准备工作，具体包含：
1. 使文件通道(即定义的文件流类的对象)与具体的磁盘上的指定文件建立关联；
2. 指定文件的打开方式，如文件是用于输出还是输入，文本文件还是二进制文件；

* 文件的打开方式有两种：
    1. 调用文件流类的构造函数，在定义文件对象时按指定方式打开有关文件，其格式如下：
    **类名 文件对象名 ("磁盘文件名"，打开方式)；**
    ```cpp
    ifstream infile("a1.dat",ios_base::binary); //即打开当前目录下一个二进制文件a1.dat用于向内存空间输入数据；
    ofstream outfile("b1.txt");//即打开当前目录下一个文本文件b1.txt用于存放内存空间输出数据；
    ```
    2. 调用文件流类的成员函数open,在定义文件流类的对象后按指定的方式打开具体文件，格式如下：
    **文件流对象名.open ("磁盘文件名"，打开方式)；**
    ```cpp
    ifstream infile;
    infile.open("a1.dat",ios_base::binary);
    ```
**当打开文件的操作成功，则文件流对象返回非零值，反之，打开文件的操作失败，文件流对象值为0；**
所以可以根据返回值来判断打开文件是否成功。
```cpp
ifstream infile;
infile.open("a1.dat",ios_base::binary);
if(!infile)
{
    cout<<"open error！"<<endl;
}   
```
文件的打开方式有很多种，如果省略打开方式，则**默认是以文本打开**；

|方式|说明|
|----|----|
|ios_base::in|以输入方式打开文件|
|ios_base::out|以输出方式打开文件，如果当前目录中有同名文件，则**清除该文件内容**|
|ios_base::app|以输出方式打开文件，如果当前目录中有同名文件,则**原文件内容保持不变，新输入的数据添加在原文件内容的末尾**|
|ios_base::ate|打开一个已有文件，打开后文件指针指向文件末尾|
|ios_base::binary|以二进制方式打开一个文件|
|ios_basse::trunc|致使现有文件被覆盖|
### 1.2 关闭文件
当打开一个文件进行读写后，应该显示的关闭文件；通过成员函数`close()`

**文件流对象名.close()**
```cpp
ifstream infile("a1.dat",ios_base::binary);
if(!infile)
{
    cout<<"open error!";
}
...
infile.close();//关闭文件
```

### 19.2 文件的读写
文件的读写有两种方式：
1. 用运算符“>>”与“<<”输入输出标准类型的数据；
   
```cpp
#include<fstream>
#include<iostream>
using namespace std;
int main()
{
    ifstream infile("/home/mrzhao/Documents/Vscode/Cpp/a1.txt");  //默认以文本方式打开
    if(!infile)
    {
        cout<<"open Error!"<<endl;
        return EXIT_FAILURE;
    }
    ofstream outfile("/home/mrzhao/Documents/Vscode/Cpp/b1.txt",ios_base::out);
    if(!outfile)
    {z
        cout<<"不能建立输出文件！"<<endl;
        return EXIT_FAILURE;
    }
    int a[10];
    double sum=0,aver;
    for(int i=0;i<10;i++)
    {
        infile>>a[i];
        sum+=a[i];
    }
    aver=sum/10;
    for(int j=0;j<10;j++)
    {
        outfile<<a[j]<<'\t';
    }
    outfile<<endl;
    outfile<<"sum="<<sum<<'\n'<<"aver="<<aver<<endl;

    infile.close(); //关闭输入文件
    outfile.close();    //关闭输出文件
}
    
```
2. 通过输出流的各种成员函数(`get(),getline(),put()`)
3. 判断程序是否读到输入文件的末尾有两种方法：
   1. 使用文件流的成员函数eof(),如，inflile.eof(),若文件结束，返回0；
   2. 判断输入表达式的返回值，如infile>>a[i]或infile.get(),若文件结束，返回0；
```cpp
#include<fstream>
#include<iostream>
using namespace std;
int main()
{
    ifstream infile("/home/mrzhao/Documents/Vscode/Cpp/a1.txt");  //默认以文本方式打开
    if(!infile)
    {
        cout<<"open Error!"<<endl;
        return EXIT_FAILURE;
    }
    ofstream outfile("/home/mrzhao/Documents/Vscode/Cpp/b1.txt",ios_base::out);
    if(!outfile)
    {
        cout<<"不能建立输出文件！"<<endl;
        return EXIT_FAILURE;
    }
    char str[10][100];
    int count,i=0;
    while (!infile.eof())
    {
        infile.getline(str[i],100);     //从infile流中读取一行长度为100，存入str[i]中
        cout<<str[i]<<endl;
    }
    
    infile.close(); //关闭输入文件
    outfile.close();    //关闭输出文件
}
    
```
## 2. string 流
主要包含以下三种方式：
* **istringstream**:从string读取数据
* **ostringstream**:向string写数据
* **stringstream**:读写string流
  
### 2.1使用istringstream
有的时候，我们需要从一串字符串中`读取单个单词`，同常需要使用`istringstream`；
例如现在有一个文件，列出了一些人和他们的姓名和电话号码，某些人只有一个号码，而另外一些人则有多个号码——家庭号码、工作号码...；这些输入文件可能是下面这种：
> 张三  132123 186028
> 李四  156898
> 王五  159288 137226 187008

文件中每一条记录都是一个人名开始，后面紧跟多个电话号码；我们先定义一个结构体：
```cpp
struct PersonInfo{
    string name;
    vector<string> phones;
};
```
```cpp
#include <iostream>
#include <vector>
#include <fstream>
#include <string>

#include <sstream>//for istringstream
using namespace std;
struct PersonInfo
{
    string name;
    vector<string> phones;
};

int main()
{
    string line,word;//分别保存来自输入的一行和单词；
    vector<PersonInfo> people; //保存来自输入的所有记录；
    while(getline(cin,line)) //!从输入流获取一行信息，并保存到line中；==>linux 下 按“ctrl + d”结束输入；
    {
        PersonInfo person;
        istringstream isstream(line);//!将line与istringstream 建立连接
        isstream>>person.name;//!从istream中，读取一个’单词‘，保存到person.name；

        while(isstream>>word) //! **继续**从string流中读取数据，是接着isstream>>person.name;再读取一个’单词‘;
                                //!而不是从头在开始读；
        {
            person.phones.push_back(word);
        }
        people.push_back(person);//将此记录追加到people末尾
    }
    for(auto &o:people)
    {
        cout<<o.name<<" ";
    }
    return 0;
}
```
最需要注意的是以下两句：
首先从string流中读取人名，这是无可厚非的，只是接下来在使用`isstream>>word`，此时从`isstream`中读取的是`string`的**下一个单词，而不是从头开始读取；**

```cpp
isstream>>person.name;//!从istream中，读取一个’单词‘，保存到person.name；
while(isstream>>word) //! **继续**从string流中读取数据，是接着isstream>>person.name;再读取一个’单词‘;
                        //!而不是从头在开始读；
{
    person.phones.push_back(word);
}
```
<font color=red>与cin相同，进行流提取的时候，每次只能提取一个'单词',即`cin>>x>>y;`需要输入两个值分别赋予给x,y;`isstream>>word`，则会从string流中提取一个Word。</font>

### 2.2 使用ostringstream
当我们逐步构造输出，最后一起打印时，`ostringstream`是非常有效的；
同常我们进行输出时，调用cout,此时会执行一句就会输出一句，如果程序有多条语句需要输出，这些语句又是无关紧要的，若执行一次输出一次，因为进行输出是比较耗费时间的，所以，可以先将要输出的东西写入到一个内存的`ostringstream`中，最后统一输出；
```cpp
ostringstream oostream ;//此处不需要绑定string对象；
oostream<<"first data"<<"\t";
oostream<<"seconde data"<<"\n";
cout<<oostream.str()<<endl;
```
## 3. getline()
原型如下：从`basic_istream`中读取一行；`basic_istream`是什么？其实就是标准的输入流`istream`,由于，`ifstream`与`istringstream` 都是继承istream,所以`basic_istream`也可以是`ifstream`与`istringstream`；

返回：**返回对输入流的引用。**
```cpp
inline basic_istream<_CharT, _Traits>& getline(basic_istream<_CharT, _Traits>& __is,
	    basic_string<_CharT, _Traits, _Alloc>& __str)
string value;
getline(cin,value);//从输入流中读取一行”单词“保存到value
getline(infile【ifstream对象】,value);//文件流中读取一行，保存到value;

```

## 4. 综合实例

```cpp
//

//

#include<iostream>
#include<sstream>
#include<string>
#include<fstream>
using namespace std;
int main()
{
    string word;
    string line;
    //从文件中读取一行
    string filepath="/home/mrzhao/Documents/Vscode/Cpp/IO输入文件.txt";
    ifstream infile(filepath,ios_base::in);

    getline(infile,line);//从infile读取一行，保存到line中；
    cout<<line<<endl;

    while (getline(infile,line))//!同过while循环读出**剩余**所有内容；
                                //!上一句通过getline(infile,line);读出第一句，再使用getline读取流时，
                                //!是从第二行开始读取,而不是从头开始读取；
    {
        cout<<line<<endl;
    }
   
    infile.close();         //首先断开与IO输入文件.txt关联后才能与别的文件进行关联；
    infile.open("/home/mrzhao/Documents/Vscode/Cpp/IO输入文件2.txt",ios_base::in);

    infile>>word;           //!从文件流中读取一个单词；
    cout<<word<<endl;
    getline(infile,line);   //! 读取第一行剩余元素；
    cout<<line<<endl;

    //从string流中逐个提取'单词’==>以空格作为划分依据
    istringstream strstream(line);//需要和string绑定；
    strstream>>word;        //!以空格作为划分依据
    cout<<word<<endl;

    strstream>>word;
    cout<<word<<endl;

    return 0;
}
```