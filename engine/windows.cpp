#include <windows.h>

#include <gl/glew.h>
#include <gl/wglew.h>
#include <cstdio>

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

static const UINT 		iResolution 		= 1;
static const UINT 		aver_frames 		= 10;	
static const double 	frame_lock 			= 30.0;

static const LPCSTR 	mainWindowClassName	= "voidEngineMainWindow";
static const DWORD		mainWindowStyle = WS_CAPTION | WS_SYSMENU;
static UINT 			mainWindowWidth;
static UINT 			mainWindowHeight;


static HGLRC 			render_context;
static UINT_PTR			frame_timer;
static DWORD			frame_sys_tick_last;
static DWORD			frame_sys_tick;
static double			frame_time;
static double 			aver_frame_time = 0;
static UINT				aver_cnt 		= 0;
static double			frame_counter;

LRESULT CALLBACK mainWindowProcedure(HWND, UINT, WPARAM, LPARAM);

void dispatch_engine_message(const MSG* );
void print_gl_info();

HWINDOW createMainWindow(UINT width, UINT height)
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
	
	return CreateWindowEx(0, mainWindowClassName, NULL, 
		mainWindowStyle, CW_USEDEFAULT, CW_USEDEFAULT, width, height, 
		NULL, NULL, instance, NULL);

}

int Engine::initPlatform()
{
	
	WNDCLASSEX				winClass;


	
	winWidth  = resolutions[iResolution].width;
	winHeight = resolutions[iResolution].height;
	
	ZeroMemory(&winClass,sizeof(winClass));
	winClass.cbSize 		= sizeof(winClass);
	winClass.lpszClassName	= mainWindowClassName;
	winClass.hInstance		= instance;
	winClass.style			= CS_OWNDC;
	winClass.lpfnWndProc	= winProc;
	winClass.hbrBackground	= (HBRUSH)GetStockObject(BLACK_BRUSH);
	winClass.hCursor		= LoadCursor(nullptr, IDC_ARROW);
	
	if(!RegisterClassEx(&winClass)){
		DEBUG("Window class ""%s"" registration error!\n", winClassName);
		return 1;
	}
	
		//printf("Timer val %d\n", (UINT)(1000.0/frame_lock));
	
	frame_timer = SetTimer(main_window,0,((UINT)(1000.0/frame_lock))-10,nullptr);
	frame_sys_tick_last = GetTickCount();
	
	ShowWindow(main_window,SW_NORMAL);
	
	MSG message;
	while(GetMessage(&message, NULL, 0, 0)){
		TranslateMessage(&message);
		DispatchMessage(&message);
		dispatch_engine_message(&message);
	}
	
	printf("App closing ... \n");
	ENGINE::eng_free();
	KillTimer(main_window,frame_timer);
	
	wglMakeCurrent	(nullptr, nullptr);
	wglDeleteContext(render_context);
	
	
	return 0;
}

LRESULT CALLBACK mainWindowProcedure(HWND Window, UINT Message, 
	WPARAM W_param, LPARAM L_param)
{
	PAINTSTRUCT paint_struct;
	LPSTR		window_title;
	static const UINT window_title_max_len = 50;
	
	switch(Message){
		case WM_CLOSE:
			ENGINE::terminate();
			
		case WM_PAINT:
			
			if(render_context != INVALID_HANDLE_VALUE){
				BeginPaint(Window,&paint_struct);
				wglMakeCurrent(paint_struct.hdc, render_context);
			
				ENGINE::eng_render();
				
				SwapBuffers(paint_struct.hdc);
				wglMakeCurrent(NULL, NULL);
				EndPaint(Window,&paint_struct);
				
				frame_sys_tick = GetTickCount();
				if(frame_sys_tick_last < frame_sys_tick){
					frame_time 		= 0.001 * (double)(frame_sys_tick - frame_sys_tick_last);
				}
				frame_sys_tick_last = frame_sys_tick;
				
				if(aver_cnt < aver_frames){
					aver_frame_time += frame_time;
					aver_cnt++;
				}else{
					aver_frame_time /= aver_frames; 
					frame_counter 	= (1.0f)/aver_frame_time;
					aver_frame_time = 0;
					aver_cnt 		= 0;
				}
				
				window_title = (LPSTR)malloc(sizeof(char)*(window_title_max_len+1));
				sprintf(window_title, "%s %dx%d :: fps %2.2f", main_window_header, main_window_width, main_window_height, frame_counter);
				SetWindowText(Window, window_title);
				free(window_title);
			}
		break;
		case  WM_TIMER:
			ENGINE::eng_compute(frame_time);
			InvalidateRect(Window, nullptr, false);
		break;
		default:
			return DefWindowProc(Window, Message, W_param, L_param);
	}
	return 0;
}

double transform_mouse_x(unsigned int Mouse_x){
	return (2.0 * (double)Mouse_x / (double)main_window_width) - 1.0;
}

double transform_mouse_y(unsigned int Mouse_y){
	return (2.0 * (double)(main_window_height - Mouse_y) / (double)main_window_height) - 1.0;
}

void dispatch_engine_message(const MSG* Win_message){
		ENGINE::EVENT_MSG engine_event;
		ZeroMemory(&engine_event,sizeof(ENGINE::EVENT_MSG));
		ENGINE::EVENT_KEY_RECORD& 		key   = engine_event.event_record.key;
		ENGINE::EVENT_MOUSE_RECORD& 	mouse = engine_event.event_record.mouse;
		
		switch(Win_message->message){
			case WM_KEYDOWN:
			case WM_KEYUP:
				engine_event.event = EV_KEY_PRESS;
				if(Win_message->message == WM_KEYDOWN)	
					key.key_down = true;
				
				key.key_code = Win_message->wParam; 
				ENGINE::eng_event(engine_event);
			break;
			case WM_MOUSEWHEEL:
				engine_event.event = EV_MOUSE_WHEEL;
				mouse.delta = GET_WHEEL_DELTA_WPARAM(Win_message->wParam);
				mouse.x     = transform_mouse_x(LOWORD(Win_message->lParam));
				mouse.y		= transform_mouse_y(HIWORD(Win_message->lParam));
				ENGINE::eng_event(engine_event);
			break;
			case WM_MOUSEMOVE:
				engine_event.event = EV_MOUSE_MOVE;
				mouse.x     = transform_mouse_x(LOWORD(Win_message->lParam));
				mouse.y		= transform_mouse_y(HIWORD(Win_message->lParam));
				ENGINE::eng_event(engine_event);
			break;
			case WM_MBUTTONDOWN:
				engine_event.event = EV_MOUSE_MKEY_DOWN;
				mouse.x     = transform_mouse_x(LOWORD(Win_message->lParam));
				mouse.y		= transform_mouse_y(HIWORD(Win_message->lParam));
				ENGINE::eng_event(engine_event);
			break;
			case WM_MBUTTONUP:
				engine_event.event = EV_MOUSE_MKEY_UP;
				mouse.x     = transform_mouse_x(LOWORD(Win_message->lParam));
				mouse.y		= transform_mouse_y(HIWORD(Win_message->lParam));
				ENGINE::eng_event(engine_event);
			break;
			case WM_LBUTTONDOWN:
				engine_event.event = EV_MOUSE_LKEY_DOWN;
				mouse.x     = transform_mouse_x(LOWORD(Win_message->lParam));
				mouse.y		= transform_mouse_y(HIWORD(Win_message->lParam));
				ENGINE::eng_event(engine_event);
			break;
			case WM_LBUTTONUP:
				engine_event.event = EV_MOUSE_LKEY_UP;
				mouse.x     = transform_mouse_x(LOWORD(Win_message->lParam));
				mouse.y		= transform_mouse_y(HIWORD(Win_message->lParam));
				ENGINE::eng_event(engine_event);
			break;
			case WM_RBUTTONDOWN:
				engine_event.event = EV_MOUSE_RKEY_DOWN;
				mouse.x     = transform_mouse_x(LOWORD(Win_message->lParam));
				mouse.y		= transform_mouse_y(HIWORD(Win_message->lParam));
				ENGINE::eng_event(engine_event);
			break;
			case WM_RBUTTONUP:
				engine_event.event = EV_MOUSE_RKEY_UP;
				mouse.x     = transform_mouse_x(LOWORD(Win_message->lParam));
				mouse.y		= transform_mouse_y(HIWORD(Win_message->lParam));
				ENGINE::eng_event(engine_event);
			break;
		}
		
		
}

void print_gl_info(){
	
	printf("==================GL_INFO=========================\n");
	
	struct __GL_STRING_INFO{
		const char* title;
		GLenum		info_id;
	}info_list[] = {
		{"OPENGL_VERSION"				, GL_VERSION					},
		{"GL_VENDOR"					, GL_VENDOR						},
		{"GL_RENDERER"					, GL_RENDERER					},
		{"GL_SHADING_LANGUAGE_VERSION"	, GL_SHADING_LANGUAGE_VERSION	}
	};
	
	for(UINT i = 0 ; i < sizeof(info_list)/sizeof(__GL_STRING_INFO); i++){
		printf("%s:\t %s\n",info_list[i].title ,glGetString(info_list[i].info_id));
	}
	
	GLint profile_mask;
	glGetIntegerv(GL_CONTEXT_PROFILE_MASK, &profile_mask);
	
	printf("PROFILE_MASK:  [%X]  ", profile_mask);
	
	if(profile_mask & GL_CONTEXT_CORE_PROFILE_BIT){
		printf("in CORE_PROFILE\n");
	}else{
		printf("in COMPATIBILITY_PROFILE\n");
	}
	
	
	GLint shading_versions;
	shading_versions = 0;
	glGetIntegerv(GL_NUM_SHADING_LANGUAGE_VERSIONS, &shading_versions);
	
	printf("shading_versions %d \n", shading_versions);
	
	for(UINT i = 0; i < shading_versions; i++){
		printf("SHADING VER %d: %s\n",i, glGetStringi(GL_SHADING_LANGUAGE_VERSION,i));
	}
	
	printf("==================================================\n");
	
}

//* platform functions *// 
void ENGINE::terminate(){
	PostQuitMessage(0);
}

bool ENGINE::get_absolute_path	(std::string& Absolure_path,const std::string& Path){
	char buffer[MAX_PATH];
	if(GetFullPathName(Path.c_str(), MAX_PATH, buffer, nullptr) != 0){
		Absolure_path = (const char*)buffer;
		return true;
	}
	
	return false;
}
