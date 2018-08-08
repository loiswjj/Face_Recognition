#include "resource.h"
#include "stdafx.h"
#include <vector>
#include <string>
#include <opencv2\opencv.hpp>
#include <commdlg.h>
#include <direct.h>
#include <crtdbg.h>
#include "stasm_lib.h"
#include "stasm_landmarks.h"
#include <io.h>
#include <opencv2\face\facerec.hpp>
#include <opencv2\face.hpp>
#include <string>

#define MAX_PATH_LEN 1024

using namespace std;
using namespace cv;

//文件库
TCHAR FileDataBase[2048];
RECT WindowRect;

//摄像头控制
VideoCapture capture;

//用户自己添加的文件
static TCHAR pBuffer_Directory[MAX_PATH][MAX_PATH];
vector<string> filenames;
//级联分类器加载
string face_cascade_name = "Classifier/haarcascade_frontalface_alt.xml";
CascadeClassifier face_cascade;
static int index = 0;
static const char* const path = "../data/testface.jpg";

static int Index = 47;
bool isCollect = false;
int Count = 0; //用于截图

bool featureDis = false;
bool faceDis = false;

static Mat CurrentImg;
//记录下当前图片的68个特征点
vector<Point> feature(68);

//训练图片
vector<Mat> images;
vector<int> labels;
//用于label的标记
static int temp = 1;
Ptr<cv::face::FaceRecognizer> Model;

string _path = "data\\att_faces\\";
//加载联机分类器
void Init(){
	if (!face_cascade.load(face_cascade_name)){
		return;
	}
}

void showMatImgToWnd(HWND pictureWnd, const cv::Mat& disimg)//Mat图像加载到picture control函数  
{
	if (disimg.empty()) return;
	static BITMAPINFO *bitMapinfo = NULL;
	static bool First = TRUE;
	if (First)
	{
		BYTE *bitBuffer = new BYTE[40 + 4 * 256];//开辟一个内存区域  
		if (bitBuffer == NULL)
		{
			return;
		}
		First = FALSE;
		memset(bitBuffer, 0, 40 + 4 * 256);
		bitMapinfo = (BITMAPINFO *)bitBuffer;
		bitMapinfo->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		bitMapinfo->bmiHeader.biPlanes = 1;
		for (int i = 0; i<256; i++)
		{ //颜色的取值范围 (0-255)  
			bitMapinfo->bmiColors[i].rgbBlue = bitMapinfo->bmiColors[i].rgbGreen = bitMapinfo->bmiColors[i].rgbRed = (BYTE)i;
		}
	}
	bitMapinfo->bmiHeader.biHeight = -disimg.rows;
	bitMapinfo->bmiHeader.biWidth = disimg.cols;
	bitMapinfo->bmiHeader.biBitCount = disimg.channels() * 8;

	RECT drect;
	GetClientRect(pictureWnd, &drect);
	HDC hDC = GetDC(pictureWnd);               //HDC是Windows的一种数据类型，是设备描述句柄； 
	SetBkMode(hDC, TRANSPARENT);
	SetStretchBltMode(hDC, COLORONCOLOR);
	StretchDIBits(hDC,
		0,
		0,
		drect.right,  //显示窗口宽度  
		drect.bottom,  //显示窗口高度  
		0,
		0,
		disimg.cols,  //显示窗口宽度  
		disimg.rows,     //图像高度  
		disimg.data,
		bitMapinfo,
		DIB_RGB_COLORS,
		SRCCOPY);
	ReleaseDC(pictureWnd, hDC);
}

string tchar2str(TCHAR* str){
	int iLen = WideCharToMultiByte(CP_ACP, 0, str, -1, NULL, 0, NULL, NULL);
	char* chRtn = new char[iLen*sizeof(char)];
	WideCharToMultiByte(CP_ACP, 0, str, -1, chRtn, iLen, NULL, NULL);
	string res(chRtn);
	return res;
}

//打开文件
BOOL OpenFileDlg(HWND hWnd, TCHAR* szFileSelect)
{
	OPENFILENAME ofn;
	ZeroMemory(&ofn, sizeof(OPENFILENAME));
	//需要兼容UNICODE处理
	ZeroMemory(szFileSelect, sizeof(TCHAR)*MAX_PATH_LEN);
	ofn.Flags = OFN_EXPLORER | OFN_ALLOWMULTISELECT;
	ofn.hwndOwner = hWnd;
	ofn.lpstrFile = szFileSelect;
	ofn.lpstrFile[0] = '\0';
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.nMaxFile = MAX_PATH_LEN;
	ofn.nMaxFileTitle = MAX_PATH_LEN;
	if (GetOpenFileName(&ofn))
	{
		//与网上给出的方法不同，实践中只要打开OK，文件路径就保存在ofn.lpstrFile参数中
		return TRUE;
	}
	return FALSE;
}

void OpenCamera(){
	capture.open(0);
}

//框出人脸
void detectAndDisplay(Mat frame){
	vector<cv::Rect> faces;
	Mat frame_gray;

	cvtColor(frame, frame_gray, CV_BGR2GRAY);
	equalizeHist(frame_gray, frame_gray);

	face_cascade.detectMultiScale(frame_gray, faces, 1.1, 2, 0 | CV_HAAR_SCALE_IMAGE, cv::Size(30, 30));

	for (int i = 0; i < faces.size(); i++){
		cv::Point center(faces[i].x + faces[i].width*0.5, faces[i].y + faces[i].height*0.5);
		ellipse(frame, center, cv::Size(faces[i].width*0.45, faces[i].height*0.5), 0, 0, 360, cv::Scalar(255, 0, 255), 4, 8, 0);
		//rectangle(frame, faces[i], Scalar(255, 0, 255), 4, 8, 0);
	}
}
//显示出人脸的特征点
void detectFeatures(Mat img2){
	int foundface;
	Mat img;
	cvtColor(img2, img, CV_BGR2GRAY, 0);
	float landmarks[2 * stasm_NLANDMARKS] = { 0 }; // x,y coords (note the 2)
	if (!stasm_search_single(&foundface, landmarks, (const char*)img.data, img.cols, img.rows, path, "../data"))
	{
		//MessageBox(NULL, _T("no face"), 0, MB_OK);
		return;
	}
	if (!foundface)
		//MessageBox(NULL, _T("no face"), 0, MB_OK);
		return;
	else
	{
		stasm_force_points_into_image(landmarks, img.cols, img.rows);//坐标在范围限制
		for (int i = 0; i <stasm_NLANDMARKS; i++)
		{
			circle(img2, Point(cvRound(landmarks[i * 2]), cvRound(landmarks[i * 2 + 1])), 3, CV_RGB(0, 0, 255), 1, 8, 0);
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
		int start = 0;
		int end = 15;
		for (int i = start + 1; i <end; i++)
		{
			line(img2, Point(cvRound(landmarks[i * 2]), cvRound(landmarks[i * 2 + 1])), Point(cvRound(landmarks[i * 2 - 2]),
				cvRound(landmarks[i * 2 - 1])), CV_RGB(0, 0, 255), 1, 8, 0);
			feature[i] = Point(cvRound(landmarks[i * 2 - 2]), cvRound(landmarks[i * 2 - 1]));
		}
		////	左眉毛[15~21)共6个点;
		start = 15;
		end = 21;
		for (int i = start + 1; i <end; i++)
		{
			line(img2, Point(cvRound(landmarks[i * 2]), cvRound(landmarks[i * 2 + 1])), Point(cvRound(landmarks[i * 2 - 2]), 
				cvRound(landmarks[i * 2 - 1])), CV_RGB(0, 0, 255), 1, 8, 0);
			feature[i] = Point(cvRound(landmarks[i * 2 - 2]), cvRound(landmarks[i * 2 - 1]));
		}
		line(img2, Point(cvRound(landmarks[end * 2 - 2]), cvRound(landmarks[end * 2 - 1])), Point(cvRound(landmarks[start * 2]), 
			cvRound(landmarks[start * 2 + 1])), CV_RGB(0, 0, 255), 1, 8, 0);
		////   右眉毛[21~27)共6个点;
		start = 21;
		end = 27;
		for (int i = start + 1; i <end; i++)
		{
			line(img2, Point(cvRound(landmarks[i * 2]), cvRound(landmarks[i * 2 + 1])), Point(cvRound(landmarks[i * 2 - 2]), 
				cvRound(landmarks[i * 2 - 1])), CV_RGB(0, 0, 255), 1, 8, 0);
			feature[i] = Point(cvRound(landmarks[i * 2 - 2]), cvRound(landmarks[i * 2 - 1]));
		}
		line(img2, Point(cvRound(landmarks[end * 2 - 2]), cvRound(landmarks[end * 2 - 1])), Point(cvRound(landmarks[start * 2]), 
			cvRound(landmarks[start * 2 + 1])), CV_RGB(0, 0, 255), 1, 8, 0);
		//左眼球[27~31)共4个点;
		start = 27;
		end = 31;
		for (int i = start + 1; i <end; i++)
		{
			line(img2, Point(cvRound(landmarks[i * 2]), cvRound(landmarks[i * 2 + 1])), Point(cvRound(landmarks[i * 2 - 2]), 
				cvRound(landmarks[i * 2 - 1])), CV_RGB(0, 0, 255), 1, 8, 0);
			feature[i] = Point(cvRound(landmarks[i * 2 - 2]), cvRound(landmarks[i * 2 - 1]));
		}
		line(img2, Point(cvRound(landmarks[end * 2 - 2]), cvRound(landmarks[end * 2 - 1])), Point(cvRound(landmarks[start * 2]), 
			cvRound(landmarks[start * 2 + 1])), CV_RGB(0, 0, 255), 1, 8, 0);
		//右眼球[32~36)共4个点;
		start = 32;
		end = 36;
		for (int i = start + 1; i <end; i++)
		{
			line(img2, Point(cvRound(landmarks[i * 2]), cvRound(landmarks[i * 2 + 1])), Point(cvRound(landmarks[i * 2 - 2]), 
				cvRound(landmarks[i * 2 - 1])), CV_RGB(0, 0, 255), 1, 8, 0);
			feature[i] = Point(cvRound(landmarks[i * 2 - 2]), cvRound(landmarks[i * 2 - 1]));
		}
		line(img2, Point(cvRound(landmarks[end * 2 - 2]), cvRound(landmarks[end * 2 - 1])), Point(cvRound(landmarks[start * 2]), 
			cvRound(landmarks[start * 2 + 1])), CV_RGB(0, 0, 255), 1, 8, 0);
		////鼻梁[37~46)共9个点
		start = 37;
		end = 46;
		for (int i = start + 1; i <end; i++)
		{
			line(img2, Point(cvRound(landmarks[i * 2]), cvRound(landmarks[i * 2 + 1])), Point(cvRound(landmarks[i * 2 - 2]), 
				cvRound(landmarks[i * 2 - 1])), CV_RGB(0, 0, 255), 1, 8, 0);
			feature[i] = Point(cvRound(landmarks[i * 2 - 2]), cvRound(landmarks[i * 2 - 1]));
		}
		////嘴外轮廓[48~60)共12个点
		start = 48;
		end = 60;
		for (int i = start + 1; i <end; i++)
		{
			line(img2, Point(cvRound(landmarks[i * 2]), cvRound(landmarks[i * 2 + 1])), Point(cvRound(landmarks[i * 2 - 2]),
				cvRound(landmarks[i * 2 - 1])), CV_RGB(0, 0, 255), 1, 8, 0);
			feature[i] = Point(cvRound(landmarks[i * 2 - 2]), cvRound(landmarks[i * 2 - 1]));
		}
		line(img2, Point(cvRound(landmarks[end * 2 - 2]), cvRound(landmarks[end * 2 - 1])), Point(cvRound(landmarks[start * 2]), 
			cvRound(landmarks[start * 2 + 1])), CV_RGB(0, 0, 255), 1, 8, 0);
		////嘴2[60~66)共6个点
		start = 60;
		end = 66;
		for (int i = start + 1; i <end; i++)
		{
			line(img2, Point(cvRound(landmarks[i * 2]), cvRound(landmarks[i * 2 + 1])), Point(cvRound(landmarks[i * 2 - 2]), 
				cvRound(landmarks[i * 2 - 1])), CV_RGB(0, 0, 255), 1, 8, 0);
			feature[i] = Point(cvRound(landmarks[i * 2 - 2]), cvRound(landmarks[i * 2 - 1]));
		}
		line(img2, Point(cvRound(landmarks[end * 2 - 2]), cvRound(landmarks[end * 2 - 1])), Point(cvRound(landmarks[start * 2]),
			cvRound(landmarks[start * 2 + 1])), CV_RGB(0, 0, 255), 1, 8, 0);
		//左眼球
		start = 68;
		end = 72;
		for (int i = start + 1; i <end; i++)
		{
			line(img2, Point(cvRound(landmarks[i * 2]), cvRound(landmarks[i * 2 + 1])), Point(cvRound(landmarks[i * 2 - 2]), 
				cvRound(landmarks[i * 2 - 1])), CV_RGB(0, 255, 255), 1, 8, 0);
		}
		line(img2, Point(cvRound(landmarks[end * 2 - 2]), cvRound(landmarks[end * 2 - 1])), Point(cvRound(landmarks[start * 2]), 
			cvRound(landmarks[start * 2 + 1])), CV_RGB(0, 255, 255), 1, 8, 0);
		//右眼球
		start = 72;
		end = 76;
		for (int i = start + 1; i <end; i++)
		{
			line(img2, Point(cvRound(landmarks[i * 2]), cvRound(landmarks[i * 2 + 1])), Point(cvRound(landmarks[i * 2 - 2]), 
				cvRound(landmarks[i * 2 - 1])), CV_RGB(0, 255, 255), 1, 8, 0);
		}
		line(img2, Point(cvRound(landmarks[end * 2 - 2]), cvRound(landmarks[end * 2 - 1])), Point(cvRound(landmarks[start * 2]), 
			cvRound(landmarks[start * 2 + 1])), CV_RGB(0, 255, 255), 1, 8, 0);
#endif
	}
}
//在控件上显示摄像区域内的图片
void DrawCameraPic(HWND hWnd){
	Mat frame;
	while (capture.isOpened()){
		capture >> frame;
		CurrentImg = frame;
		char c = waitKey(30);
		if (c >=0 ){
			capture.release();
			break;
		}
		if (faceDis){
			detectAndDisplay(frame);
		}
		if (featureDis){
			detectFeatures(frame);
		}
		//showMatImgToWnd(hWnd, frame);
		string s;
		if (isCollect && Count<9){
			imwrite(s.assign(_path).append(to_string(Index)).append("\\").append(to_string(Count)).append(".jpg"), frame);
			Count++;
		}
		if (Count == 10){
			Index++;
			Count = 0;
			isCollect = false;
		}
		imshow("pic", frame);
	}
	cvDestroyWindow("pic");
}
//选择想要打开的文件并显示在控件上
void Openphoto(HWND hParent, HWND hWnd){
	capture.release();
	if (OpenFileDlg(hParent, pBuffer_Directory[index])){
		Mat pic = imread(tchar2str(pBuffer_Directory[index]));
		CurrentImg = pic;
		if (faceDis){
			detectAndDisplay(pic);
		}
		if (featureDis){
			detectFeatures(pic);
		}
		showMatImgToWnd(hWnd, pic);
		index++;
	}
}
//获得固定文件夹下（训练文件夹）的所有图片
void GetAllPic(string path, vector<string>& files){
	intptr_t handle;
	struct _finddata_t fileinfo;
	string p;
	handle = _findfirst(p.assign(path).append("\\*.*").c_str(), &fileinfo);
	if (-1 == handle) return;
	while (!_findnext(handle, &fileinfo)){
		if ((fileinfo.attrib &  _A_SUBDIR))
		{
			if (strcmp(fileinfo.name, ".") != 0 && strcmp(fileinfo.name, "..") != 0)
			{
				temp = atoi(fileinfo.name);
				GetAllPic(p.assign(path).append("\\").append(fileinfo.name), files);
			}
		}
		else
		{
			labels.push_back(temp);
			files.push_back(p.assign(path).append("\\").append(fileinfo.name));  
		}
	}
	_findclose(handle);
}

//redpic
void ReadPic(vector<string> filenames){
	for (int i = 0; i < filenames.size(); i++){
		Mat temp = imread(filenames[i]);
		cvtColor(temp, temp, CV_BGR2GRAY);
		if (!temp.empty()){
			images.push_back(temp);
		}
	}
}

//识别表情类别
void PicTrain(){
	string path = "data\\att_faces";
	GetAllPic(path, filenames);

	ReadPic(filenames);
	//开始训练
	Model = cv::face::createLBPHFaceRecognizer();
	Model->train(images, labels);
}

//人脸识别
void detectFace(){
	Mat im;
	cvtColor(CurrentImg, im, CV_BGR2GRAY);
	int predit = Model->predict(im);
	putText(CurrentImg, to_string(predit), Point(10, 30), CV_FONT_HERSHEY_COMPLEX, 1, Scalar(0,0,255),2);
}

//旋转不变LBP  
Mat RILBP(Mat img)
{
	uchar RITable[256];
	int temp;
	int val;
	Mat result;
	result.create(img.rows - 2, img.cols - 2, img.type());
	result.setTo(0);

	for (int i = 0; i<256; i++)
	{
		val = i;
		for (int j = 0; j<7; j++)
		{
			temp = i >> 1;
			if (val>temp)
			{
				val = temp;
			}
		}
		RITable[i] = val;
	}

	for (int i = 1; i<img.rows - 1; i++)
	{
		for (int j = 1; j<img.cols - 1; j++)
		{
			uchar center = img.at<uchar>(i, j);
			uchar code = 0;
			code |= (img.at<uchar>(i - 1, j - 1) >= center) << 7;
			code |= (img.at<uchar>(i - 1, j) >= center) << 6;
			code |= (img.at<uchar>(i - 1, j + 1) >= center) << 5;
			code |= (img.at<uchar>(i, j + 1) >= center) << 4;
			code |= (img.at<uchar>(i + 1, j + 1) >= center) << 3;
			code |= (img.at<uchar>(i + 1, j) >= center) << 2;
			code |= (img.at<uchar>(i + 1, j - 1) >= center) << 1;
			code |= (img.at<uchar>(i, j - 1) >= center) << 0;
			result.at<uchar>(i - 1, j - 1) = RITable[code];
		}
	}
	return result;
}

