#include <platform.h>
#include <engine_platform.h>

#include <std_defines.h>

#include <windows.h>

#include <gl/glew.h>
#include <gl/wglew.h>
#include <cstdio>

static struct{
	UINT width;
	UINT height;
}resolutions[]={
	{640, 420},
	{960, 630}
};

static const UINT 		cur_resolution 		= 1;

static UINT 			main_window_width  	= 640;
static UINT 			main_window_height 	= 420;
static const UINT 		aver_frames 		= 10;	
static const double 	frame_lock 			= 30.0;
static const LPCSTR 	main_window_class_name 	= "main_window_class";
static const LPCSTR 	main_window_header	  	= "voidEngine2D";

static HWND 			main_window;
static HGLRC 			render_context;
static UINT_PTR			frame_timer;
static DWORD			frame_sys_tick_last;
static DWORD			frame_sys_tick;
static double			frame_time;
static double 			aver_frame_time = 0;
static UINT				aver_cnt 		= 0;
static double			frame_counter;

LRESULT CALLBACK main_window_procedure(HWND, UINT, WPARAM, LPARAM);
void dispatch_engine_message(const MSG* );
void print_gl_info();

int APIENTRY WinMain(HINSTANCE Instance, HINSTANCE Prev_instance, LPSTR Cmd_line, int Cmd_show){
	
	HDC	 					main_window_dc;	
	WNDCLASSEX				wcex;
	int 					pixel_format_index;
	PIXELFORMATDESCRIPTOR 	pfd;
	RECT					area_size_rect;
	DWORD					window_style;
	HGLRC 					dummy_gl_context;
	GLint 					gl_version[2];
	UINT 					nNumFormats;
	
	main_window_width 	= resolutions[cur_resolution].width;
	main_window_height	= resolutions[cur_resolution].height;
	
	ZeroMemory(&wcex,sizeof(WNDCLASSEX));
	wcex.cbSize 		= sizeof(WNDCLASSEX);
	wcex.lpszClassName	= main_window_class_name;
	wcex.hInstance		= Instance;
	wcex.style			= CS_OWNDC;
	wcex.lpfnWndProc	= main_window_procedure;
	wcex.hbrBackground	= (HBRUSH)GetStockObject(BLACK_BRUSH);
	wcex.hCursor		= LoadCursor(nullptr, IDC_ARROW);
	
	if(!RegisterClassEx(&wcex)){
		printf("Window class ""%s"" registration error!\n",main_window_class_name);
		return 1;
	}
	
	window_style = WS_CAPTION | WS_SYSMENU;
	ZeroMemory(&area_size_rect,sizeof(RECT));
	area_size_rect.right	= main_window_width;
	area_size_rect.bottom	= main_window_height;
	
	AdjustWindowRect(&area_size_rect,window_style,false);
	area_size_rect.bottom 	-= (area_size_rect.top);
	area_size_rect.right	-= area_size_rect.left;
	
	main_window = CreateWindowEx(0, main_window_class_name, main_window_header,
		window_style,
		CW_USEDEFAULT, CW_USEDEFAULT, 
		area_size_rect.right, area_size_rect.bottom, NULL, NULL, Instance, NULL);
		
	if(main_window == INVALID_HANDLE_VALUE){
		printf("Instance of window class ""%s"" creation error!\n",main_window_class_name);
		return 1;
	}
	
	main_window_dc = GetDC(main_window);

	ZeroMemory(&pfd,sizeof(PIXELFORMATDESCRIPTOR));
	pfd.nSize 		= sizeof(PIXELFORMATDESCRIPTOR);
	pfd.nVersion	= 1;
	pfd.dwFlags		= PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pfd.iLayerType	= PFD_MAIN_PLANE;
	pfd.iPixelType	= PFD_TYPE_RGBA;
	pfd.cColorBits	= 24;
	pfd.cDepthBits 	= 16;
	
	pixel_format_index = ChoosePixelFormat(main_window_dc, &pfd);
	if(pixel_format_index == 0){
		printf("invalid pixel format!\n");
		return 1;
	}
	
	SetPixelFormat(main_window_dc, pixel_format_index, &pfd);
	
	//* dummy context for version and compatibility check *//
	dummy_gl_context = wglCreateContext(main_window_dc);
	wglMakeCurrent(main_window_dc, dummy_gl_context);
	glewInit();

	glGetIntegerv(GL_MAJOR_VERSION, &gl_version[0]);
	glGetIntegerv(GL_MINOR_VERSION, &gl_version[1]);
	
	printf("Avalable GL version %d.%d\n", gl_version[0], gl_version[1] );
	
	wglMakeCurrent(NULL, NULL);
	wglDeleteContext(dummy_gl_context);
	ReleaseDC(main_window, main_window_dc);
	DestroyWindow(main_window);
	
	main_window = CreateWindowEx(0, main_window_class_name, main_window_header,
		window_style,
		CW_USEDEFAULT, CW_USEDEFAULT, 
		area_size_rect.right, area_size_rect.bottom, NULL, NULL, Instance, NULL);
		
	if(main_window == INVALID_HANDLE_VALUE){
		printf("Instance of window class ""%s"" creation error!\n",main_window_class_name);
		return 1;
	}
	
	main_window_dc = GetDC(main_window);
	
	//* natural gl context creation  *//
	float pfAttribFList[] = {0, 0};
	int piAttribIList[] = { 
		WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
		WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
		WGL_COLOR_BITS_ARB, 32,
		WGL_RED_BITS_ARB, 8,
		WGL_GREEN_BITS_ARB, 8,
		WGL_BLUE_BITS_ARB, 8,
		WGL_ALPHA_BITS_ARB, 8,
		WGL_DEPTH_BITS_ARB, 16,
		WGL_STENCIL_BITS_ARB, 0,
		WGL_DOUBLE_BUFFER_ARB, GL_TRUE,
		WGL_PIXEL_TYPE_ARB, WGL_TYPE_RGBA_ARB,
		WGL_SAMPLE_BUFFERS_ARB, GL_TRUE,
		WGL_SAMPLES_ARB, 16,
		0, 0
	};
	
	int gl_context_attrs[] =
	{
		WGL_CONTEXT_MAJOR_VERSION_ARB, 	gl_version[0],
		WGL_CONTEXT_MINOR_VERSION_ARB, 	gl_version[1],
		//WGL_CONTEXT_PROFILE_MASK_ARB,	WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
		//WGL_CONTEXT_PROFILE_MASK_ARB,	WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB,
		0, 0
	};
	
	wglChoosePixelFormatARB(main_window_dc, piAttribIList, pfAttribFList, 1, &pixel_format_index, &nNumFormats);
	SetPixelFormat(main_window_dc, pixel_format_index, &pfd);
	
	render_context = wglCreateContextAttribsARB(main_window_dc, nullptr, gl_context_attrs);
	wglMakeCurrent(main_window_dc, render_context);
	/*==================================================*/
	
	print_gl_info();
	glViewport(0, 0, main_window_width, main_window_height);
	ReleaseDC(main_window,main_window_dc);
	
	//printf("Timer val %d\n", (UINT)(1000.0/frame_lock));
	
	frame_timer = SetTimer(main_window,0,((UINT)(1000.0/frame_lock))-10,nullptr);
	frame_sys_tick_last = GetTickCount();
	
	/*engine init there*/
	ENGINE::eng_init((const char*) Cmd_line);	
	
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

LRESULT CALLBACK main_window_procedure(HWND Window, UINT Message, WPARAM W_param, LPARAM L_param){
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
