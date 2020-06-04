#pragma once

#define TOTAL_ROW 20
#define TOTAL_COL 40
#define TOTAL_MINE 50
#define TOTAL_BLOCK (TOTAL_ROW*TOTAL_COL)
#define OFFSET_X 30
#define OFFSET_Y 30

class MyWnd{
public:
	int Register();
	int Create();
public:
	HWND hwnd;
};

class MyApp{
public:
	int Init(HINSTANCE hInst);
	int Run();
public:
	MyWnd wnd;
	HINSTANCE inst;
	HDC hdc;
	HPEN	pen;
	HBRUSH	brush;
};

class GameCtrl{
public:
	POINT pt;
	RECT  rect;
	BYTE  r,c,ir,ic,state;
	BYTE  firstClick;
	BYTE  map[TOTAL_ROW][TOTAL_COL];
	BYTE  data[TOTAL_ROW][TOTAL_COL];
	WORD  mines[TOTAL_MINE];
	WORD  remain;
};

typedef void (__stdcall *pCallBack)(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
#define CallBack(name) __stdcall name(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
typedef struct{
	int msg;
	pCallBack	pf;
}MessageMap;

extern GameCtrl gc;
extern MyApp app;
extern void CallBack(OnCreate);
extern void CallBack(OnChar);
extern void CallBack(OnPaint);
extern void CallBack(OnLButtonDown);
extern void CallBack(OnMButtonDown);
extern void CallBack(OnRButtonDown);
