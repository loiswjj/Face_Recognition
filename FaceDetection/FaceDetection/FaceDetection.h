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

//�ļ���
TCHAR FileDataBase[2048];
RECT WindowRect;

//����ͷ����
VideoCapture capture;

//�û��Լ���ӵ��ļ�
static TCHAR pBuffer_Directory[MAX_PATH][MAX_PATH];
vector<string> filenames;
//��������������
string face_cascade_name = "Classifier/haarcascade_frontalface_alt.xml";
CascadeClassifier face_cascade;
static int index = 0;
static const char* const path = "../data/testface.jpg";

static int Index = 47;
bool isCollect = false;
int Count = 0; //���ڽ�ͼ

bool featureDis = false;
bool faceDis = false;

static Mat CurrentImg;
//��¼�µ�ǰͼƬ��68��������
vector<Point> feature(68);

//ѵ��ͼƬ
vector<Mat> images;
vector<int> labels;
//����label�ı��
static int temp = 1;
Ptr<cv::face::FaceRecognizer> Model;

string _path = "data\\att_faces\\";
//��������������
void Init(){
	if (!face_cascade.load(face_cascade_name)){
		return;
	}
}

void showMatImgToWnd(HWND pictureWnd, const cv::Mat& disimg)//Matͼ����ص�picture control����  
{
	if (disimg.empty()) return;
	static BITMAPINFO *bitMapinfo = NULL;
	static bool First = TRUE;
	if (First)
	{
		BYTE *bitBuffer = new BYTE[40 + 4 * 256];//����һ���ڴ�����  
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
		{ //��ɫ��ȡֵ��Χ (0-255)  
			bitMapinfo->bmiColors[i].rgbBlue = bitMapinfo->bmiColors[i].rgbGreen = bitMapinfo->bmiColors[i].rgbRed = (BYTE)i;
		}
	}
	bitMapinfo->bmiHeader.biHeight = -disimg.rows;
	bitMapinfo->bmiHeader.biWidth = disimg.cols;
	bitMapinfo->bmiHeader.biBitCount = disimg.channels() * 8;

	RECT drect;
	GetClientRect(pictureWnd, &drect);
	HDC hDC = GetDC(pictureWnd);               //HDC��Windows��һ���������ͣ����豸��������� 
	SetBkMode(hDC, TRANSPARENT);
	SetStretchBltMode(hDC, COLORONCOLOR);
	StretchDIBits(hDC,
		0,
		0,
		drect.right,  //��ʾ���ڿ��  
		drect.bottom,  //��ʾ���ڸ߶�  
		0,
		0,
		disimg.cols,  //��ʾ���ڿ��  
		disimg.rows,     //ͼ��߶�  
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

//���ļ�
BOOL OpenFileDlg(HWND hWnd, TCHAR* szFileSelect)
{
	OPENFILENAME ofn;
	ZeroMemory(&ofn, sizeof(OPENFILENAME));
	//��Ҫ����UNICODE����
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
		//�����ϸ����ķ�����ͬ��ʵ����ֻҪ��OK���ļ�·���ͱ�����ofn.lpstrFile������
		return TRUE;
	}
	return FALSE;
}

void OpenCamera(){
	capture.open(0);
}

//�������
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
//��ʾ��������������
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
		stasm_force_points_into_image(landmarks, img.cols, img.rows);//�����ڷ�Χ����
		for (int i = 0; i <stasm_NLANDMARKS; i++)
		{
			circle(img2, Point(cvRound(landmarks[i * 2]), cvRound(landmarks[i * 2 + 1])), 3, CV_RGB(0, 0, 255), 1, 8, 0);
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
		int start = 0;
		int end = 15;
		for (int i = start + 1; i <end; i++)
		{
			line(img2, Point(cvRound(landmarks[i * 2]), cvRound(landmarks[i * 2 + 1])), Point(cvRound(landmarks[i * 2 - 2]),
				cvRound(landmarks[i * 2 - 1])), CV_RGB(0, 0, 255), 1, 8, 0);
			feature[i] = Point(cvRound(landmarks[i * 2 - 2]), cvRound(landmarks[i * 2 - 1]));
		}
		////	��üë[15~21)��6����;
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
		////   ��üë[21~27)��6����;
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
		//������[27~31)��4����;
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
		//������[32~36)��4����;
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
		////����[37~46)��9����
		start = 37;
		end = 46;
		for (int i = start + 1; i <end; i++)
		{
			line(img2, Point(cvRound(landmarks[i * 2]), cvRound(landmarks[i * 2 + 1])), Point(cvRound(landmarks[i * 2 - 2]), 
				cvRound(landmarks[i * 2 - 1])), CV_RGB(0, 0, 255), 1, 8, 0);
			feature[i] = Point(cvRound(landmarks[i * 2 - 2]), cvRound(landmarks[i * 2 - 1]));
		}
		////��������[48~60)��12����
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
		////��2[60~66)��6����
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
		//������
		start = 68;
		end = 72;
		for (int i = start + 1; i <end; i++)
		{
			line(img2, Point(cvRound(landmarks[i * 2]), cvRound(landmarks[i * 2 + 1])), Point(cvRound(landmarks[i * 2 - 2]), 
				cvRound(landmarks[i * 2 - 1])), CV_RGB(0, 255, 255), 1, 8, 0);
		}
		line(img2, Point(cvRound(landmarks[end * 2 - 2]), cvRound(landmarks[end * 2 - 1])), Point(cvRound(landmarks[start * 2]), 
			cvRound(landmarks[start * 2 + 1])), CV_RGB(0, 255, 255), 1, 8, 0);
		//������
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
//�ڿؼ�����ʾ���������ڵ�ͼƬ
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
//ѡ����Ҫ�򿪵��ļ�����ʾ�ڿؼ���
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
//��ù̶��ļ����£�ѵ���ļ��У�������ͼƬ
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

//ʶ��������
void PicTrain(){
	string path = "data\\att_faces";
	GetAllPic(path, filenames);

	ReadPic(filenames);
	//��ʼѵ��
	Model = cv::face::createLBPHFaceRecognizer();
	Model->train(images, labels);
}

//����ʶ��
void detectFace(){
	Mat im;
	cvtColor(CurrentImg, im, CV_BGR2GRAY);
	int predit = Model->predict(im);
	putText(CurrentImg, to_string(predit), Point(10, 30), CV_FONT_HERSHEY_COMPLEX, 1, Scalar(0,0,255),2);
}

//��ת����LBP  
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

