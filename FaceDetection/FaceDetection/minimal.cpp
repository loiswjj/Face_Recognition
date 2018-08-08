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
		printf("������ͷʧ��");
		return -1;
	}//����Ƿ�ɹ�������ͷ
	Mat img2;
	while (1)
	{
		static const char* const path = "../data/testface.jpg";
		capture>> img2;
		if (!capture.read(img2))//����жϵı����
		{
			break;
		}
		Mat gray;
		Mat img;
		cvtColor(img2,img,CV_BGR2GRAY,0);
		if (waitKey(30) >= 0)//ˢ�¼��30ms����������˳�
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
			stasm_force_points_into_image(landmarks, img.cols, img.rows);//�����ڷ�Χ����
			for (int i = 0; i <stasm_NLANDMARKS; i++)
			{  
				circle(img2,Point(cvRound(landmarks[i*2]),cvRound(landmarks[i*2+1])),3,CV_RGB(255,255,255),1,8, 0);
			}
#if 0
			float pinned[2 * stasm_NLANDMARKS]; // x,y coords  
			memset(pinned, 0, sizeof(pinned));//��ʼ��һ��pin�ռ��stasm_NLANDMARKһ����  
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
			memset(landmarks, 0, sizeof(landmarks));//��landmarks����Ϊ0  
			if (!stasm_search_pinned(landmarks,pinned, (const char*)img.data, img.cols, img.rows, path))  
				printf("Nooooooooooooooooooooooooooooooooo");
			for (int i = 0; i <stasm_NLANDMARKS; i++)
			{  
				circle(img2,Point(cvRound(landmarks[i*2]),cvRound(landmarks[i*2+1])),3,CV_RGB(255,0,255),1,8, 0);
			}
#endif
			//  76��
#if 1
			stasm_convert_shape(landmarks, 76);
			//�°�����[0~15)��15����;
			int start=0;
			int end=15;
			for (int i = start+1; i <end; i++)
			{  
				line(img2,Point(cvRound(landmarks[i*2]),cvRound(landmarks[i*2+1])),Point(cvRound(landmarks[i*2-2]),cvRound(landmarks[i*2-1])),CV_RGB(255,255,255),1,8, 0);
			}
			////	��üë[15~21)��6����;
			start=15;
			end=21;
			for (int i = start+1; i <end; i++)
			{  
				line(img2,Point(cvRound(landmarks[i*2]),cvRound(landmarks[i*2+1])),Point(cvRound(landmarks[i*2-2]),cvRound(landmarks[i*2-1])),CV_RGB(255,255,255),1,8, 0);
			}
			line(img2,Point(cvRound(landmarks[end*2-2]),cvRound(landmarks[end*2-1])),Point(cvRound(landmarks[start*2]),cvRound(landmarks[start*2+1])),CV_RGB(255,255,255),1,8, 0);
			//	//��üë[21~27)��6����;
			start=21;
			end=27;
			for (int i = start+1; i <end; i++)
			{  
				line(img2,Point(cvRound(landmarks[i*2]),cvRound(landmarks[i*2+1])),Point(cvRound(landmarks[i*2-2]),cvRound(landmarks[i*2-1])),CV_RGB(255,255,255),1,8, 0);
			}
			line(img2,Point(cvRound(landmarks[end*2-2]),cvRound(landmarks[end*2-1])),Point(cvRound(landmarks[start*2]),cvRound(landmarks[start*2+1])),CV_RGB(255,255,255),1,8, 0);
			//������[27~31)��4����;
				start=27;
				end=31;
				for (int i = start+1; i <end; i++)
				{  
					line(img2,Point(cvRound(landmarks[i*2]),cvRound(landmarks[i*2+1])),Point(cvRound(landmarks[i*2-2]),cvRound(landmarks[i*2-1])),CV_RGB(255,255,255),1,8, 0);
				}
				line(img2,Point(cvRound(landmarks[end*2-2]),cvRound(landmarks[end*2-1])),Point(cvRound(landmarks[start*2]),cvRound(landmarks[start*2+1])),CV_RGB(255,255,255),1,8, 0);
				//������[32~36)��4����;
				start=32;
				end=36;
				for (int i = start+1; i <end; i++)
				{  
					line(img2,Point(cvRound(landmarks[i*2]),cvRound(landmarks[i*2+1])),Point(cvRound(landmarks[i*2-2]),cvRound(landmarks[i*2-1])),CV_RGB(255,255,255),1,8, 0);
				}
				line(img2,Point(cvRound(landmarks[end*2-2]),cvRound(landmarks[end*2-1])),Point(cvRound(landmarks[start*2]),cvRound(landmarks[start*2+1])),CV_RGB(255,255,255),1,8, 0);
			////����[37~46)��9����
			start=37;
			end=46;
			for (int i = start+1; i <end; i++)
			{  
				line(img2,Point(cvRound(landmarks[i*2]),cvRound(landmarks[i*2+1])),Point(cvRound(landmarks[i*2-2]),cvRound(landmarks[i*2-1])),CV_RGB(255,255,255),1,8, 0);
			}
			////��������[48~60)��12����
			start=48;
			end=60;
			for (int i = start+1; i <end; i++)
			{  
				line(img2,Point(cvRound(landmarks[i*2]),cvRound(landmarks[i*2+1])),Point(cvRound(landmarks[i*2-2]),cvRound(landmarks[i*2-1])),CV_RGB(255,255,255),1,8, 0);
			}
			line(img2,Point(cvRound(landmarks[end*2-2]),cvRound(landmarks[end*2-1])),Point(cvRound(landmarks[start*2]),cvRound(landmarks[start*2+1])),CV_RGB(255,255,255),1,8, 0);
			////��2[60~66)��6����
			start=60;
			end=66;
			for (int i = start+1; i <end; i++)
			{  
				line(img2,Point(cvRound(landmarks[i*2]),cvRound(landmarks[i*2+1])),Point(cvRound(landmarks[i*2-2]),cvRound(landmarks[i*2-1])),CV_RGB(255,255,255),1,8, 0);
			}
			line(img2,Point(cvRound(landmarks[end*2-2]),cvRound(landmarks[end*2-1])),Point(cvRound(landmarks[start*2]),cvRound(landmarks[start*2+1])),CV_RGB(255,255,255),1,8, 0);
			//������
			start=68;
			end=72;
			for (int i = start+1; i <end; i++)
			{  
				line(img2,Point(cvRound(landmarks[i*2]),cvRound(landmarks[i*2+1])),Point(cvRound(landmarks[i*2-2]),cvRound(landmarks[i*2-1])),CV_RGB(0,255,255),1,8, 0);
			}
			line(img2,Point(cvRound(landmarks[end*2-2]),cvRound(landmarks[end*2-1])),Point(cvRound(landmarks[start*2]),cvRound(landmarks[start*2+1])),CV_RGB(0,255,255),1,8, 0);
			//������
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
