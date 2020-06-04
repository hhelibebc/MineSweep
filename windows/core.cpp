#include "stdafx.h"
#include "core.h"
#include "resource1.h"

MyApp app;
GameCtrl gc;
HBITMAP bmp;
static char *Class = "DIY window";
static char *Title = "魔改的win32程序";
static const char *pStr;
static const char* StrTabs[] = {"","You are failed!","You are succeed!"};
static MSG msg;
static MessageMap cb_tab[]={
	WM_CREATE,      OnCreate,
	WM_PAINT,		OnPaint,
	WM_CHAR,        OnChar,
	WM_LBUTTONDOWN,	OnLButtonDown,
	WM_MBUTTONDOWN, OnMButtonDown,
	WM_RBUTTONDOWN,	OnRButtonDown,
};
#define CNT	(sizeof(cb_tab)/sizeof(cb_tab[0]))

static bool IsValidCoord(int r,int c){
	return (r>=0 && r<TOTAL_ROW && c>=0 && c<TOTAL_COL);
}
static void DrawBlock(BYTE r,BYTE c,BYTE ind){
	gc.rect.left   = (c << 5) + OFFSET_X;
	gc.rect.top    = (r << 5) + OFFSET_Y;
	gc.rect.right  = gc.rect.left + 32;
	gc.rect.bottom = gc.rect.top + 32;
	gc.ir = ind / 5;
	gc.ic = ind % 5;
	::InvalidateRect(app.wnd.hwnd,&gc.rect,0);
	OnPaint(app.wnd.hwnd,0,0,0);
}
static void DrawString(const char* str){
	pStr = str;
	gc.rect.top = 0;
	gc.rect.left = OFFSET_X;
	gc.rect.right = 400;
	gc.rect.bottom = 20;
	::InvalidateRect(app.wnd.hwnd,&gc.rect,1);
	OnPaint(app.wnd.hwnd,1,0,0);
}
static void DrawAllBlock(){
	for(int i=0;i<TOTAL_ROW;i++){
		for(int j=0;j<TOTAL_COL;j++){
			DrawBlock(i,j,gc.map[i][j]);
		}
	}
}
static void CreateMines(int firstPoint){
	int i = 0,j,v;
	while(i < TOTAL_MINE){
		v = rand()%TOTAL_BLOCK;
		for(j = 0;j<i;j++){
			if(v == firstPoint || v == gc.mines[j])
				break;
		}
		if(j == i){
			gc.mines[j] = v;
			gc.data[v/TOTAL_COL][v%TOTAL_COL] = 1;
			i++;
		}
	}
}
static int  CountRoundMines(int r,int c){
	int res = 0;
	for(int i=r-1;i<r+2;i++){
		for(int j=c-1;j<c+2;j++){
			if(i>=0 && i<TOTAL_ROW && j>=0 && j<TOTAL_COL){
				res += gc.data[i][j];
			}
		}
	}
	return res;
}
static void GameOver(int result){
	for(int i=0;i<TOTAL_ROW;i++){
		for(int j=0;j<TOTAL_COL;j++){
			if(gc.data[i][j] == 1){
				gc.map[i][j] = 11;
				DrawBlock(i,j,11);
			}
		}
	}
	gc.state = result;
	DrawString(StrTabs[result]);
}
static void ResetGame(){
	DrawString(StrTabs[0]);
	gc.state = 0;
	gc.firstClick = 0;
	gc.remain = TOTAL_BLOCK;
	memset(gc.map,0,sizeof(gc.map));
	memset(gc.data,0,sizeof(gc.data));
	DrawAllBlock();
}
static void DisposeOneClick(int r,int c){
	if((gc.map[r][c] == 0 || gc.map[r][c] == 12) && IsValidCoord(r,c)){
		if(gc.data[r][c] == 1)
			GameOver(1);
		else{
			int v1 = CountRoundMines(r,c);
			gc.map[r][c] = v1+2;
			DrawBlock(r,c,v1+2);
			if(--gc.remain <= TOTAL_MINE)
				GameOver(2);
			if(v1 == 0){
				DisposeOneClick(r-1,c-1);
				DisposeOneClick(r-1,c);
				DisposeOneClick(r-1,c+1);
				DisposeOneClick(r,c-1);
				DisposeOneClick(r,c+1);
				DisposeOneClick(r+1,c-1);
				DisposeOneClick(r+1,c);
				DisposeOneClick(r+1,c+1);
			}
		}
	}
}

void CallBack(OnChar){
	switch(wParam&0xff){
	case 's':ResetGame();break;
	default:break;
	}
}
void CallBack(OnCreate){
	bmp = LoadBitmap(app.inst, MAKEINTRESOURCE(IDB_BITMAP1));//加载位图
	gc.remain = TOTAL_BLOCK;
	gc.state = 1;
}
void CallBack(OnPaint){
	PAINTSTRUCT ps;
	app.hdc = BeginPaint(hWnd,&ps);

	if(message == 0){
		HDC hdcMem = (HDC)CreateCompatibleDC(app.hdc);//创建兼容性DC
		SelectObject(hdcMem, bmp);//将位图和兼容性DC绑定
		BitBlt(app.hdc, gc.rect.left, gc.rect.top, 32, 32, hdcMem, gc.ic<<5, gc.ir<<5, SRCCOPY);//将兼容DC的内容复制到hdc上面，也就是显示的窗口上。
		DeleteObject(hdcMem);//删除兼容性DC
	}
	else if(message == 1){
		TextOut(app.hdc,gc.rect.left,gc.rect.top,pStr,strlen(pStr));
	}

	EndPaint(hWnd,&ps);
}
void CallBack(OnLButtonDown){
	if(gc.firstClick == 0){
		gc.firstClick = 1;
		CreateMines(gc.r * TOTAL_COL + gc.c);
	}
	if(gc.firstClick == 1){
		DisposeOneClick(gc.r,gc.c);
	}
}
void CallBack(OnMButtonDown){
	for(int i=gc.r-1;i<gc.r+2;i++){
		for(int j=gc.c-1;j<gc.c+2;j++){
			DisposeOneClick(i,j);
		}
	}
}
void CallBack(OnRButtonDown){
	int v = gc.map[gc.r][gc.c];
	switch(v){
		case 0: v = 13;break;
		case 12:v = 0;break;
		case 13:v = 12;break;
		default:break;
	}
	DrawBlock(gc.r,gc.c,v);
	gc.map[gc.r][gc.c] = v;
}
LRESULT CallBack(WndProc){
	char i;
	switch(message){
	case WM_LBUTTONDOWN:
	case WM_MBUTTONDOWN:
	case WM_RBUTTONDOWN:
		gc.pt.x = lParam&0xffff;
		gc.pt.y = lParam>>16;
		gc.r = (gc.pt.y - OFFSET_Y)>>5;
		gc.c = (gc.pt.x - OFFSET_X)>>5;
		if(!IsValidCoord(gc.r,gc.c) || gc.state != 0)
			return 0;
		break;
	default:break;
	}
	for(i=0;i<CNT;i++){
		if(message == cb_tab[i].msg){
			cb_tab[i].pf(hWnd,message,wParam,lParam);
			return 0;
		}
	}
	return DefWindowProc(hWnd, message, wParam, lParam);
}

int MyWnd::Register(){
	WNDCLASSEX wcex;

	memset(&wcex,0,sizeof(wcex));
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= app.inst;
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszClassName	= Class;

	return RegisterClassEx(&wcex);
}
int MyWnd::Create(){
	hwnd = CreateWindow(Class, Title, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, app.inst, NULL);
	if(NULL == hwnd)
		return 0;
	::ShowWindow(hwnd,SW_SHOW);
	::UpdateWindow(hwnd);
	return 1;
}
int MyApp::Init(HINSTANCE hinst){
	inst = hinst;
	wnd.Register();
	return wnd.Create();
}
int MyApp::Run(){
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return 1;
}