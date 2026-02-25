> 此教程来源于:[CSDN:yuntian_li](https://blog.csdn.net/weixin_43991178/article/details/105119610)

[TOC]

* 教程代码：https://github.com/yuntianli91/pangolin_tutorial
* ROS Pangolin手册：[pangolin namespace](http://docs.ros.org/fuerte/api/pangolin_wrapper/html/namespacepangolin.html#ae25baa30091d20a3119459811f8d5b35)
### 1.创建一个简单的Pangolin
就像每一个编程语言的教程中都会有的Hello World一样，在Pangolin的学习中，也让我们首先来看一个简单的例子，在这个例子中，我们会创建一个交互窗口，并在窗口中显示一个立方体和对应的坐标系，代码如下：

```cpp
#include <pangolin/pangolin.h>

int main( int /*argc*/, char** /*argv*/ )
{
    // 创建名称为“Main”的GUI窗口，尺寸为640×640
    pangolin::CreateWindowAndBind("Main",640,480);
    // 启动深度测试
    glEnable(GL_DEPTH_TEST);

    // 创建一个观察相机视图
    pangolin::OpenGlRenderState s_cam(
        pangolin::ProjectionMatrix(640,480,420,420,320,320,0.2,100),
        pangolin::ModelViewLookAt(2,0,2, 0,0,0, pangolin::AxisY)
    );

    // 创建交互视图
    pangolin::Handler3D handler(s_cam); //交互相机视图句柄
    pangolin::View& d_cam = pangolin::CreateDisplay()
            .SetBounds(0.0, 1.0, 0.0, 1.0, -640.0f/480.0f)
            .SetHandler(&handler);

    while( !pangolin::ShouldQuit() )
    {
        // 清空颜色和深度缓存
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        d_cam.Activate(s_cam);

        // 在原点绘制一个立方体
        pangolin::glDrawColouredCube();
        // 绘制坐标系
       	glLineWidth(3);
        glBegin ( GL_LINES );
	    glColor3f ( 0.8f,0.f,0.f );
	    glVertex3f( -1,-1,-1 );
	    glVertex3f( 0,-1,-1 );
	    glColor3f( 0.f,0.8f,0.f);
	    glVertex3f( -1,-1,-1 );
	    glVertex3f( -1,0,-1 );
	    glColor3f( 0.2f,0.2f,1.f);
	    glVertex3f( -1,-1,-1 );
	    glVertex3f( -1,-1,0 );
	    glEnd();

        // 运行帧循环以推进窗口事件
        pangolin::FinishFrame();
    }
    
    return 0;
}

```
#### 1.1 代码解析
现在让我们逐步剖析上述代码：
```cpp
#include <pangolin/pangolin.h>
```
首先我们引入pangolin的头文件，Pangolin几乎所有的功能都在该头文件中。

```cpp
// 创建名称为“Main”的GUI窗口，尺寸为640×640
pangolin::CreateWindowAndBind("Main",640,480);
// 启动深度测试
glEnable(GL_DEPTH_TEST);

```
接下来，我们使用`CreateWindowAndBind`命令创建了一个视窗对象，函数的入口的`参数`依次为**视窗的名称、宽度和高度**，该命令类似于OpenCV中的namedWindow，即创建一个用于显示的窗体。同时，我们<u>启动了深度测试功能，该功能会使得pangolin只会绘制朝向镜头的那一面像素点</u>，避免容易混淆的透视关系出现，因此在任何3D可视化中都应该开启该功能。

```cpp
// 创建一个观察相机视图
// ProjectMatrix(int h, int w, int fu, int fv, int cu, int cv, int znear, int zfar) 
//      参数依次为观察相机的图像高度、宽度、4个内参以及最近和最远视距
// ModelViewLookAt(double x, double y, double z,double lx, double ly, double lz, AxisDirection Up)
//      参数依次为相机所在的位置，以及相机所看的视点位置(一般会设置在原点)
pangolin::OpenGlRenderState s_cam(
    pangolin::ProjectionMatrix(640,480,420,420,320,320,0.2,100),
    pangolin::ModelViewLookAt(2,0,2, 0,0,0, pangolin::AxisY)
);

```
在完成视窗的创建后，我们需要在视窗中`“放置”`一个摄像机（注意这里的摄像机是用于观察的摄像机而非SLAM中的相机），我们需要给出摄像机的内参矩阵`ProjectionMatrix`从而在我们对摄像机进行`交互操作`时，Pangolin会自动根据内参矩阵完成对应的透视变换。此外，我们还需要给出摄像机**初始时刻**所处的位置，摄像机的**视点位置**（即**摄像机的光轴朝向哪一个点**）以及**摄像机的本身哪一轴朝上**。

```cpp
// 创建交互视图
pangolin::Handler3D handler(s_cam); //交互相机视图句柄
pangolin::View& d_cam = pangolin::CreateDisplay()
        .SetBounds(0.0, 1.0, 0.0, 1.0, -640.0f/480.0f)
        .SetHandler(&handler);

```
接下来我们需要创建一个交互式视图（`view`）<u>用于显示上一步摄像机所“拍摄”到的内容</u>，这一步类似于`OpenGL`中的`viewport`处理。`setBounds`()函数前四个参数依次表示视图在视窗中的范围（下、上、左、右），可以采用**相对坐标**（0~1）以及绝对坐标（使用`Attach`对象）。

```cpp
 while( !pangolin::ShouldQuit() )
    {
        // 清空颜色和深度缓存
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        d_cam.Activate(s_cam);

        // 在原点绘制一个立方体
        pangolin::glDrawColouredCube();
        // 绘制坐标系
       	glLineWidth(3);
        glBegin ( GL_LINES );
	    glColor3f ( 0.8f,0.f,0.f );
	    glVertex3f( -1,-1,-1 );
	    glVertex3f( 0,-1,-1 );
	    glColor3f( 0.f,0.8f,0.f);
	    glVertex3f( -1,-1,-1 );
	    glVertex3f( -1,0,-1 );
	    glColor3f( 0.2f,0.2f,1.f);
	    glVertex3f( -1,-1,-1 );
	    glVertex3f( -1,-1,0 );
	    glEnd();

        // 运行帧循环以推进窗口事件
        pangolin::FinishFrame();
    }

```

在完成了上述所有准备工作之后，我们就可以开始绘制我们需要的图形了，首先我们使用`glclear`命令分别**清空色彩缓存**和**深度缓存并<u>激活之前设定好的视窗对象</u>**（否则视窗内会保留上一帧的图形，这种“多重曝光”效果通常并不是我们需要的）。接下来我们分别绘制了一个彩色立方体和一个坐标系，在绘制完成后，需要使用`FinishFrame`命令刷新视窗。

### 2. Pangolin与多线程
上一个例子中,我们在主线程中创建了视窗、视图并绘制了对应的图像，但在更多的应用中，出于效率考虑，可视化部分内容通常单独运行在一个线程中，现在让我们来将Task1中的代码改写为多线程版本：

```cpp
#include <pangolin/pangolin.h>
#include <thread>

static const std::string window_name = "HelloPangolinThreads";

void setup() {
    // create a window and bind its context to the main thread
    pangolin::CreateWindowAndBind(window_name, 640, 480);

    // enable depth
    glEnable(GL_DEPTH_TEST);

    // unset the current context from the main thread
    pangolin::GetBoundWindow()->RemoveCurrent();
}

void run() {
    // fetch the context and bind it to this thread
    pangolin::BindToContext(window_name);

    // we manually need to restore the properties of the context
    glEnable(GL_DEPTH_TEST);

    // Define Projection and initial ModelView matrix
    pangolin::OpenGlRenderState s_cam(
        pangolin::ProjectionMatrix(640,480,420,420,320,240,0.2,100),
        pangolin::ModelViewLookAt(-2,2,-2, 0,0,0, pangolin::AxisY)
    );

    // Create Interactive View in window
    pangolin::Handler3D handler(s_cam);
    pangolin::View& d_cam = pangolin::CreateDisplay()
            .SetBounds(0.0, 1.0, 0.0, 1.0, -640.0f/480.0f)
            .SetHandler(&handler);

    while( !pangolin::ShouldQuit() )
    {
        // Clear screen and activate view to render into
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        d_cam.Activate(s_cam);

        // Render OpenGL Cube
        pangolin::glDrawColouredCube();

        // Swap frames and Process Events
        pangolin::FinishFrame();
    }

    // unset the current context from the main thread
    pangolin::GetBoundWindow()->RemoveCurrent();
}

int main( int /*argc*/, char** /*argv*/ )
{
    // create window and context in the main thread
    setup();

    // use the context in a separate rendering thread
    std::thread render_loop;
    render_loop = std::thread(run);
    render_loop.join();

    return 0;
}

```
#### 2.1 代码解析

```cpp
void setup() {
    // create a window and bind its context to the main thread
    pangolin::CreateWindowAndBind(window_name, 640, 480);

    // enable depth
    glEnable(GL_DEPTH_TEST);

    // unset the current context from the main thread
    pangolin::GetBoundWindow()->RemoveCurrent();
}

```

在多线程版本的`HelloPangolin`中，我们首先利用`setup()`函数创建了一个视窗用于后续的显示，但这个视窗实在主线程中创建的，因此在主线程调用完后，需要使用**GetBoundWindow()->RemoveCurrent()**将其解绑。

```cpp
void run() {
    // fetch the context and bind it to this thread
    pangolin::BindToContext(window_name);

    // we manually need to restore the properties of the context
    glEnable(GL_DEPTH_TEST);
	...
	...
    // unset the current context from the main thread
    pangolin::GetBoundWindow()->RemoveCurrent();
}

```
随后，我们新开一个线程，运行我们的`run()`函数，在`run()`函数中，我们首先使用`BindToContext()`**函数将之前解绑的视窗绑定到当前线程，随后需要重新设置视窗的属性（即启动深度测试）**；同样，在线程结束时，我们需要**解绑视窗**。

### 3. Pangolin 控件
我们在上一讲程序的基础上，为视窗添加一个控制面板，展示一些简单的控件操作，代码如下：

```cpp
#include <pangolin/pangolin.h>
#include <string>
#include <iostream>
// ----------------------------------------------------------------- //

void SampleMethod()
{
    std::cout << "You typed ctrl-r or pushed reset" << std::endl;
    // std::cout << "Window width: " << i << std::endl;
}
// ----------------------------------------------------------------------- //
int main(/*int argc, char* argv[]*/)
{  

    // 创建视窗
    pangolin::CreateWindowAndBind("Main",640,480);
    // 启动深度测试
    glEnable(GL_DEPTH_TEST);
    // 创建一个摄像机
    pangolin::OpenGlRenderState s_cam(
        pangolin::ProjectionMatrix(640,480,420,420,320,240,0.1,1000),
        pangolin::ModelViewLookAt(-0,0.5,-3, 0,0,0, pangolin::AxisY)
    );
    // 分割视窗
    const int UI_WIDTH = 180;

    // 右侧用于显示视口
    pangolin::View& d_cam = pangolin::CreateDisplay()
        .SetBounds(0.0, 1.0, pangolin::Attach::Pix(UI_WIDTH), 1.0, -640.0f/480.0f)
        .SetHandler(new pangolin::Handler3D(s_cam));
    // 左侧用于创建控制面板
    pangolin::CreatePanel("ui")
        .SetBounds(0.0, 1.0, 0.0, pangolin::Attach::Pix(UI_WIDTH));

    // 创建控制面板的控件对象，pangolin中
    pangolin::Var<bool> A_Button("ui.a_button", false, false);  // 按钮
    pangolin::Var<bool> A_Checkbox("ui.a_checkbox", false, true); // 选框
    pangolin::Var<double> Double_Slider("ui.a_slider", 3, 0, 5); //double滑条
    pangolin::Var<int> Int_Slider("ui.b_slider", 2, 0, 5); //int滑条
    pangolin::Var<std::string> A_string("ui.a_string", "Hello Pangolin");

    pangolin::Var<bool> SAVE_IMG("ui.save_img", false, false);  // 按钮
    pangolin::Var<bool> SAVE_WIN("ui.save_win", false, false); // 按钮
    pangolin::Var<bool> RECORD_WIN("ui.record_win", false, false); // 按钮

    pangolin::Var<std::function<void()>> reset("ui.Reset", SampleMethod);// 

    // 绑定键盘快捷键
    // Demonstration of how we can register a keyboard hook to alter a Var
    pangolin::RegisterKeyPressCallback(pangolin::PANGO_CTRL + 'b', pangolin::SetVarFunctor<double>("ui.a_slider", 3.5));

    // Demonstration of how we can register a keyboard hook to trigger a method
    pangolin::RegisterKeyPressCallback(pangolin::PANGO_CTRL + 'r', SampleMethod);
    // Default hooks for exiting (Esc) and fullscreen (tab).
    while( !pangolin::ShouldQuit() )
    {
        // Clear entire screen
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);    

        // 各控件的回调函数
        if(pangolin::Pushed(A_Button))
            std::cout << "Push button A." << std::endl;
        
        if(A_Checkbox)
            Int_Slider = Double_Slider;
        // 保存整个win
        if( pangolin::Pushed(SAVE_WIN) )
        pangolin::SaveWindowOnRender("window");
		// 保存view
        if( pangolin::Pushed(SAVE_IMG) )
        d_cam.SaveOnRender("cube");
		// 录像
        if( pangolin::Pushed(RECORD_WIN) )
        pangolin::DisplayBase().RecordOnRender("ffmpeg:[fps=50,bps=8388608,unique_filename]//screencap.avi");
        d_cam.Activate(s_cam);
        // glColor3f(1.0,0.0,1.0);
        pangolin::glDrawColouredCube();

        // Swap frames and Process Events
        pangolin::FinishFrame();
    }

    return 0;
}

```

#### 3.1 代码解析
```cpp
 // 分割视窗
 const int UI_WIDTH = 180;
 // 右侧用于显示视口
 pangolin::View& d_cam = pangolin::CreateDisplay()
     .SetBounds(0.0, 1.0, pangolin::Attach::Pix(UI_WIDTH), 1.0, -640.0f/480.0f)
     .SetHandler(new pangolin::Handler3D(s_cam));
 // 左侧用于创建控制面板
 pangolin::CreatePanel("ui")
     .SetBounds(0.0, 1.0, 0.0, pangolin::Attach::Pix(UI_WIDTH));

```

与前两例相同，我们首先依旧需要创建一个视窗和对应的摄像机模型，随后我们在将视窗的坐标180像素宽度位置使用`CreatePanel()`命令创建一个面板，并给这个面板命名为`“ui”`，这里`"ui"`是面板的`tag`名称，后续所有控制的操作都通过这个`tag`绑定到对应的面板上。视窗的其余部分则为用于显示的`viewport`，创建方式和上一节相同。

```cpp
  // 创建控制面板的控件对象，pangolin中
    pangolin::Var<bool> A_Button("ui.a_button", false, false);  // 按钮
    pangolin::Var<bool> A_Checkbox("ui.a_checkbox", false, true); // 选框
    pangolin::Var<double> Double_Slider("ui.a_slider", 3, 0, 5); //double滑条
    pangolin::Var<int> Int_Slider("ui.b_slider", 2, 0, 5); //int滑条
    pangolin::Var<std::string> A_string("ui.a_string", "Hello Pangolin");

    pangolin::Var<bool> SAVE_IMG("ui.save_img", false, false);  // 按钮
    pangolin::Var<bool> SAVE_WIN("ui.save_win", false, false); // 按钮
    pangolin::Var<bool> RECORD_WIN("ui.record_win", false, false); // 按钮

    pangolin::Var<std::function<void()>> reset("ui.Reset", SampleMethod);// 

```
随后，我们来创建一系列常用控件，`Pangolin`将所有“控件”视为一个`pangolin::Var`对象，该对象是一个模板类，我们可以向其中传递自定义的类型模板，亦可使用`Pangolin`预先定义的类模板。这里将常用的`pangolin::Var`对象整理如下：

* `pangolin::Var<bool>` ：`bool`型`Var`对象，创建**参数**依次为**控件的tag、初始值以及是否可以`toggle`**。当`toggle`设置为`true`时，该对象表示一个**复选框**（`Checkbox`）；设置为`false`时则表示一个**按钮**（Button）。初始值对于设置为`true`或`false`会影响选框是否被选中，对于按钮来说则没有影响，不过出于习惯，一般都会设置为false。控件的`tag`是唯一的，命名格式为`panel_tag.controller_tag`，例如这里我们所有控件需要板顶的面板`tag`为`“ui”`，因此所有的控件`tag`都命名为`ui.xxx`的形式，以下所有的控件`tag`都遵从这一规范，不再赘述。

* `pangolin::Var<int/double/float>`：这一类Var对象为常见的**滑条**对象，创建**参数**依次为**控件tag、初始值、最小值、最大值和logscale**。其中`logscale`表示**是否以对数坐标形式显示**；最小值和最大值控制滑条的范围，如果不设置的话默认最小值为0，最大值为1;主要注意的是，这里的初始值值得是滑条上初始显示的数字，因此其不需要在滑条的范围内，只不过在用户移动滑条后，显示的数字会更新为滑条当前位置对应的数字。

* `pangolin::Var<std::function<void()>>`：这一类控件同样实现**按钮控件**的功能，只是其在创建时传入一个`std::function`函数对象，因此不需要在后续循环中进行回调函数的书写。不过如果回调函数中如果需要进行参数的传入和传出，使用std::function会比较麻烦，因此其常用来编写一些void(void)类型的简单功能函数。

需要注意的是，上述所有控件的**必要参数**只有控件tag和初始值，其他参数不存在时pangolin会自动调用默认参数进行处理。

```cpp
  if(pangolin::Pushed(A_Button))
            std::cout << "Push button A." << std::endl;
        
        if(A_Checkbox)
            Int_Slider = Double_Slider;
        
        if( pangolin::Pushed(SAVE_WIN) )
        pangolin::SaveWindowOnRender("window");

        if( pangolin::Pushed(SAVE_IMG) )
        d_cam.SaveOnRender("cube");

        if( pangolin::Pushed(RECORD_WIN) )
        pangolin::DisplayBase().RecordOnRender("ffmpeg:[fps=50,bps=8388608,unique_filename]//screencap.avi");
```

最后，在`while`循环中，我们为`Checkbox`和`Button`编写了对应的回调函数，对于`Checkbox`来说，直接判断其本身状态为`true`和`false`即可；而`Button`对象则需要使用`pangolin::Pushed(string tag)`函数判断其是否按下。

### 4. 多视图与图片显示
注意：这里的多视图只是功能上用于在视窗的不同区域叠加显示不同功能模块的多视图，而非真正意义上将视窗严格划分的多视图，pangolin中提供了SimpleMultiDisplay例子用于演示整整的多视图分割，但据作者说只是demo，运行效果并不稳定.

```cpp
#include <opencv2/opencv.hpp>
#include <pangolin/pangolin.h>

int main(int argc, char** argv){
    // 创建视窗
    pangolin::CreateWindowAndBind("MultiImage", 752, 480);
    // 启动深度测试
    glEnable(GL_DEPTH_TEST);
    // 设置摄像机
    pangolin::OpenGlRenderState s_cam(
        pangolin::ProjectionMatrix(752, 480, 420, 420, 320, 320, 0.1, 1000),
        pangolin::ModelViewLookAt(-2, 0, -2, 0, 0, 0, pangolin::AxisY)
    );
    // ---------- 创建三个视图 ---------- //
    pangolin::View& d_cam = pangolin::Display("cam")
        .SetBounds(0., 1., 0., 1., -752/480.)
        .SetHandler(new pangolin::Handler3D(s_cam));
    
    pangolin::View& cv_img_1 = pangolin::Display("image_1")
        .SetBounds(2/3.0f, 1.0f, 0., 1/3.0f, 752/480.)
        .SetLock(pangolin::LockLeft, pangolin::LockTop);
    
    pangolin::View& cv_img_2 = pangolin::Display("image_2")
        .SetBounds(0., 1/3.0f, 2/3.0f, 1.0, 752/480.)
        .SetLock(pangolin::LockRight, pangolin::LockBottom);
    // 创建glTexture容器用于读取图像
    pangolin::GlTexture imgTexture1(752, 480, GL_RGB, false, 0, GL_BGR, GL_UNSIGNED_BYTE);
    pangolin::GlTexture imgTexture2(752, 480, GL_RGB, false, 0, GL_BGR, GL_UNSIGNED_BYTE);

    while(!pangolin::ShouldQuit()){
        // 清空颜色和深度缓存
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        // 启动相机
        d_cam.Activate(s_cam);
        // 添加一个立方体
        glColor3f(1.0f, 1.0f, 1.0f);
        pangolin::glDrawColouredCube();
        // 从文件读取图像
        cv::Mat img1 = cv::imread("../task4/img/img1.png");
        cv::Mat img2 = cv::imread("../task4/img/img2.png");
        // 向GPU装载图像
        imgTexture1.Upload(img1.data, GL_BGR, GL_UNSIGNED_BYTE);
        imgTexture2.Upload(img2.data, GL_BGR, GL_UNSIGNED_BYTE);
        // 显示图像
        cv_img_1.Activate();
        glColor3f(1.0f, 1.0f, 1.0f); // 设置默认背景色，对于显示图片来说，不设置也没关系
        imgTexture1.RenderToViewportFlipY(); // 需要反转Y轴，否则输出是倒着的
        
        cv_img_2.Activate();
        glColor3f(1.0f, 1.0f, 1.0f); // 设置默认背景色，对于显示图片来说，不设置也没关系
        imgTexture2.RenderToViewportFlipY();

        pangolin::FinishFrame();
    }

    return 0;
}

```

#### 4.1 代码解析
```cpp
 pangolin::View& d_cam = pangolin::Display("cam")
        .SetBounds(0., 1., 0., 1., -752/480.)
        .SetHandler(new pangolin::Handler3D(s_cam));
    
    pangolin::View& cv_img_1 = pangolin::Display("image_1")
        .SetBounds(2/3.0f, 1.0f, 0., 1/3.0f, 752/480.)
        .SetLock(pangolin::LockLeft, pangolin::LockTop);
    
    pangolin::View& cv_img_2 = pangolin::Display("image_2")
        .SetBounds(0., 1/3.0f, 2/3.0f, 1.0, 752/480.)
        .SetLock(pangolin::LockRight, pangolin::LockBottom);

```
在这个例子中，我们首先创建在视窗中创建了三个视图，其中一个是我们很熟悉的相机视图，在本例中我们特意让相机视图充满了整个视窗，以演示我们前面说明的这里的多视图其实是通过视图“叠加”实现的。紧接着我们创建了另外两个视图用于显示图片，其中一个视图位于左上角，一个视图位于右下角。

这里我们着重解释下创建视图函数中的**第五个参数**。我们注意到，<u>在后两个视图的创建中，该参数是**正值**，而在创建动画视窗视使用的是**负值**。该参数实际上表征的是**视图的分辨率**，当该参数**取正值时**，`pangolin`**会将由前四个参数设置的视图大小进行裁剪**，以满足所设置的分辨率；当该值为**负值**时，pangolin会在将**图片拉伸**以充满由前四个参数设置的视图范围。</u>

此外，由于后两个视图并不是交互视图，因此我们没有使用`setHandler()`设置视图句柄，而是使用了`setLock()`函数设置了视图锁定的位置，**该函数会在我们缩放整个视窗后，按照设定的`lock`选项自动锁定对其位置。**例如在本例中，我们将左上角的视图设置为`left`和`top`锁定，右下角的视图则设置为`right`和`bottom`锁定。


```cpp
 // 创建glTexture容器用于读取图像
    pangolin::GlTexture imgTexture1(752, 480, GL_RGB, false, 0, GL_BGR, GL_UNSIGNED_BYTE);
    pangolin::GlTexture imgTexture2(752, 480, GL_RGB, false, 0, GL_BGR, GL_UNSIGNED_BYTE);
```

随后我们需要创建两个图像纹理容器`pangolin::GlTexture`用于向上面创建的视图装载图像，该对象的**入口参数依次为图像宽度、图像高度、`pangolin`的内部图像存储格式，是否开启现行采样，边界大小（像素）、gl图像存储格式以及gl数据存储格式**。由于我们使用`OpenCV`从文件中读取并存储图像，`cv::Mat`的图像存储顺序为`BGR`，而数据存储格式为`uint`型，因此最后两个参数我们分别设置为`GL_BGR`和`GL_UNSIGNED_BYTE`，至于`pangolin`的内部存储格式，对图片的显示影响不大，因此一般都设置为`GL_RGB`。

需要注意的是，这里**<u>图像的宽度和高度一定要设置为和原图片一致</u>**，否则会导致图像无法正常显示。另外两个参数默认设置为`false`和`0`即可。

```cpp
 // 向GPU装载图像
 imgTexture1.Upload(img1.data, GL_BGR, GL_UNSIGNED_BYTE);
 imgTexture2.Upload(img2.data, GL_BGR, GL_UNSIGNED_BYTE);
 // 显示图像
 cv_img_1.Activate();
 glColor3f(1.0f, 1.0f, 1.0f); // 设置默认背景色，对于显示图片来说，不设置也没关系
 imgTexture1.RenderToViewportFlipY(); // 需要反转Y轴，否则输出是倒着的
        
 cv_img_2.Activate();
 glColor3f(1.0f, 1.0f, 1.0f); // 设置默认背景色，对于显示图片来说，不设置也没关系
 imgTexture2.RenderToViewportFlipY();

```

最后，我们通过创建的`pangolin::GlTexture`对象向`GPU`中装载图像，由于该对象只接受`uchar*`对象，因此需要传递`cv::Mat`的`data`成员，而不能传递`cv::Mat`本身；另外两个参数则和我们在创建`pangolin::GlTexture`对象时使用的最后两个参数一致。

在装载完图像后，我们依次**激活视窗、设置默认背景色、最后渲染显示图像**，这里原始渲染出的图像是倒着的，因此我们翻转了Y轴，具体原因暂不明。


### 5.pangolin绘制数据曲线

首先我们来学习如何使用`pangolin`绘制函数曲线，在本例中，我们将在一个视图中分别绘制 `sin ⁡ ( x ) \sin(x) sin(x)、 cos ⁡ ( x ) \cos(x) cos(x)`以及 `sin ⁡ ( x ) + cos ⁡ ( x ) \sin(x)+\cos(x) sin(x)+cos(x)`的曲线，代码如下：

```cpp
#include <iostream>
#include <pangolin/pangolin.h>

int main(/*int argc, char* argv[]*/)
{
  // Create OpenGL window in single line
  pangolin::CreateWindowAndBind("Main",640,480);

  // Data logger object
  pangolin::DataLog log;

  // Optionally add named labels
  std::vector<std::string> labels;
  labels.push_back(std::string("sin(t)"));
  labels.push_back(std::string("cos(t)"));
  labels.push_back(std::string("sin(t)+cos(t)"));
  log.SetLabels(labels);

  const float tinc = 0.01f;

  // OpenGL 'view' of data. We might have many views of the same data.
  pangolin::Plotter plotter(&log,0.0f,4.0f*(float)M_PI/tinc,-4.0f,4.0f,(float)M_PI/(4.0f*tinc),0.5f);
  plotter.SetBounds(0.0, 1.0, 0.0, 1.0);
  plotter.Track("$i");//坐标轴自动滚动

  // Add some sample annotations to the plot（为区域着色）
  plotter.AddMarker(pangolin::Marker::Vertical,   50*M_PI, pangolin::Marker::LessThan, pangolin::Colour::Blue().WithAlpha(0.2f) );
  plotter.AddMarker(pangolin::Marker::Horizontal,   3, pangolin::Marker::GreaterThan, pangolin::Colour::Red().WithAlpha(0.2f) );
  plotter.AddMarker(pangolin::Marker::Horizontal,    3, pangolin::Marker::Equal, pangolin::Colour::Green().WithAlpha(0.2f) );

  pangolin::DisplayBase().AddDisplay(plotter);

  float t = 0;

  // Default hooks for exiting (Esc) and fullscreen (tab).
  while( !pangolin::ShouldQuit() )
  {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    log.Log(sin(t),cos(t),sin(t)+cos(t));
    t += tinc;

    // Render graph, Swap frames and Process Events
    pangolin::FinishFrame();
  }

  return 0;
}

```

#### 5.1 代码解析
```cpp
 // Data logger object
  pangolin::DataLog log;

  // Optionally add named labels
  std::vector<std::string> labels;
  labels.push_back(std::string("sin(t)"));
  labels.push_back(std::string("cos(t)"));
  labels.push_back(std::string("sin(t)+cos(t)"));
  log.SetLabels(labels);

``` 
在`pangolin`中，待可视化的数据全部存储在`pangolin::DataLog`对象中，因此我们首先创建了一个`pangolin::DataLog`对象，并使用对应的成员函数`SetLabels()`设置对应数据的名称（即图例）。

```cpp
 // OpenGL 'view' of data. We might have many views of the same data.
  pangolin::Plotter plotter(&log,0.0f,4.0f*(float)M_PI/tinc,-4.0f,4.0f,(float)M_PI/(4.0f*tinc),0.5f);
  plotter.SetBounds(0.0, 1.0, 0.0, 1.0);
  plotter.Track("$i");//坐标轴自动滚动

```

而数据的可视化则是通过`pangolin::Plotter`对象来实现的，该对象的构造参数的第一个参数为需要绘制的`pangolin::DataLog`对象；随后4个参数**依次Plotter的左边界、右边界、下边界、上边界**，即`Plotter`中  x轴 y 轴的范围；最后两个参数依次为x轴和y轴的坐标轴刻度大小。

```cpp
  // Add some sample annotations to the plot（为区域着色）
  plotter.AddMarker(pangolin::Marker::Vertical,   50*M_PI, pangolin::Marker::LessThan, pangolin::Colour::Blue().WithAlpha(0.2f) );
  plotter.AddMarker(pangolin::Marker::Horizontal,   3, pangolin::Marker::GreaterThan, pangolin::Colour::Red().WithAlpha(0.2f) );
  plotter.AddMarker(pangolin::Marker::Horizontal,    3, pangolin::Marker::Equal, pangolin::Colour::Green().WithAlpha(0.2f) );

```
随后我们演示了在`Plotter`中使用`plotter`的成员函数`AddMarker`添加一些标志块的功能，该函数入口参数依次为标志块的方向，标志块的数值，标志块的判别方式以及标志块的颜色。例如第一个标志块的方向为垂直方向，数值为 `50π`，判断方式为小于，颜色为带透明度的蓝色，因此我们在程序的运行结果中会发现x轴坐标小于`50π`的范围都被标记为了透明的蓝色。同理第二个`Marker`将`y`轴大于3的区域标记为了红色，第三个`Marker`由于是等于，因此其只将`y=3`这一条线标记为了绿色。

```cpp
  pangolin::DisplayBase().AddDisplay(plotter);
```
随后，我们将构建好的`plotter`添加到`Display`中。

```cpp
while( !pangolin::ShouldQuit() )
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    log.Log(sin(t),cos(t),sin(t)+cos(t));
    t += tinc;

    // Render graph, Swap frames and Process Events
    pangolin::FinishFrame();
}

```
在帧循环中，我们只需要使用`DataLog::Log()`函数不断更新`DataLog`中的数据，`pangolin`就会根据我们之前创建的`plotter`自动在视窗中绘制数据。


### 6. pangolin绘制相机位姿

SLAM可视化中最重要的一个内容就是实时显示相机的轨迹和当前的位姿，本次任务的代码较多，这里只贴一些关键部分的代码，完整的代码参见[github](https://github.com/yuntianli91/pangolin_tutorial)。
首先让我们来绘制一个简单的相机模型，这里我们借鉴ORB-SLAM的做法，通过一些简单的线条来表示相机模型：

```cpp
	const float w = 0.2;
	const float h = w * 0.75;
	const float z = w * 0.6;

	glLineWidth(2); 
	glBegin(GL_LINES);
	glColor3f(0.0f,1.0f,1.0f);
	glVertex3f(0,0,0);		glVertex3f(w,h,z);
	glVertex3f(0,0,0);		glVertex3f(w,-h,z);
	glVertex3f(0,0,0);		glVertex3f(-w,-h,z);
	glVertex3f(0,0,0);		glVertex3f(-w,h,z);
	glVertex3f(w,h,z);		glVertex3f(w,-h,z);
	glVertex3f(-w,h,z);		glVertex3f(-w,-h,z);
	glVertex3f(-w,h,z);		glVertex3f(w,h,z);
	glVertex3f(-w,-h,z);    glVertex3f(w,-h,z);
	glEnd();

```

下面我们让这个相机模型动起来，显然最简单的想法是在 <u>**每次获取相机的位姿后，对上述八点线段的坐标进行相应的变换，进而绘制出当前时刻的相机模型** </u>。但如果每次都需要我们去计算变换后的位姿，这无疑是非常麻烦且容易出错的。

幸运的是，`OpenGL`提供了`glMultMatrix()`函数自动帮我们处理图像点的位姿转换，代码如下：

```cpp
	glPushMatrix();
	std::vector<GLdouble> Twc = {R(0, 0), R(1,0), R(2, 0), 0.,
                            R(0, 1), R(1, 1), R(2, 1), 0.,
                            R(0, 2), R(1, 2), R(2, 2), 0.,
                            pos.x(), pos.y(), pos.z(), 1.};
    glMultMatrixd(Twc.data());
    // 绘制相机轮廓线
    const float w = 0.2;
    const float h = w * 0.75;
    const float z = w * 0.6;

    glLineWidth(2); 
    glBegin(GL_LINES);
    glColor3f(0.0f,1.0f,1.0f);
	glVertex3f(0,0,0);		glVertex3f(w,h,z);
	glVertex3f(0,0,0);		glVertex3f(w,-h,z);
	glVertex3f(0,0,0);		glVertex3f(-w,-h,z);
	glVertex3f(0,0,0);		glVertex3f(-w,h,z);
	glVertex3f(w,h,z);		glVertex3f(w,-h,z);
	glVertex3f(-w,h,z);		glVertex3f(-w,-h,z);
	glVertex3f(-w,h,z);		glVertex3f(w,h,z);
	glVertex3f(-w,-h,z);    glVertex3f(w,-h,z);
	glEnd();
    glPopMatrix();

```
首先我们需要使用`glPushMatrix()`告诉`pangolin`我们需要使用一个矩阵；随后我们使用`glMultMatrixd()`告诉`pangolin`后续绘制中的所有坐标均需要乘以这个矩阵；最后再`glPopMatrix()`弹出矩阵，便于下一次循环填入新的矩阵数值。需要注意的是，`pangolin`里的矩阵是按照**列主序**存储的。

```cpp
 // -------- 绘制相机轨迹 --------//
	glLineWidth(2);
    glBegin(GL_LINES);
    glColor3f(0.f, 1.f, 0.f);
    for(size_t i=0; i<traj.size() - 1; i++){
        glVertex3d(traj[i].x(), traj[i].y(), traj[i].z());
        glVertex3d(traj[i+1].x(), traj[i+1].y(), traj[i+1].z());
    }
    glEnd();

```
最后，我们将所有的相机位置存储起来，并依次画线，即可得到相机的轨迹