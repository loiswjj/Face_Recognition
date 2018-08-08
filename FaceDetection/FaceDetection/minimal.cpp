// minimal.cpp: Display the landmarks of a face in an image.
//              This demonstrates stasm_search_single.

#include <stdio.h>
#include <stdlib.h>
#include "opencv/highgui.h"
#include "opencv2/objdetect/objdetect.hpp"  
#include "opencv2/highgui/highgui.hpp"  
#include "opencv2/imgproc/imgproc.hpp"  
#include <iostream>  
#include <stdio.h>  
#include <opencv2/core/core.hpp>   
#include "stasm_lib.h"
#include <core/core.hpp>
#include <direct.h>  
#include <crtdbg.h>
#include "stasm_landmarks.h"
using namespace cv ;

int main()
{
	VideoCapture capture(0);
	if (!capture.isOpened())
	{ 
		printf("打开摄像头失败");
		return -1;
	}//检测是否成功打开摄像头
	Mat img2;
	while (1)
	{
		static const char* const path = "../data/testface.jpg";
		capture>> img2;
		if (!capture.read(img2))//这个判断的必须的
		{
			break;
		}
		Mat gray;
		Mat img;
		cvtColor(img2,img,CV_BGR2GRAY,0);
		if (waitKey(30) >= 0)//刷新间隔30ms，按任意键退出
			break;
		int foundface;
		float landmarks[2 * stasm_NLANDMARKS]={0}; // x,y coords (note the 2)
		if (!stasm_search_single(&foundface, landmarks,(const char*)img.data, img.cols, img.rows, path, "../data"))
		{
			printf("Error in stasm_search_single: %s\n", stasm_lasterr());
			return 0;
		}
		if (!foundface)
			printf("No face found\n");
		else
		{
			stasm_force_points_into_image(landmarks, img.cols, img.rows);//坐标在范围限制
			for (int i = 0; i <stasm_NLANDMARKS; i++)
			{  
				circle(img2,Point(cvRound(landmarks[i*2]),cvRound(landmarks[i*2+1])),3,CV_RGB(255,255,255),1,8, 0);
			}
#if 0
			float pinned[2 * stasm_NLANDMARKS]; // x,y coords  
			memset(pinned, 0, sizeof(pinned));//初始化一个pin空间和stasm_NLANDMARK一样大  
			pinned[L_LEyeOuter*2]      = landmarks[L_LEyeOuter*2] + 2;  
			pinned[L_LEyeOuter*2+1]    = landmarks[L_LEyeOuter*2+1];  
			pinned[L_REyeOuter*2]      = landmarks[L_REyeOuter*2] - 2;  
			pinned[L_REyeOuter*2+1]    = landmarks[L_REyeOuter*2+1];  
			pinned[L_CNoseTip*2]       = landmarks[L_CNoseTip*2];  
			pinned[L_CNoseTip*2+1]     = landmarks[L_CNoseTip*2+1];  
			pinned[L_LMouthCorner*2]   = landmarks[L_LMouthCorner*2];  
			pinned[L_LMouthCorner*2+1] = landmarks[L_LMouthCorner*2+1];  
			pinned[L_RMouthCorner*2]   = landmarks[L_RMouthCorner*2];  
			pinned[L_RMouthCorner*2+1] = landmarks[L_RMouthCorner*2+1];  
			memset(landmarks, 0, sizeof(landmarks));//将landmarks重置为0  
			if (!stasm_search_pinned(landmarks,pinned, (const char*)img.data, img.cols, img.rows, path))  
				printf("Nooooooooooooooooooooooooooooooooo");
			for (int i = 0; i <stasm_NLANDMARKS; i++)
			{  
				circle(img2,Point(cvRound(landmarks[i*2]),cvRound(landmarks[i*2+1])),3,CV_RGB(255,0,255),1,8, 0);
			}
#endif
			//  76点
#if 1
			stasm_convert_shape(landmarks, 76);
			//下巴轮廓[0~15)共15个点;
			int start=0;
			int end=15;
			for (int i = start+1; i <end; i++)
			{  
				line(img2,Point(cvRound(landmarks[i*2]),cvRound(landmarks[i*2+1])),Point(cvRound(landmarks[i*2-2]),cvRound(landmarks[i*2-1])),CV_RGB(255,255,255),1,8, 0);
			}
			////	右眉毛[15~21)共6个点;
			start=15;
			end=21;
			for (int i = start+1; i <end; i++)
			{  
				line(img2,Point(cvRound(landmarks[i*2]),cvRound(landmarks[i*2+1])),Point(cvRound(landmarks[i*2-2]),cvRound(landmarks[i*2-1])),CV_RGB(255,255,255),1,8, 0);
			}
			line(img2,Point(cvRound(landmarks[end*2-2]),cvRound(landmarks[end*2-1])),Point(cvRound(landmarks[start*2]),cvRound(landmarks[start*2+1])),CV_RGB(255,255,255),1,8, 0);
			//	//右眉毛[21~27)共6个点;
			start=21;
			end=27;
			for (int i = start+1; i <end; i++)
			{  
				line(img2,Point(cvRound(landmarks[i*2]),cvRound(landmarks[i*2+1])),Point(cvRound(landmarks[i*2-2]),cvRound(landmarks[i*2-1])),CV_RGB(255,255,255),1,8, 0);
			}
			line(img2,Point(cvRound(landmarks[end*2-2]),cvRound(landmarks[end*2-1])),Point(cvRound(landmarks[start*2]),cvRound(landmarks[start*2+1])),CV_RGB(255,255,255),1,8, 0);
			//左眼球[27~31)共4个点;
				start=27;
				end=31;
				for (int i = start+1; i <end; i++)
				{  
					line(img2,Point(cvRound(landmarks[i*2]),cvRound(landmarks[i*2+1])),Point(cvRound(landmarks[i*2-2]),cvRound(landmarks[i*2-1])),CV_RGB(255,255,255),1,8, 0);
				}
				line(img2,Point(cvRound(landmarks[end*2-2]),cvRound(landmarks[end*2-1])),Point(cvRound(landmarks[start*2]),cvRound(landmarks[start*2+1])),CV_RGB(255,255,255),1,8, 0);
				//右眼球[32~36)共4个点;
				start=32;
				end=36;
				for (int i = start+1; i <end; i++)
				{  
					line(img2,Point(cvRound(landmarks[i*2]),cvRound(landmarks[i*2+1])),Point(cvRound(landmarks[i*2-2]),cvRound(landmarks[i*2-1])),CV_RGB(255,255,255),1,8, 0);
				}
				line(img2,Point(cvRound(landmarks[end*2-2]),cvRound(landmarks[end*2-1])),Point(cvRound(landmarks[start*2]),cvRound(landmarks[start*2+1])),CV_RGB(255,255,255),1,8, 0);
			////鼻梁[37~46)共9个点
			start=37;
			end=46;
			for (int i = start+1; i <end; i++)
			{  
				line(img2,Point(cvRound(landmarks[i*2]),cvRound(landmarks[i*2+1])),Point(cvRound(landmarks[i*2-2]),cvRound(landmarks[i*2-1])),CV_RGB(255,255,255),1,8, 0);
			}
			////嘴外轮廓[48~60)共12个点
			start=48;
			end=60;
			for (int i = start+1; i <end; i++)
			{  
				line(img2,Point(cvRound(landmarks[i*2]),cvRound(landmarks[i*2+1])),Point(cvRound(landmarks[i*2-2]),cvRound(landmarks[i*2-1])),CV_RGB(255,255,255),1,8, 0);
			}
			line(img2,Point(cvRound(landmarks[end*2-2]),cvRound(landmarks[end*2-1])),Point(cvRound(landmarks[start*2]),cvRound(landmarks[start*2+1])),CV_RGB(255,255,255),1,8, 0);
			////嘴2[60~66)共6个点
			start=60;
			end=66;
			for (int i = start+1; i <end; i++)
			{  
				line(img2,Point(cvRound(landmarks[i*2]),cvRound(landmarks[i*2+1])),Point(cvRound(landmarks[i*2-2]),cvRound(landmarks[i*2-1])),CV_RGB(255,255,255),1,8, 0);
			}
			line(img2,Point(cvRound(landmarks[end*2-2]),cvRound(landmarks[end*2-1])),Point(cvRound(landmarks[start*2]),cvRound(landmarks[start*2+1])),CV_RGB(255,255,255),1,8, 0);
			//左眼球
			start=68;
			end=72;
			for (int i = start+1; i <end; i++)
			{  
				line(img2,Point(cvRound(landmarks[i*2]),cvRound(landmarks[i*2+1])),Point(cvRound(landmarks[i*2-2]),cvRound(landmarks[i*2-1])),CV_RGB(0,255,255),1,8, 0);
			}
			line(img2,Point(cvRound(landmarks[end*2-2]),cvRound(landmarks[end*2-1])),Point(cvRound(landmarks[start*2]),cvRound(landmarks[start*2+1])),CV_RGB(0,255,255),1,8, 0);
			//右眼球
			start=72;
			end=76;
			for (int i = start+1; i <end; i++)
			{  
				line(img2,Point(cvRound(landmarks[i*2]),cvRound(landmarks[i*2+1])),Point(cvRound(landmarks[i*2-2]),cvRound(landmarks[i*2-1])),CV_RGB(0,255,255),1,8, 0);
			}
			line(img2,Point(cvRound(landmarks[end*2-2]),cvRound(landmarks[end*2-1])),Point(cvRound(landmarks[start*2]),cvRound(landmarks[start*2+1])),CV_RGB(0,255,255),1,8, 0);
#endif
		}
		int thickness = 1;  
		int lineType = 8;  
		cvNamedWindow("show_image",256);
		cv::imshow("show_image", img2);
	}
	return 0;
}
