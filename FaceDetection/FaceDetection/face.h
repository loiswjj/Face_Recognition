//release
//开发环境vs2013+opencv2.4.8 
//如果版本不匹配，请新建个工程重新编译

#include <opencv2\opencv.hpp>
#include <iostream>

using namespace cv;
using namespace std;

//void onMouse(int event, int x, int y, int flags, void *ustc);
Mat warping(Mat &src, int rMax, Point prePoint, Point endPoint);