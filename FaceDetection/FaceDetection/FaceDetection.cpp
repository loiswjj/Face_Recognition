// FaceDetection.cpp : Defines the entry point for the application.
//

#include "FaceDetection.h"

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name

// Forward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPTSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

 	// TODO: Place code here.
	MSG msg;
	HACCEL hAccelTable;

	// Initialize global strings
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_FACEDETECTION, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_FACEDETECTION));

	// Main message loop:
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int) msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_FACEDETECTION));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_FACEDETECTION);
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd;

   hInst = hInstance; // Store instance handle in our global variable

   hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
	   CW_USEDEFAULT, 0, 600, 400, NULL, NULL, hInstance, NULL);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   Init();
   PicTrain();
   return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;
	static HWND Pic;
	static HWND hButton[3];
	GetModuleFileName(NULL, FileDataBase, sizeof(FileDataBase));

	switch (message)
	{
	case WM_CREATE:
		Pic = CreateWindow(TEXT("static"), 0, WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON,
			0, 0, 400, 400, hWnd, (HMENU)0, hInst, NULL);
		hButton[0] = CreateWindow(TEXT("button"), _T("人脸检测"), WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON,
			450, 100, 80, 30, hWnd, (HMENU)1, hInst, NULL);
		hButton[1] = CreateWindow(TEXT("button"), _T("特征点识别"), WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON,
			450, 160, 80, 30, hWnd, (HMENU)2, hInst, NULL);
		hButton[2] = CreateWindow(TEXT("button"), _T("人脸识别"), WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON,
			450, 220, 80, 30, hWnd, (HMENU)3, hInst, NULL);
	/*	hButton[3] = CreateWindow(TEXT("button"), _T("人脸识别"), WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON,
			450, 220, 80, 30, hWnd, (HMENU)4, hInst, NULL);*/
		break;
	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// Parse the menu selections:
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_PHOTO:
			Openphoto(hWnd,Pic);
			break;
		case IDM__OPENCAMERA:
			OpenCamera();
			DrawCameraPic(Pic);
			break;
		case ID_INIT_TRAIN:
			PicTrain();
			break;
		case ID_INIT_COLLECTPIC:
			isCollect = true;
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		case 2:
			//特征点
			faceDis = false;
			featureDis = true;
			detectFeatures(CurrentImg);
			showMatImgToWnd(Pic, CurrentImg);
			break;
		case 1:
			//人脸检测
			featureDis = false;
			faceDis = true;
			detectAndDisplay(CurrentImg);
			showMatImgToWnd(Pic, CurrentImg);
			break;
		case 3:
			//人脸识别
			detectFace();
			showMatImgToWnd(Pic, CurrentImg);
			break;
		case 4:
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		// TODO: Add any drawing code here...
		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}
