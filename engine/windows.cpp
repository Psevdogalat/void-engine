#include <windows.h>
#include <engine.hpp>
#include <debug.h>

static struct{
	UINT width;
	UINT height;
}resolutions[] = 
{
	{640, 420},
	{960, 630}
};

using namespace VoidEngine;

static const UINT 		iResolution 	= 1;
static const UINT 		averFrames 		= 10;	
static const double 	frameLock 		= 30.0;

static HWND				hMainWindow = INVALID_HANDLE_VALUE;
static const LPCSTR 	mainWindowClassName	= "voidEngineMainWindow";
static const DWORD		mainWindowStyle = WS_CAPTION | WS_SYSMENU;

static UINT 			mainWindowWidth;
static UINT 			mainWindowHeight;

LRESULT CALLBACK mainWindowProcedure(HWND, UINT, WPARAM, LPARAM);

void dispatchEngineMessage(Engine * engine, const MSG * winMessage);

UINT getMainWindowWidth()
{
	return resolutions[iResolution].width;
};

UINT getMainWindowHeight()
{
	return resolutions[iResolution].height;
};

HWND createMainWindow(UINT width, UINT height)
{
	HINSTANCE instance;
	RECT areaSizeRect;

	instance  = GetModuleHandle(NULL);

	ZeroMemory(&areaSizeRect,sizeof(areaSizeRect));
	areaSizeRect.right	= width;
	areaSizeRect.bottom	= height;
	
	AdjustWindowRect(&areaSizeRect, winStyle, false);
	width  = areaSizeRect.right - areaSizeRect.left;
	height = areaSizeRect.bottom - areaSizeRect.top;

	
	hMainWindow	= CreateWindowEx(0, mainWindowClassName, NULL, 
		mainWindowStyle, CW_USEDEFAULT, CW_USEDEFAULT, width, height, 
		NULL, NULL, instance, NULL);

	return hMainWindow;
}


HWND getMainWindow()
{
	if(hMainWindow != INVALID_HANDLE_VALUE)
		return hMainWindow;
	
	hMainWindow = createMainWindow( getMainWindowWidth(), 
		getMainWindowHeight());
	
	return hMainWindow;
};


int Engine::initPlatform()
{
	
	HINSTANCE 		instance;
	WNDCLASSEX		winClass;
	
	instance  = GetModuleHandle(NULL);

	ZeroMemory(&winClass,sizeof(winClass));
	winClass.cbSize 		= sizeof(winClass);
	winClass.lpszClassName	= mainWindowClassName;
	winClass.hInstance		= instance;
	winClass.style			= CS_OWNDC;
	winClass.lpfnWndProc	= mainWindowProcedure;
	winClass.hbrBackground	= (HBRUSH)GetStockObject(BLACK_BRUSH);
	winClass.hCursor		= LoadCursor(nullptr, IDC_ARROW);
	
	if(!RegisterClassEx(&winClass)){
		DEBUG("Window class ""%s"" registration error!\n", winClassName);
		return 1;
	}
	
	frameTimer = SetTimer(NULL ,0,((UINT)(1000.0/frameLock))-10, nullptr);
	frameLastTick = GetTickCount();

};

int Engine::loopPlatform()
{
	MSG message;

	if(hMainWindow == INVALID_HANDLE_VALUE)
	{
		DEBUG("MainWindow not found! abort ...\n");
		return 1;
	}
	
	ShowWindow(hMainWindow, SW_NORMAL);

	while(GetMessage(&message, NULL, 0, 0)){
		TranslateMessage(&message);
		DispatchMessage(&message);
		dispatchEngineMessage(this, message);
	}

	return 0;
};

int Engine::freePlatform()
{
	if(hMainWindow != INVALID_HANDLE_VALUE)
		DestroyWindow(hMainWindow);

	KillTimer(NULL ,frameTimer);
	
};
	

LRESULT CALLBACK mainWindowProcedure(HWND wnd, UINT msg, 
	WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT paintStruct;
	
	switch(Message){
		case WM_CLOSE:
			PostQuitMessage(0);
			
		case WM_PAINT:
			engine = ;
			BeginPaint(wnd, &paintStruct);
			render((Engine *)wParam, paintStruct.hdc);
			EndPaint(wnd, &paintStruct);
			
		break;
		default:
			return DefWindowProc(wnd, msg, w, L_param);
	}
	return 0;
}


double transformMouseX(UINT x, UINT width)
{
	return (2.0 * (double)x / (double)width) - 1.0;
}

double transformMouseY(UINT y, UINT height)
{
	return (2.0 * (double)(height - y) / (double)height) - 1.0;
}

void dispatchEngineMessage(Engine * engine, const MSG winMessage)
{
		VoidEngine::EVENT_MSG event;
		VoidEngine::EVENT_KEY_RECORD& 	key   = event.event_record.key;
		VoidEngine::EVENT_MOUSE_RECORD&	mouse = event.event_record.mouse;

		ZeroMemory(&event,sizeof(event));
		
		switch(winMessager->message){
			case WM_KEYDOWN:
			case WM_KEYUP:
				event.event = EV_KEY_PRESS;
				if(winMessage->message == WM_KEYDOWN)	
					key.key_down = true;
				
				key.key_code = winMessage->wParam; 
				engine->handleEvent(event);
			break;
			case WM_MOUSEWHEEL:
				event.event = EV_MOUSE_WHEEL;
				mouse.delta = GET_WHEEL_DELTA_WPARAM(winMessage->wParam);
				mouse.x     = transform_mouse_x(LOWORD(winMessage->lParam));
				mouse.y		= transform_mouse_y(HIWORD(winMessage->lParam));
				engine->handleEvent(event);
			break;
			case WM_MOUSEMOVE:
				event.event = EV_MOUSE_MOVE;
				mouse.x     = transform_mouse_x(LOWORD(winMessage->lParam));
				mouse.y		= transform_mouse_y(HIWORD(winMessage->lParam));
				engine->handleEvent(event);
			break;
			case WM_MBUTTONDOWN:
				event.event = EV_MOUSE_MKEY_DOWN;
				mouse.x     = transform_mouse_x(LOWORD(winMessage->lParam));
				mouse.y		= transform_mouse_y(HIWORD(winMessage->lParam));
				engine->handleEvent(event);
			break;
			case WM_MBUTTONUP:
				engine_event.event = EV_MOUSE_MKEY_UP;
				mouse.x     = transform_mouse_x(LOWORD(winMessage->lParam));
				mouse.y		= transform_mouse_y(HIWORD(winMessage->lParam));
				engine->handleEvent(event);
			break;
			case WM_LBUTTONDOWN:
				engine_event.event = EV_MOUSE_LKEY_DOWN;
				mouse.x     = transform_mouse_x(LOWORD(winMessage->lParam));
				mouse.y		= transform_mouse_y(HIWORD(winMessage->lParam));
				engine->handleEvent(event);
			break;
			case WM_LBUTTONUP:
				engine_event.event = EV_MOUSE_LKEY_UP;
				mouse.x     = transform_mouse_x(LOWORD(winMessage->lParam));
				mouse.y		= transform_mouse_y(HIWORD(winMessage->lParam));
				engine->handleEvent(event);
			break;
			case WM_RBUTTONDOWN:
				engine_event.event = EV_MOUSE_RKEY_DOWN;
				mouse.x     = transform_mouse_x(LOWORD(winMessage->lParam));
				mouse.y		= transform_mouse_y(HIWORD(winMessage->lParam));
				engine->handleEvent(event);
			break;
			case WM_RBUTTONUP:
				engine_event.event = EV_MOUSE_RKEY_UP;
				mouse.x     = transform_mouse_x(LOWORD(winMessage->lParam));
				mouse.y		= transform_mouse_y(HIWORD(winMessage->lParam));
				engine->handleEvent(event);
			break;
			case WM_TIMER:
				engine->calculate();
				if(hMainWindow != INVALID_HANDLE_VALUE)
					InvalidateRect(hMainWindow, nullptr, false);

			break;
		};
		
};

bool Engine::getAbsolutePath(std::string& pathAbsolute, 
	const std::string& path)
{
	char buffer[MAX_PATH];
	if(GetFullPathName(path.c_str(), MAX_PATH, buffer, nullptr) != 0)
	{
		pathAbsolute = (const char*)buffer;
		return true;
	}
	
	return false;
}
