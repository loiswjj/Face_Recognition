//release
//��������vs2013+opencv2.4.8 
//����汾��ƥ�䣬���½����������±���

#include <opencv2\opencv.hpp>
#include <iostream>

using namespace cv;
using namespace std;

//void onMouse(int event, int x, int y, int flags, void *ustc);
Mat warping(Mat &src, int rMax, Point prePoint, Point endPoint);