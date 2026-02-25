[TOC]
## 1. 使用Boost.asio 进行网络通信;
## 2. tcp 通信:
```cpp
//server.cpp
#include <iostream>
#include <boost/asio.hpp>

using namespace std;
using namespace boost::asio;
int main()
{
    try
    {
        typedef ip::tcp::acceptor acceptor_type;
        typedef ip::tcp::endpoint endpoint_type;
        typedef ip::tcp::socket socket_type;

        std::cout << "Server start" << std::endl;
        io_service service_io;

        acceptor_type acceptor(service_io, endpoint_type(ip::tcp::v4(), 8888));
        cout << "服务器 ip address:" << acceptor.local_endpoint().address() << endl;
        cout << "服务器capacity:" << acceptor.local_endpoint().capacity() << endl;
        cout << "服务器port:" << acceptor.local_endpoint().port() << endl;
        cout << "服务器protocol:" << acceptor.local_endpoint().protocol().type() << endl;

        vector<char> str(1024,0);
        boost::system::error_code ec;
        while (true)
        {
            cout << "等待客户端接入..." << endl;
            socket_type sock(service_io); //通过socket建立数据连接;
            acceptor.accept(sock);        //阻塞函数,会一直阻塞直到客户端接入;
            cout << "读取客户端数据..." << endl;
            sock.read_some(buffer(str),ec);//读取数据也是一个阻塞函数,知道有数据可以读取;
            cout<<"客户端数据数据:"<<&str[0]<<endl;
            cout << "客户端接入,准备发送数据..." << endl;
            cout << "当前客户端ip:" << sock.remote_endpoint().address() << endl;
            cout << "当前客户端capacity:" << sock.local_endpoint().capacity() << endl;
            cout << "当前客户端port:" << sock.local_endpoint().port() << endl;
            cout << "当前客户端protocol:" << sock.local_endpoint().protocol().type() << endl;
            sock.send(buffer("发送数据测试"));  
        }
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
    }

    return 0;
}


//Client.cpp
#include <iostream>
#include <boost/asio.hpp>
using namespace std;
using namespace boost::asio;
int main()
{
    try
    {
        io_service sevice_io;
        ip::tcp::socket socket_(sevice_io);
        cout << "client start..." << endl;
        ip::tcp::endpoint ep(ip::address::from_string("127.0.0.1"), 8888);
        socket_.connect(ep);

        vector<char> str(1024, 0);
        boost::system::error_code rc;

        while (true)
        {
            socket_.send(buffer("客户端测试数据:123456789"));
            socket_.read_some(buffer(str), rc);
            if (rc)
            {
                break;
            }
            cout << &str[0] << endl;
        }
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
    }
    return 0;
}
//CMakeList.txt
cmake_minimum_required(VERSION 3.0)
project(tcp_icp)

# Check C++11 or C++0x support
include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
CHECK_CXX_COMPILER_FLAG("-std=c++0x" COMPILER_SUPPORTS_CXX0X)
if(COMPILER_SUPPORTS_CXX11)
   set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
   add_definitions(-DCOMPILEDWITHC11)
   message(STATUS "Using flag -std=c++11.")
elseif(COMPILER_SUPPORTS_CXX0X)
   set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
   add_definitions(-DCOMPILEDWITHC0X)
   message(STATUS "Using flag -std=c++0x.")
else()
   message(FATAL_ERROR "The compiler ${CMAKE_CXX_COMPILER} has no C++11 support. Please use a different C++ compiler.")
endif()
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS}  -Wall  -O3 -march=native -pthread")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall   -O3 -march=native -pthread")

find_package(OpenCV 3.0 QUIET)
if(NOT OpenCV_FOUND)
   find_package(OpenCV 2.4.3 QUIET)
   if(NOT OpenCV_FOUND)
      message(FATAL_ERROR "OpenCV > 2.4.3 not found.")
   endif()
endif()

include_directories(${OpenCV_INCLUDE_DIRS})
add_executable(client client.cpp)
add_executable(server server.cpp)

target_link_libraries(client ${OpenCV_LIBRARIES})
target_link_libraries(server ${OpenCV_LIBRARIES})
```
