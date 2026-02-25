[TOC]
## 1.0 C语言结构体
C 语言的结构体与Cpp的结构体定义不同:
```cpp
#include <stdio.h>
struct Student
{
    char name[50];
    int age;
};

int main()
{
    struct Student stu1={"zhangsan",20};//与cpp在定义变量上最大的区别是,C中定义结构体变量,
                                            //必须添加`struct`,而Cpp中可以直接使用结构体变量名进行声明;
    printf("age=%d\n",stu1.age);
    printf("name=%s\n",stu1.name);
    return 0;
}

```
与cpp在定义变量上最大的区别是,C中定义结构体变量,必须添加`struct`,而Cpp中可以直接使用结构体变量名进行声明;
### 1.1 定义结构类型变量
1. 先声明结构体类型(`struct Student`),再定义该类型的变量(`stu1,stu2`);==>此时声明的结构体变量是一个局部变量

```cpp
struct Student stu1,stu2;
```
2. 在声明的同时定义变量==>此时声明的结构体变量是一个全局变量

```cpp
struct 结构体名
{
    成员表列;
}变量名表列;

struct Student
{
    char name[50];
    int age;
}stu1,stu2;
```
3. 不指定类型名而直接定义结构体类型变量==>无法定义结构体变量;

```cpp
struct 
{
    成员表列;
}变量名表列;
```
### 1.2 结构变量的初始化和引用
1. 在定义的同时初始化;
2. 在定义时初始化;
```cpp
#include <stdio.h>
struct Student
{
    char name[50];
    int age;
}stu2={"lisi",25};//在定义的同时初始化;

int main()
{
    struct Student stu1={"zhangsan",20};//在定义时初始化;
    printf("age=%d\n",stu1.age);
    printf("name=%s\n",stu1.name);

    printf("age=%d\n",stu2.age);
    printf("name=%s\n",stu2.name);
    return 0;
}
```
## 2. 结构体数组
```cpp
1. 第一种定义方式

 struct 结构体名
   {
       成员列表;
   } 数组名[数组长度];

2. 第二种定义方式

结构数组类型 数组名[数组长度];

struct Student stus[100];

3. 结构体数组初始化

结构体数组初始化的形式是在定义数组的后面加上`={初值表列}`,不同数组成员之间**不需要**用`{}`隔开,这是不同于数组;

#include <stdio.h>
struct Student
{
    char name[50];
    int age;
}stu2[2]={"张三",25,"李四",20};//在定义的同时初始化,两个数组成员在赋值的时候'不需要用大括号'分开;

void test()
{
    for(int i=0;i<2;i++)
    {
        printf("%s\n","全局变量...");
        printf("age=%d\n",stu2[i].age);
        printf("name=%s\n",stu2[i].name);
    }
}
int main()
{
    struct Student stu1[2]={"zhangsan",60,"lisi",50};//在定义时初始化,两个数组成员在赋值的时候'不需要用大括号'分开;;

    for(int i=0;i<2;i++)
    {
        printf("age=%d\n",stu1[i].age);
        printf("name=%s\n",stu1[i].name);

        printf("age=%d\n",stu2[i].age);
        printf("name=%s\n",stu2[i].name);
    }
    atexit(test);//main函数调用完才调用test;
    return 0;
}
```
## 3. typedef 用于结构体
```cpp
#include <stdio.h>
typedef struct
{
    int month;
    int year;
    int day;
} Date;

//使用typedef 可以简化程序:
/*
 *以上声明了一个新类型名为Date,代表上面的结构体,然后就可以用新的类型名Date去定义变量;
 * 这样做的好处就是在定义结构体变量时,不用在添加"struct"
 */

int main()
{
    Date birthday;//定义结构体类型变量birthday,不需要写成struct Date birthday;
    Date *p;//定义结构体指针类型变量p,指向此结构体类型数据;

    //p->day=100;此时p变量指向的内存地址是不确定的;

    p=&birthday;//由于birthday是一个局部变量,已经创建好了内存空间,所以地址是确定的;

    p->day=100;
    p->year=2020;
    p->month=12;
    printf("%d\t,%d\t,%d\n",p->day,p->month,p->year);
    
    return 0;
}

```

### 3.1 typedef 用于数组

```cpp
typedef int Num[100];//声明Num为整数数组类型别名
Num a;//此时定义a为整型数组名,他有100个元素;
```
### 3.2 typedef 用于结构体数组

```cpp
typedef struct Data
{
    //数据成员;
}DataList[100]; //声明DataList为*struct Data*数组类型别名
DataList dataList;//此时定义dataList为*struct Data*类型数组名,他有100个元素;
                    //等价于:struct Data dataList[100]; struct Data==>类型名;dataList 数组名;

#include <stdio.h>
typedef struct Data
{
    int year;
    int month;
    int day;
}DataList[10];

int main()
{
    DataList datalist;//此时datalist是一个包含10个struct Data结构体元素的数组.

    for(int i;i<10;i++)
    {
        datalist[i].day=i;
        datalist[i].year=2021;
        datalist[i].month=i*i;
    }

    for(int i;i<10;i++)
    {
        printf ("%d\n%d\n%d\n", datalist[i].day,datalist[i].month, datalist[i].year);
    }
    return 0;
}
```