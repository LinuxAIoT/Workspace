## STL算法

## 1.distance()
我们知道，作用于同一容器的 2 个同类型迭代器可以有效指定一个区间范围。在此基础上，如果想获取该指定范围内包含元素的个数，就可以借助本节要讲的 distance() 函数。
distance() 函数用于计算两个迭代器表示的范围内包含元素的个数，其语法格式如下： 
```cpp
template<class InputIterator>
  typename iterator_traits<InputIterator>::difference_type distance (InputIterator first, InputIterator last);
```
其中，first 和 last 都为迭代器，其类型可以是输入迭代器、前向迭代器、双向迭代器以及随机访问迭代器；该函数会返回[first, last)范围内包含的元素的个数。

注意，first 和 last 的迭代器类型，直接决定了 distance() 函数底层的实现机制：
* 当 first 和 last 为随机访问迭代器时，distance() 底层直接采用 last - first 求得 [first, last) 范围内包含元素的个数，其时间复杂度为O(1)常数阶；
* 当 first 和 last 为非随机访问迭代器时，distance() 底层通过不断执行 ++first（或者 first++）直到 first==last，由此来获取 [first, last) 范围内包含元素的个数，其时间复杂度为O(n)线性阶。
另外，distance() 函数定义在<iterator>头文件，并位于 std 命名空间中。因此在使用此函数前，程序中应包含如下代码： 
```cpp
#include <iostream>     // std::cout
#include <iterator>     // std::distance  需要包含此头文件
#include <list>         // std::list
using namespace std;
int main() 
{
    //创建一个空 list 容器
    list<int> mylist;
    //向空 list 容器中添加元素 0~9
    for (int i = 0; i < 10; i++) {
        mylist.push_back(i);
     }
     //指定 2 个双向迭代器，用于执行某个区间
    list<int>::iterator first = mylist.begin();//指向元素 0
    list<int>::iterator last = mylist.end();//指向元素 9 之后的位置
    //获取 [first,last) 范围内包含元素的个数
    cout << "distance() = " << distance(first, last);
    return 0;

    // 输出：10
}    
```   