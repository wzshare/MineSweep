// Win32Project3.cpp : 定义应用程序的入口点。
//

#include "stdafx.h"
#include "MineSweep.h"

#define GET_X_LPARAM(lp)			((int)(short)LOWORD(lp))
#define GET_Y_LPARAM(lp)			((int)(short)HIWORD(lp))
#define MAX_LOADSTRING 100
#define DEFAULT_WIDTH 19			//定义边的宽度	width 3+5+3+3+5
#define DEFAULT_HIGHT 66			//定义边的高度	hight 3+5++2+40+2+5+2+2+5
#define N 720						//定义最大砖数，游戏设定最多720个
#define random(x) (rand()%x)		//随机函数
#define ID_TIMER    1

// 全局变量:
HINSTANCE hInst;							// 当前实例
TCHAR szTitle[MAX_LOADSTRING];				// 标题栏文本
TCHAR szWindowClass[MAX_LOADSTRING];		// 主窗口类名
char Brick[N];								//砖块数组				
char Backgroud[N];							//砖块对应下面背景的数组
int Gametime[3], Minecount[3] ;
int wsize, hsize, mcount ;

// 此代码模块中包含的函数的前向声明:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	UserDefined(HWND, UINT, WPARAM, LPARAM);
void				SetWindowSize(HWND, int, int);
void				DrawRect(HDC, int, int, int, int, int, int, int);
int					SetNumber(int);
void				SearchMine(HDC, int, int, int, int, HDC);
void				SetMine(int) ;
void				DrawBlackground(HDC, int, int);
void				RefreshMcount(HDC , int , int , HDC , int);
bool				gamewin();

int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPTSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	MSG msg;
	HACCEL hAccelTable;

	// 初始化全局字符串
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_MINESWEEP, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// 执行应用程序初始化:
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_MINESWEEP));

	// 主消息循环:
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
//  函数: MyRegisterClass()
//
//  目的: 注册窗口类。
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
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_MINESWEEP));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_MINESWEEP);
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

//
//   函数: InitInstance(HINSTANCE, int)
//
//   目的: 保存实例句柄并创建主窗口
//
//   注释:
//
//        在此函数中，我们在全局变量中保存实例句柄并
//        创建和显示主程序窗口。
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd;

   hInst = hInstance; // 将实例句柄存储在全局变量中

   hWnd = CreateWindow(szWindowClass, 
					  szTitle, 
					  WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
					  200, 
					  100, 
					  CW_USEDEFAULT, 
					  CW_USEDEFAULT, 
					  NULL, 
					  NULL, 
					  hInstance, 
					  NULL);

   if (!hWnd)
   {
      return FALSE;
   }

   //SetWindowSize(hWnd, DEFAULT_WIDTH, DEFAULT_HIGHT);

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  函数: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  目的: 处理主窗口的消息。
//
//  WM_COMMAND	- 处理应用程序菜单
//  WM_PAINT	- 绘制主窗口
//  WM_DESTROY	- 发送退出消息并返回
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static HBITMAP	hBitmap, hBitmap1, hBitmap2 ;
	static int		cxClient, cyClient, cxSource, cySource, cxSource1, cySource1, cxSource2, cySource2;
	static int		gtime, m ;
	static BITMAP	bitmap, bitmap1, bitmap2 ;
	int				wmId, wmEvent, x, y, i, xPos, yPos ;
	static bool		gstart, gend;
	PAINTSTRUCT		ps;
	HDC				hdc, hdcMem;

	switch (message)
	{
	case WM_CREATE:
		srand((int)time(0));
		wsize = 9 ;				//初级、中级、高级的宽（砖数）为：	9 16 30
		hsize = 9 ;				//初级、中级、高级的高（砖数）为：	9 16 16
		gtime = 0 ;
		gstart = FALSE ;
		gend = FALSE ;
		mcount = 10 ;
		m = mcount ;
		
		SetMine(mcount) ;		//设置雷区：SetMine
		
		hBitmap = LoadBitmap (hInst, MAKEINTRESOURCE(IDB_BITMAP1)) ;
		hBitmap1 = LoadBitmap (hInst, MAKEINTRESOURCE(IDB_BITMAP2)) ;
		hBitmap2 = LoadBitmap (hInst, MAKEINTRESOURCE(IDB_BITMAP3)) ;
		
		GetObject (hBitmap, sizeof (BITMAP), &bitmap) ;
		GetObject (hBitmap1, sizeof (BITMAP), &bitmap1) ;
		GetObject (hBitmap2, sizeof (BITMAP), &bitmap2) ;
		
		cxSource = bitmap.bmWidth ;
		cySource = bitmap.bmHeight/16 ;
		cxSource1 = bitmap1.bmWidth ;
		cySource1 = bitmap1.bmHeight/12 ;
		cxSource2 = bitmap2.bmWidth ;
		cySource2 = bitmap2.bmHeight/5 ;

		SetWindowSize(hWnd, DEFAULT_WIDTH + cxSource * wsize, DEFAULT_HIGHT + cySource * hsize);
		//设置窗口大小

		return 0 ;

	case WM_TIMER :
		gtime ++ ;
		Gametime[0] = gtime / 100 ;
		Gametime[1] = (gtime - Gametime[0] * 100) / 10 ;
		Gametime[2] = gtime % 10 ;
		hdc = GetDC(hWnd);
		hdcMem = CreateCompatibleDC (hdc) ;
		SelectObject (hdcMem, hBitmap1) ;
		x = cxClient - (36 + 15);
		y = 17 ;
		for(i = 0 ;i < 3 ; i ++) 
		{
			x = cxClient - (36 + 15) + i * 12;
			y = 17 ;
			BitBlt (hdc, x, y, cxSource1, cySource1, hdcMem, 0, (11 - Gametime[i]) * 23, SRCCOPY) ;
		}
		DeleteDC (hdcMem) ;
		return 0 ;

	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// 分析菜单选择:
		switch (wmId)
		{
		case IDM_32772: //开局
			
		//SetMine
			SetMine(mcount) ;
			KillTimer (hWnd, ID_TIMER) ;
			gstart = FALSE ;
			gend = FALSE ;
			InvalidateRect (hWnd, NULL, TRUE) ;
			break ;

		case IDM_32773: //初级
			wsize = 9 ;
			hsize = 9 ;
			mcount = 10 ;
			m = mcount ;
			KillTimer (hWnd, ID_TIMER) ;
			gstart = FALSE ;
			gend = FALSE ;
			SetMine(mcount) ;
			SetWindowSize(hWnd, DEFAULT_WIDTH + cxSource * wsize, DEFAULT_HIGHT + cySource * hsize);
			break ;

		case IDM_32774: // 中级
			wsize = 16 ;
			hsize = 16 ;
			mcount = 40 ;
			m = mcount ;
			KillTimer (hWnd, ID_TIMER) ;
			gstart = FALSE ;
			gend = FALSE ;
			SetMine(mcount) ;
			SetWindowSize(hWnd, DEFAULT_WIDTH + cxSource * wsize, DEFAULT_HIGHT + cySource * hsize);
			break;

		case IDM_32775: // 高级
			wsize = 30 ;
			hsize = 16 ;
			mcount = 99 ;
			m = mcount ;
			KillTimer (hWnd, ID_TIMER) ;
			gstart = FALSE ;
			gend = FALSE ;
			SetMine(mcount) ;
			SetWindowSize(hWnd, DEFAULT_WIDTH + cxSource * wsize, DEFAULT_HIGHT + cySource * hsize);
			break ;

		case IDM_32776:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_USERDEFINED), hWnd, UserDefined);
			SetWindowSize(hWnd, DEFAULT_WIDTH + cxSource * wsize, DEFAULT_HIGHT + cySource * hsize);
			break;

		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;

		case IDM_EXIT:
			KillTimer (hWnd, ID_TIMER) ;
			DestroyWindow(hWnd);
			break;

		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;

	case WM_SIZE:
		cxClient = LOWORD (lParam) ;
		cyClient = HIWORD (lParam) ;
		return 0 ;

	case WM_LBUTTONUP :
		hdc = GetDC(hWnd);
		hdcMem = CreateCompatibleDC (hdc) ;
		SelectObject (hdcMem, hBitmap2) ;

		xPos = GET_X_LPARAM(lParam) ; 
		yPos = GET_Y_LPARAM(lParam) ;

		if(yPos > 17 && yPos < 41 && xPos > cxClient / 2 - 12 && xPos < cxClient / 2 + 12)
		{
			SetMine(mcount) ;
			m = mcount ;
			KillTimer (hWnd, ID_TIMER) ;
			gstart = FALSE ;
			gend = FALSE ;
			InvalidateRect (hWnd, NULL, TRUE) ;
		}
		if(!gend)
		{
			x = cxClient / 2 - 12 ;
			y = 17 ;
			BitBlt (hdc, x, y, cxSource2, cySource2, hdcMem, 0, 96, SRCCOPY) ;
		}

		SelectObject (hdcMem, hBitmap) ;
		SetViewportOrgEx(hdc,11,58,NULL);
		if(xPos >= 11 && xPos < 11 + cxSource * wsize && yPos >= 58 && yPos < 58 + cySource * hsize && gstart == FALSE)
		//如果点击在雷区并且游戏未开始（game start）
		{
			gstart = TRUE ;
			gtime = 1 ;
			Gametime[2] = 1 ;
			InvalidateRect (hWnd, NULL, TRUE) ;
			SetTimer (hWnd, ID_TIMER, 1000, NULL) ;
		}

		x = (xPos - 11) / cxSource * cxSource;
		y = (yPos - 58) / cySource * cySource;
		i = x / cxSource + y / cySource * wsize;
		if(!gend)
		//如果游戏没有结束（game end）
		{
			if(Brick[i] == 5)
			{
				KillTimer (hWnd, ID_TIMER) ;
				Brick[i] = 3 ;
				BitBlt (hdc, x, y, cxSource, cySource, hdcMem, 0, Brick[i] * 16, SRCCOPY) ;
				Backgroud[i] = Brick[i];
				gend = TRUE;

				SelectObject (hdcMem, hBitmap2) ;
				SetViewportOrgEx(hdc,0,0,NULL);
				x = cxClient / 2 - 12 ;
				y = 17 ;
				BitBlt (hdc, x, y, cxSource2, cySource2, hdcMem, 0, 48, SRCCOPY) ;
			}
			else 
				SearchMine(hdc, x, y, cxSource, cySource, hdcMem);
		}

		if(gamewin())
		{
			KillTimer (hWnd, ID_TIMER) ;
			Brick[i] = 3 ;
			gend = TRUE;

			SelectObject (hdcMem, hBitmap2) ;
			SetViewportOrgEx(hdc,0,0,NULL);
			x = cxClient / 2 - 12 ;
			y = 17 ;
			BitBlt (hdc, x, y, cxSource2, cySource2, hdcMem, 0, 24, SRCCOPY) ;
		}
		
		DeleteDC (hdcMem) ;
		return 0 ;
		
	case WM_LBUTTONDOWN:
		xPos = GET_X_LPARAM(lParam) ; 
		yPos = GET_Y_LPARAM(lParam) ;
		hdc = GetDC(hWnd);
		hdcMem = CreateCompatibleDC (hdc) ;
		SelectObject (hdcMem, hBitmap2) ;
		
		if(yPos > 17 && yPos < 41 && xPos > cxClient / 2 - 12 && xPos < cxClient / 2 + 12)
		{
			x = cxClient / 2 - 12 ;
			y = 17 ;
			BitBlt (hdc, x, y, cxSource2, cySource2, hdcMem, 0, 0, SRCCOPY) ;
		}
		else if(!gend)
		{
			x = cxClient / 2 - 12 ;
			y = 17 ;
			BitBlt (hdc, x, y, cxSource2, cySource2, hdcMem, 0, 72, SRCCOPY) ;
		}
		SelectObject (hdcMem, hBitmap) ;
		SetViewportOrgEx(hdc,11,58,NULL);
		if(!gend)
		//如果游戏没有结束（game end）
		{
			x = (xPos - 11) / cxSource * cxSource;
			y = (yPos - 58) / cySource * cySource;
			i = x / cxSource + y / cySource * wsize;
			if(xPos >= 11 && xPos < 11 + cxSource * wsize && yPos >= 58 
				&& yPos < 58 + cySource * hsize && Backgroud[i] == 0)
			{
				BitBlt (hdc, x, y, cxSource, cySource, hdcMem, 0, 15 * 16, SRCCOPY) ;
			}
		}
		DeleteDC (hdcMem) ;
		return 0 ;

	case WM_RBUTTONDOWN :
		xPos = GET_X_LPARAM(lParam) ; 
		yPos = GET_Y_LPARAM(lParam) ;
		hdc = GetDC(hWnd);
		SetViewportOrgEx(hdc,11,58,NULL);
		hdcMem = CreateCompatibleDC (hdc) ;
		SelectObject (hdcMem, hBitmap) ;
		if(!gend && xPos >= 11 && xPos < 11 + cxSource * wsize && yPos >= 58 
				&& yPos < 58 + cySource * hsize)
		//如果游戏没有结束（game end）
		{
			x = (xPos - 11) / cxSource * cxSource;
			y = (yPos - 58) / cySource * cySource;
			i = x / cxSource + y / cySource * wsize;
			if(Backgroud[i] == 0 && m >0)
			{
				Backgroud[i] = 1 ;
				BitBlt (hdc, x, y, cxSource, cySource, hdcMem, 0, Backgroud[i] * 16, SRCCOPY) ;
				m -- ;
				SelectObject (hdcMem, hBitmap1) ;
				SetViewportOrgEx(hdc,0,0,NULL);
				RefreshMcount(hdc, cxSource1, cySource1, hdcMem, m) ;
			}
			else if(Backgroud[i] == 1)
			{
				Backgroud[i] = 0 ;
				BitBlt (hdc, x, y, cxSource, cySource, hdcMem, 0, Backgroud[i] * 16, SRCCOPY) ;
				m ++ ;
				SelectObject (hdcMem, hBitmap1) ;
				SetViewportOrgEx(hdc,0,0,NULL);
				RefreshMcount(hdc, cxSource1, cySource1, hdcMem, m) ;
			}
		}

		if(gamewin())
		{
			KillTimer (hWnd, ID_TIMER) ;
			Brick[i] = 3 ;
			gend = TRUE;

			SelectObject (hdcMem, hBitmap2) ;
			SetViewportOrgEx(hdc,0,0,NULL);
			x = cxClient / 2 - 12 ;
			y = 17 ;
			BitBlt (hdc, x, y, cxSource2, cySource2, hdcMem, 0, 24, SRCCOPY) ;
		}
		DeleteDC (hdcMem) ;
		return 0 ;

	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		// TODO: 在此添加任意绘图代码...
		
		DrawBlackground(hdc, cxClient, cyClient) ;		//Draw Blackgroud

		/**/
		//bricks
		hdcMem = CreateCompatibleDC (hdc) ;
		SelectObject (hdcMem, hBitmap) ;
		for (y = 58 ; y < cyClient - 8 ; y += cySource  )
		{
			for (x = 11 ; x < cxClient - 8 ; x += cxSource )
			{
				i = (x - 11) / cxSource + (y - 58) / cySource * wsize;
				BitBlt (hdc, x, y, cxSource, cySource, hdcMem, 0, Backgroud[i] * 16, SRCCOPY) ;
			}
		}
		
		//timer
		SelectObject (hdcMem, hBitmap1) ;
		for(i = 0 ;i < 3 ; i ++) 
		{
			x = cxClient - (36 + 15) + i * 12;
			y = 17 ;
			BitBlt (hdc, x, y, cxSource1, cySource1, hdcMem, 0, (11 - Gametime[i]) * 23, SRCCOPY) ;
		}

		//Minecount
		for(i = 0 ;i < 3 ; i ++) 
		{
			x = 17 + i * 12;
			y = 17 ;
			BitBlt (hdc, x, y, cxSource1, cySource1, hdcMem, 0, (11 - Minecount[i]) * 23, SRCCOPY) ;
		}

		//Button
		SelectObject (hdcMem, hBitmap2) ;
		x = cxClient / 2 - 12 ;
		y = 17 ;
		BitBlt (hdc, x, y, cxSource2, cySource2, hdcMem, 0, 96, SRCCOPY) ;
		DeleteDC (hdcMem) ;
		EndPaint(hWnd, &ps);
		break;

	case WM_DESTROY:
		KillTimer (hWnd, ID_TIMER) ;
		PostQuitMessage(0);
		break;

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// “关于”框的消息处理程序。
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

// “自定义”框的消息处理程序。
INT_PTR CALLBACK UserDefined(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	int w, h, m ;
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		SetDlgItemInt(hDlg, 141, wsize, TRUE);
		SetDlgItemInt(hDlg, 142, hsize, TRUE);
		SetDlgItemInt(hDlg, 143, mcount, TRUE);
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK )
		{
			w = GetDlgItemInt(hDlg, 141, NULL, TRUE) ;
			h = GetDlgItemInt(hDlg, 142, NULL, TRUE) ;
			m = GetDlgItemInt(hDlg, 143, NULL, TRUE) ;
			if(w >= 9 && w <= 26)
				wsize = w ;
			else if(w < 9)
				wsize = 9 ;
			else wsize = 26 ;
			if(h >= 9 && h <= 30)
				hsize = h ;
			else if(h < 9)
				hsize = 9 ;
			else hsize = 30 ;
			if(m >= 10 && m <= (wsize - 1) * (hsize - 1))
				mcount = m ;
			else if (m < 10)
				mcount = 10 ;
			else mcount = (wsize - 1) * (hsize - 1) ;
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		else if (LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}

//
//  函数: SetWindowSize(HWND, int, int)
//
//  目的: 设置窗口大小。
//
//	width	-	窗口的宽
//	hight	-	窗口的高
//
void SetWindowSize(HWND hWnd, int width, int hight)  
{  
	RECT WindowRect;  
	RECT ClientRect;  
	GetWindowRect(hWnd, &WindowRect);  
	GetClientRect(hWnd, &ClientRect);  
	WindowRect.right += (width - ClientRect.right);  
	WindowRect.bottom += (hight - ClientRect.bottom);  
	MoveWindow(hWnd, WindowRect.left, WindowRect.top, WindowRect.right-WindowRect.left, 
		WindowRect.bottom-WindowRect.top, true);  
	InvalidateRect (hWnd, NULL, TRUE) ;
}

//
//  函数: DrawRect(HDC, int, int, int, int, int, int, int)
//
//  目的: 绘制矩形，无返回值。
//	
//	xLeft, yTop		-	矩形左上角
//	xRight, yDown	-	矩形右下角
//	R, G, B			-	矩形颜色
//
void DrawRect(HDC hdc, int xLeft, int yTop, int xRight, int yDown, int R, int G, int B)
{
	RECT			rect ;
	HBRUSH			hBrush ;
	SetRect (&rect, xLeft, yTop, xRight, yDown) ;
	hBrush = CreateSolidBrush ( RGB (R, G, B)) ;
	FillRect (hdc, &rect, hBrush) ;
}

//
//	函数：DrawBlackground(HDC, intt, intt)
//
//	目的：在客户区画背景框架。
//
//	cxClient, cyClient	-	窗口的宽和高
//
void DrawBlackground(HDC hdc, int cxClient, int cyClient)
{
	DrawRect(hdc, 3, 3, cxClient, cyClient, 192, 192, 192);
	DrawRect(hdc, 8, 8, cxClient - 5, 10, 128, 128, 128);
	DrawRect(hdc, 8, 8, 10, 47, 128, 128, 128);
	DrawRect(hdc, 10, 45, cxClient - 5, 47, 255, 255, 255);
	DrawRect(hdc, cxClient - 7, 8, cxClient - 5, 47, 255, 255, 255);
	DrawRect(hdc, 8, 55, cxClient - 5, cyClient - 5, 128, 128, 128);
	DrawRect(hdc, 11, 58, cxClient - 5, cyClient - 5, 255, 255, 255);
}

//
//	函数：SetNumber(int)
//	
//	目的：设置雷区的数字，返回(int)当前统计数字。
//
int SetNumber(int i)
{
	int x, y ,j, cout = 0;
	int t[8];
	x = i / wsize;
	y = i % wsize;
	if( x - 1 >= 0 && y - 1 >= 0 )
		t[0] =  (x - 1) * wsize + (y - 1);
	else
		t[0] = -1;
	if(y - 1 >= 0)
		t[1] =  x * wsize + (y - 1); 
	else
		t[1] = -1;
	if(x + 1 < hsize && y - 1 >= 0)
		t[2] =  (x + 1) * wsize + (y - 1); 
	else
		t[2] = -1;
	if( x - 1 >= 0)
		t[3] =  (x - 1) * wsize + y ;
	else
		t[3] = -1;
	if(x + 1 < hsize)
		t[4] =  (x + 1) * wsize + y ;
	else
		t[4] = -1;
	if(x - 1 >= 0 && y + 1 < wsize)
		t[5] =  (x - 1) * wsize + (y + 1);
	else
		t[5] = -1;
	if(y + 1 < wsize)
		t[6] =  x * wsize + (y + 1);
	else
		t[6] = -1;
	if(x + 1 < hsize && y + 1 < wsize)
		t[7] =  (x + 1) * wsize + (y + 1);
	else
		t[7] = -1;
	for(j = 0; j < 8; j ++)
	{
		if((t[j] >= 0) && (Brick[t[j]] == 5) )
			cout ++;
	}
	return cout;
}

//
//	函数：SearchMine(HDC , int , int , int , int , HDC)
//
//	目的：搜索雷，无雷的地方会被打开。
//
void SearchMine(HDC hdc, int x, int y, int cxSource, int cySource, HDC hdcMem)
{
	int i;
	i = x / cxSource + y / cySource * wsize;
	/*鼠标位置超出雷区或者鼠标位置已经打开，则退出*/
	if(x < 0 || x >= cxSource * wsize || y < 0 || y >= cySource * hsize || i < 0 || Backgroud[i] != 0)
		return;
	if(Brick[i] > 6 && Brick[i] < 15)
	{
		BitBlt (hdc, x, y, cxSource, cySource, hdcMem, 0, Brick[i] * 16, SRCCOPY) ;
		Backgroud[i] = Brick[i];
		return;
	}
	if(Brick[i] == 15)
	{
		BitBlt (hdc, x, y, cxSource, cySource, hdcMem, 0, Brick[i] * 16, SRCCOPY) ;
		Backgroud[i] = Brick[i];
	}
	SearchMine(hdc, x, y - cySource, cxSource, cySource, hdcMem );
	SearchMine(hdc, x - cxSource, y, cxSource, cySource, hdcMem );
	SearchMine(hdc, x, y + cySource, cxSource, cySource, hdcMem );
	SearchMine(hdc, x + cxSource, y, cxSource, cySource, hdcMem );
}

//
//	函数：SetMine(int)
//
//	目的：设置雷区，包括雷数，时间。
//
void SetMine(int count)
{
	int i ,t ;
	int brickcount = wsize * hsize ;
	for(i = 0; i < N; i ++)
	{
		Backgroud[i] = 0 ;
		Brick[i] = 15;
	}
	for(i = 0; i < count; i ++)
	{
		t = random(brickcount);
		if(Brick[t] == 15)
			Brick[t] = 5;
		else
			i --;
	}
	for(i = 0 ; i < brickcount; i++)
	{
		if(Brick[i] != 5)
			Brick[i] = 15 - SetNumber(i);
	}
	
	//左上角雷的个数
	Minecount[0] = count / 100 ;
	Minecount[1] = (count - Minecount[0] * 100) / 10 ;
	Minecount[2] = count % 10 ;
	
	//时间归零
	for(i = 0; i < 3; i ++)
	{
		Gametime[i] = 0 ;
	}
}

//
//	函数：RefreshMcount(HDC , int , int , HDC , int)
//
//	目的：刷新剩余雷的数目。
//
//	cxSource1	-	一个数字位图的宽
//	cySource1	-	一个数字位图的高
//	count		-	剩余雷数
//
void RefreshMcount(HDC hdc, int cxSource1, int cySource1, HDC hdcMem, int count)
{
	int i, x, y;

	Minecount[0] = count / 100 ;
	Minecount[1] = (count - Minecount[0] * 100) / 10 ;
	Minecount[2] = count % 10 ;

	for(i = 0 ;i < 3 ; i ++) 
	{
		x = 17 + i * 12;
		y = 17 ;
		BitBlt (hdc, x, y, cxSource1, cySource1, hdcMem, 0, (11 - Minecount[i]) * 23, SRCCOPY) ;
	}
}

//
//	函数：gamewin()
//
//	目的：游戏是否胜利，胜利返回TRUE，没有胜利返回FALSE。
//
bool gamewin()
{
	int i, t ;
	t = 0 ;
	for(i = 0; i < wsize * hsize; i ++)
	{
		if(Backgroud[i] == 0 || Backgroud[i] == 1)
			t ++ ;
	}
	if(t == mcount)
		return TRUE ;
	else return FALSE ;
}