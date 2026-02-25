
### 1、_khbit()函数

---
<font color=blue>_kbbit()函数的作用是检查控制台窗口的按键是否被按下。其格式为</font>
```cpp
int _kbhit( void );
```

如果在调用该函数时，有`按键`被按下，则返回一个`非零值`，否则该函数的返回值是0。需要注意的是，该函数是一个`非阻塞函数`，不管有没有按键被按下，该函数都会立即返回。_khbit()函数`一般与_getch()函数与getche()函数组合使用获取按键信息`。

---
### 2、_getch()函数
---
<font color=blue >_getch()函数的作用是从控制台中获取输入的字符，在获取输入的字符之后，并不会在控制台中显示该字符。</font>

该函数的格式为:

```cpp
int _getch( void );
```

该函数的返回值是获取到的字符值。需要注意的是，_getch()函数是一个阻塞函数，直到有字符输入时才会返回，所以该函数不会返回错误值。

**_khbit()函数和_getch()函数都是在<font color=red>conio.h</font>头文件中定义。**

---
### 2.2 example

---

_khbit()函数和_getch()函数组合使用

```cpp
    while (!_kbhit())
     
    {
     
    printf("Hit me! ");
     
    Sleep(1000);
     
    }
     
    printf("\nKey struck was '%c'\n", _getch());
```

`在以上代码中，当没有按键被按下时，_kbhit()函数的返回值是0，所以while()语句会循环显示“Hit me!”信息。`一旦有按键被按下，此时_kbhit()函数的返回值是非零，之后通过_getch()函数获取按键信息，并显示。图1是代码运行的效果。


