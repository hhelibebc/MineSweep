#include "stdafx.h"
#include "winApp.h"
#include "core.h"

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	if(!app.Init(hInstance))
		return 0;
	return app.Run();
}
