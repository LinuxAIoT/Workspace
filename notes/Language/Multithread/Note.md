## 1.线程休眠

```cpp
#include <iostream>
#include <thread>

using namespace std;
int main()
{
    std::this_thread::sleep_for(std::chrono::seconds(5)); //5s
    std::this_thread::sleep_for(std::chrono::seconds(5));   //1秒 = 1000毫秒=10^6微秒
    cout<<"5s\n";
    std::this_thread::sleep_for(std::chrono::microseconds(5*1000000));  //微妙
    cout<<"5s\n";
    std::this_thread::sleep_for(std::chrono::milliseconds(5000));  //毫秒
    cout<<"5s\n";
    std::this_thread::sleep_for(std::chrono::minutes(1));
    cout<<"1min\n";
    std::this_thread::sleep_for(std::chrono::hours(5));
    return 0;
}
 
```
## 2.detach()
detach：分离：
也就是主线程和子线程不在汇合。各自执行自己所在代码段，主线程不必等待子线程结束（jion），主线程可以提前结束；一旦detach（）之后，与这个主线程关联的thread对象就会失去与原来主线程之间的关联，此时这个子线程就会驻留在后台运行（主线程跟该线程失去联系），这个子线程就相当于被c++运行时库接管，当子线程执行完毕，由运行时库负责清理该线程相关的资源（守护线程）；
<font color=red>一旦执行detach()后，无法在执行jion()，但jion()后，可以调用detach()分离该线程</font>

## 3.jionable()
jionable:判断是否可以使用jion()或者detach()；如果该线程没有调用jion()或者调用detach()就会返回true；

