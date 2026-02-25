## 1. findContours()
Opencv中通过使用findContours函数，简单几个的步骤就可以检测出物体的轮廓

先从findContours函数原型看起：

```cpp
    findContours( InputOutputArray image, OutputArrayOfArrays contours,
                                  OutputArray hierarchy, int mode,
                                  int method, Point offset=Point());
```
1. image，单通道图像矩阵，可以是灰度图，但更常用的是二值图像，一般是经过Canny、拉普拉斯等边缘检测算子处理过的二值像；
2. contours，定义为`vector<vector<Point>> contour`，是一个向量，并且是一个双重向量，向量内每个元素保存了一组由连续的Point点构成的点的集合的向量，每一组Point点集就是一个轮廓有多少轮廓，向量contours就有多少元素。
3. hierarchy，定义为“vector<Vec4i> hierarchy”，先来看一下Vec4i的定义：
```cpp
    typedef    Vec<int, 4> Vec4i;     
``` 
`Vec4i`是`Vec<int,4>`的别名，定义了一个“向量内每一个元素包含了4个int型变量”的向量。所以从定义上看，hierarchy也是一个向量，向量内每个元素保存了一个包含4个int整型的数组。向量hiararchy内的元素和轮廓向量contours内的元素是一一对应的，向量的容量相同。hierarchy向量内每一个元素的4个int型变量——hierarchy[i][0] ~hierarchy[i][3]，分别表示第i个轮廓的后一个轮廓、前一个轮廓、父轮廓、内嵌轮廓的索引编号。如果当前轮廓没有对应的后一个轮廓、前一个轮廓、父轮廓或内嵌轮廓的话，则hierarchy[i][0] ~hierarchy[i][3]的相应位被设置为默认值-1。

4. int型的mode，定义轮廓的检索模式：
* 取值一：CV_RETR_EXTERNAL只检测最外围轮廓，包含在外围轮廓内的内围轮廓被忽略
* 取值二：CV_RETR_LIST   检测所有的轮廓，包括内围、外围轮廓，但是检测到的轮廓不建立等级关系，彼此之间独立，没有等级关系，这就意味着这个检索模式下不存在父轮廓或内嵌轮廓，所以hierarchy向量内所有元素的第3、第4个分量都会被置为-1.
* 取值三：CV_RETR_CCOMP  检测所有的轮廓，但所有轮廓只建立两个等级关系，外围为顶层，若外围内的内围轮廓还包含了其他的轮廓信息，则内围内的所有轮廓均归属于顶层.
* 取值四：CV_RETR_TREE， 检测所有轮廓，所有轮廓建立一个等级树结构。外层轮廓包含内层轮廓，内层轮廓还可以继续包含内嵌轮廓。
5. int型的method，定义轮廓的近似方法：
* 取值一：CV_CHAIN_APPROX_NONE 保存物体边界上所有连续的轮廓点到contours向量内
* 取值二：CV_CHAIN_APPROX_SIMPLE 仅保存轮廓的拐点信息，把所有轮廓拐点处的点保存入contours
向量内，拐点与拐点之间直线段上的信息点不予保留
* 取值三和四：CV_CHAIN_APPROX_TC89_L1，CV_CHAIN_APPROX_TC89_KCOS使用teh-Chinl chain 近
似算法
6. 第六个参数：Point偏移量，所有的轮廓信息相对于原始图像对应点的偏移量，相当于在每一个检测出的轮廓点上加
上该偏移量，并且Point还可以是负值！
```cpp
#include "core/core.hpp"  
#include "highgui/highgui.hpp"  
#include "imgproc/imgproc.hpp"  
#include "iostream"
 
using namespace std; 
using namespace cv;  
 
int main(int argc,char *argv[])  
{
	Mat imageSource=imread(argv[1],0);
	imshow("Source Image",imageSource);
	Mat image;
	GaussianBlur(imageSource,image,Size(3,3),0);
	Canny(image,image,100,250);
	vector<vector<Point>> contours;
	vector<Vec4i> hierarchy;
	findContours(image,contours,hierarchy,RETR_TREE,CHAIN_APPROX_SIMPLE,Point());
	Mat imageContours=Mat::zeros(image.size(),CV_8UC1);
	Mat Contours=Mat::zeros(image.size(),CV_8UC1);  //绘制
	for(int i=0;i<contours.size();i++)
	{
		//contours[i]代表的是第i个轮廓，contours[i].size()代表的是第i个轮廓上所有的像素点数
		for(int j=0;j<contours[i].size();j++) 
		{
			//绘制出contours向量内所有的像素点
			Point P=Point(contours[i][j].x,contours[i][j].y);
			Contours.at<uchar>(P)=255;
		}
 
		//输出hierarchy向量内容
		char ch[256];
		sprintf(ch,"%d",i);
		string str=ch;
		cout<<"向量hierarchy的第" <<str<<" 个元素内容为："<<endl<<hierarchy[i]<<endl<<endl;
 
		//绘制轮廓
		drawContours(imageContours,contours,i,Scalar(255),1,8,hierarchy);
	}
	imshow("Contours Image",imageContours); //轮廓
	imshow("Point of Contours",Contours);   //向量contours内保存的所有轮廓点集
	waitKey(0);
	return 0;
}
```
