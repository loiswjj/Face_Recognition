#include "face.h"
#include "stdafx.h"

Mat warping(Mat &src, int rMax, Point prePoint, Point endPoint)
{
	double S = 1;//变换强度
	//	cout << "起始坐标：" << prePoint << "      终止坐标： " << endPoint << endl;
	int cols = src.cols;
	int rows = src.rows;
	Mat mask = Mat(Size(cols, rows), CV_8UC1, Scalar(0, 0, 0));
	Mat result = Mat(Size(cols, rows), CV_8UC3, Scalar(0, 0, 0));

	double x = 0, y = 0;
	for (int i = prePoint.y-rMax; i < prePoint.y + rMax;++i)
	{
		uchar  *data = mask.ptr<uchar>(i);
		for (int j= prePoint.x - rMax; j < prePoint.x + rMax; ++j)
		{

			double r = std::sqrt(0.01+(i - prePoint.y)*(i - prePoint.y) + (j - prePoint.x)*(j - prePoint.x));
			if (r <= rMax && i >= 0 && j >= 0 && i < rows && j < cols)
			{

				data[j] = 255;
				double temp1 = r*r;
				double temp2 = 1.0*(endPoint.x - prePoint.x)* (endPoint.x - prePoint.x)
					+ 1.0*(endPoint.y - prePoint.y)* (endPoint.y - prePoint.y);

				double temp = 1.0*(rMax*rMax - temp1) / (rMax*rMax - temp1 + (100.0/ S)*temp2);

				x = j - (endPoint.x - prePoint.x)*temp*temp;
				y = i - (endPoint.y - prePoint.y)*temp*temp;

				int x1 = (int)x;
				int y1 = (int)y;
				int x2 = x1 + 1;
				int y2 = y1 + 1;

				Vec3b src1 = src.at<Vec3b>(y1, x1);
				Vec3b src2 = src.at<Vec3b>(y1, x2);
				Vec3b src3 = src.at<Vec3b>(y2, x1);
				Vec3b src4 = src.at<Vec3b>(y2, x2);

				Vec3d up,down;
				up[0] = (double)(src1[0] * (x - x1) + src2[0] * (x2 - x));
				up[1] = (double)(src1[1] * (x - x1) + src2[1] * (x2 - x));
				up[2] = (double)(src1[2] * (x - x1) + src2[2] * (x2 - x));


				down[0] = (double)(src3[0] * (x - x1) + src4[0] * (x2 - x));
				down[1] = (double)(src3[1] * (x - x1) + src4[1] * (x2 - x));
				down[2] = (double)(src3[2] * (x - x1) + src4[2] * (x2 - x));


				result.at<Vec3b>(i, j)[0] = (cvRound)(up[0]*(y - y1) + down[0]*(y2 - y));
				result.at<Vec3b>(i, j)[1] = (cvRound)(up[1] * (y - y1) + down[1] * (y2 - y));
				result.at<Vec3b>(i, j)[2] = (cvRound)(up[2] * (y - y1) + down[2] * (y2 - y));

				//	cout << "(" << j << "," << i << ")   " << "(" << x << "," << y << ")   "<<endl;
			}
		}
	}

	for (int i = 0; i < rows;++i)
	{
		uchar * maskData = mask.ptr<uchar>(i);
		Vec3b * srcData = src.ptr<Vec3b>(i);
		Vec3b * dstData = result.ptr<Vec3b>(i);
		for (int j = 0; j < cols;++j)
		{
			if (maskData[j]==255)
				continue;
			dstData[j] = srcData[j];
		}
	}

	return result;
}