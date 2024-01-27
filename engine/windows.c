#include <windows.h>
#include <stdbool.h>
#include <platform.h>
#include <debug.h>

static HINSTANCE instance;
static const LPCSTR wndClassName = "__VoidEngWCLS__";
static const DWORD wndStyle = WS_CAPTION | WS_SYSMENU | WS_OVERLAPPEDWINDOW;
static const DWORD wndClassStyle = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
static UINT_PTR timer = 0;
static TimerProcedure timerProc = NULL;

LRESULT CALLBACK winProcedure(HWND wnd, UINT msg, 
	WPARAM wParam, LPARAM lParam);

#define WND_STATUS_NULL 0
#define WND_STATUS_ZOMBIE 1
#define WND_STATUS_ALIVE 2

typedef struct {
	unsigned int status;
	HWND hwnd;
	RenderProcedure 	renderProcedure;
	ReshapeProcedure 	reshapeProcedure;
	KeyProcedure 		keyProcedure;
	MouseProcedure 		mouseProcedure;
}WindowRecord;

static WindowRecord windows[PLFM_MAX_WINDOWS];
static unsigned int windowsCount;

void initWinsowsList();
void initWindowRecord(WindowRecord * record);
unsigned int getFreeWindowId();
unsigned int findWindow(HWND hwnd);
WindowRecord * getWidnowRecord(unsigned int wndId);

void initWinsowsList(){
	unsigned int i;
	for(i = 0; i < PLFM_MAX_WINDOWS; i++)
		initWindowRecord(windows + i);
};

void initWindowRecord(WindowRecord * record){
	record->status = WND_STATUS_NULL;
	record->hwnd = NULL;
	record->renderProcedure = NULL;
	record->reshapeProcedure = NULL;
	record->keyProcedure = NULL;
	record->mouseProcedure = NULL;
};

unsigned int getFreeWindowId(){
	unsigned int wndId;
	WindowRecord * wndRecord;

	for(wndId = 0; wndId < PLFM_MAX_WINDOWS; wndId++)
	{
		wndRecord = (windows + wndId);
		if(wndRecord->hwnd == NULL)
			return wndId;
	};

	return PLFM_INVALID_ID;
};

unsigned int findWindow(HWND hwnd){
	unsigned int wndId;

	if(hwnd == NULL)
		return PLFM_INVALID_ID;

	for(wndId = 0; wndId < PLFM_MAX_WINDOWS; wndId++)
		if(windows[wndId].hwnd == hwnd)
			return wndId;

	return PLFM_INVALID_ID;
};

WindowRecord * getWidnowRecord(unsigned int wndId){
	if(wndId >= PLFM_MAX_WINDOWS || wndId == PLFM_INVALID_ID)
		return NULL;

	return (windows + wndId);
};

void freeWindows(){
	unsigned int wndId;
	HWND hwnd;

	for(wndId = 0; wndId < PLFM_MAX_WINDOWS; wndId++){
		hwnd = windows[wndId].hwnd;
		if(hwnd != NULL)
			DestroyWindow(hwnd);

		initWindowRecord(windows + wndId);
	};
};

void freeTimers(){
	if(timer != 0)
		KillTimer(NULL, timer);
};


unsigned int plfmInit(){
	WNDCLASSEX winClass;

	instance  = GetModuleHandle(NULL);
	initWinsowsList();

	ZeroMemory(&winClass, sizeof(winClass));
	winClass.cbSize	= sizeof(winClass);
	winClass.lpszClassName = wndClassName;
	winClass.hInstance = instance;
	winClass.style = wndClassStyle;
	winClass.lpfnWndProc = winProcedure;
	winClass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	winClass.hCursor = LoadCursor(NULL, IDC_ARROW);

	if(!RegisterClassEx(&winClass)){
		DEBUG("Window class ""%s"" registration error!\n", 
			winClass.lpszClassName);
		return PLFM_ERROR;
	};

	timer = 0;
	timerProc = NULL;

	return PLFM_SUCCESS;
};

unsigned int plfmFree()
{
	freeWindows();
	freeTimers();

	return PLFM_SUCCESS;	
};
	
unsigned int plfmLoop()
{
	MSG message;

	while(GetMessage(&message, NULL, 0, 0)){
		TranslateMessage(&message);
		DispatchMessage(&message);
	};

	return PLFM_SUCCESS;
};

unsigned int plfmCreateWindow(unsigned int  width, unsigned int height){
	RECT areaRect;
	HWND hwnd;
	WindowRecord * wndRecord;
	unsigned int wndId;

	wndId = getFreeWindowId();
	if(wndId == PLFM_INVALID_ID)
		return PLFM_INVALID_ID;

	wndRecord = getWidnowRecord(wndId);
	initWindowRecord(wndRecord);

	ZeroMemory(&areaRect,sizeof(areaRect));
	areaRect.right = width;
	areaRect.bottom	= height;
	AdjustWindowRect(&areaRect, wndStyle, false);
	width = areaRect.right - areaRect.left;
	height = areaRect.bottom - areaRect.top;
	
	hwnd = CreateWindowEx(0, wndClassName, NULL, wndStyle, 
		CW_USEDEFAULT, CW_USEDEFAULT, width, height, NULL, NULL, 
		instance, NULL);

	if(hwnd == NULL)
		return PLFM_INVALID_ID;

	wndRecord->hwnd = hwnd;
	windowsCount++;

	return wndId;
};

unsigned int plfmDestroyWindow(unsigned int wndId){
	WindowRecord * wndRecord;

	wndRecord = getWidnowRecord(wndId);
	if(wndRecord == NULL){
		return PLFM_ERROR;
	};

	if(wndRecord->hwnd == NULL)
		return PLFM_ERROR;

	DestroyWindow(wndRecord->hwnd);
	wndRecord->hwnd = NULL;	
	windowsCount--;

	if(windowsCount == 0)
		PostQuitMessage(0);

	return PLFM_SUCCESS;
};

unsigned int plfmShowWindow(unsigned int wndId){
	WindowRecord * wndRecord;

	wndRecord = getWidnowRecord(wndId);
	if(wndRecord == NULL){
		return PLFM_ERROR;
	};

	if(wndRecord->hwnd == NULL)
		return PLFM_ERROR;

	ShowWindow(wndRecord->hwnd, SW_SHOW);

	return PLFM_SUCCESS;
};

unsigned int plfmHideWindow(unsigned int wndId){
	WindowRecord * wndRecord;

	wndRecord = getWidnowRecord(wndId);
	if(wndRecord == NULL){
		return PLFM_ERROR;
	};

	if(wndRecord->hwnd == NULL)
		return PLFM_ERROR;

	ShowWindow(wndRecord->hwnd, SW_HIDE);

	return PLFM_SUCCESS;
};

const char * plfmExpandPath(const char * const path)
{
	static char buffer[MAX_PATH];

	if(GetFullPathName(path, MAX_PATH, buffer, NULL) != 0)
		return (const char*)buffer;
	
	return NULL;
};

unsigned int plfmSetRenderProcedure(unsigned int wndId, RenderProcedure proc){
	WindowRecord * record;

	record = getWidnowRecord(wndId);
	if(record == NULL || record->hwnd == NULL)
		return PLFM_ERROR;

	record->renderProcedure = proc;
	return PLFM_SUCCESS;
};

unsigned int plfmSetReshapeProcedure(unsigned int wndId, ReshapeProcedure proc)
{
	WindowRecord * record;

	record = getWidnowRecord(wndId);
	if(record == NULL || record->hwnd == NULL)
		return PLFM_ERROR;

	record->reshapeProcedure = proc;
	return PLFM_SUCCESS;
};

unsigned int plfmSetKeyProcedure(unsigned int wndId, KeyProcedure proc){
	WindowRecord * record;

	record = getWidnowRecord(wndId);
	if(record == NULL || record->hwnd == NULL)
		return PLFM_ERROR;

	record->keyProcedure = proc;
	return PLFM_SUCCESS;
};

unsigned int plfmSetMouseProcedure(unsigned int wndId, MouseProcedure proc){
	WindowRecord * record;

	record = getWidnowRecord(wndId);
	if(record == NULL || record->hwnd == NULL)
		return PLFM_ERROR;

	record->mouseProcedure = proc;
	return PLFM_SUCCESS;
};

void mainTimerProcedure(HWND hwnd, UINT par1, UINT_PTR par2, DWORD par3){
	if(timerProc != NULL)
		timerProc(0);
};

unsigned int plfmSetTimer(unsigned int period, TimerProcedure proc){
	timerProc = proc;

	if(timer == 0){
		timer = SetTimer(NULL, 0, period, mainTimerProcedure);
	}else{
		KillTimer(NULL, timer);
		timer = SetTimer(NULL, 0, period, mainTimerProcedure);
	};

};

LRESULT CALLBACK winProcedure(HWND hwnd, UINT msg, 
	WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT paintStruct;
	unsigned int wndId;
	WindowRecord * wndRecord;
	
	wndId = findWindow(hwnd);
	if(wndId == PLFM_INVALID_ID)
		return DefWindowProc(hwnd, msg, wParam, lParam);

	wndRecord = getWidnowRecord(wndId);
	switch(msg){
		case WM_CLOSE:
			plfmDestroyWindow(wndId);
		break;
		case WM_PAINT:
			BeginPaint(hwnd, &paintStruct);
			if(wndRecord->renderProcedure != NULL)
				wndRecord->renderProcedure(wndId);

			EndPaint(hwnd, &paintStruct);

		break;
		case WM_MOUSEMOVE:
			if(wndRecord->mouseProcedure != NULL)
				wndRecord->mouseProcedure(wndId, 
					LOWORD(lParam), HIWORD(lParam), wParam);

		break;
		case WM_CHAR:
			if(wndRecord->keyProcedure != NULL)
				wndRecord->keyProcedure(wndId, wParam);

		break;
		case WM_SIZE:
			if(wndRecord->reshapeProcedure != NULL)
				wndRecord->reshapeProcedure(wndId, 
					LOWORD(lParam), HIWORD(lParam));

		break;
		default:
			return DefWindowProc(hwnd, msg, wParam, lParam);
	};

	return 0;
};

