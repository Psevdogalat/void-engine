
#include <windows.h>
#include <gl/glew.h>
#include <stdbool.h>
#include <platform.h>
#include <debug.h>

static HINSTANCE instance;
static const LPCSTR wndClassName = "__VoidEngWCLS__";
static const DWORD wndStyle = WS_CAPTION | WS_SYSMENU | WS_OVERLAPPEDWINDOW;
static const DWORD wndClassStyle = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
static UINT_PTR timer = 0;
static PlfmTimerProc timerProc = NULL;

LRESULT CALLBACK winProcedure(HWND wnd, UINT msg, 
	WPARAM wParam, LPARAM lParam);

#define WND_STATUS_NULL 0
#define WND_STATUS_ZOMBIE 1
#define WND_STATUS_ALIVE 2

struct WindowRecord {
	unsigned int status;
	HWND shWnd;
	PlfmRenderProc	renderProc;
	PlfmReshapeProc reshapeProc;
	PlfmKeyProc 	keyProc;
	PlfmMouseProc 	mouseProc;
	PlfmCloseProc	closeProc;
};

static struct WindowRecord windowsRecords[PLFM_MAX_WINDOWS];
static unsigned int windowsCount;

void initWinsowsList();
void initWindowRecord(struct WindowRecord * record);
PLFMHWND getFreeHWND();
PLFMHWND findWindow(HWND hwnd);
struct WindowRecord * getWidnowRecord(PLFMHWND hWnd);

void initWinsowsList(){
	unsigned int i;
	for(i = 0; i < PLFM_MAX_WINDOWS; i++)
		initWindowRecord(&windowsRecords[i]);
};

void initWindowRecord(struct WindowRecord * record){
	record->status = WND_STATUS_NULL;
	record->shWnd = NULL;
	record->renderProc = NULL;
	record->reshapeProc = NULL;
	record->keyProc = NULL;
	record->mouseProc = NULL;
};

PLFMHWND getFreeHWND(){
	unsigned int i;
	struct WindowRecord * wndRecord;

	for(i = 0; i < PLFM_MAX_WINDOWS; i++){
		wndRecord = &windowsRecords[i];
		if(wndRecord->shWnd == NULL)
			return i + 1;
	};

	return PLFM_INVALID_HANDLE;
};

PLFMHWND findWindow(HWND hWnd){
	unsigned int i;

	if(hWnd == NULL || hWnd == INVALID_HANDLE_VALUE)
		return PLFM_INVALID_HANDLE;

	for(i = 0; i < PLFM_MAX_WINDOWS; i++)
		if(windowsRecords[i].shWnd == hWnd)
			return i + 1;

	return PLFM_INVALID_HANDLE;
};

struct WindowRecord * getWidnowRecord(PLFMHWND hWnd){
	if(hWnd - 1 >= PLFM_MAX_WINDOWS || hWnd == PLFM_INVALID_HANDLE)
		return NULL;

	return &windowsRecords[hWnd-1];
};

HWND getSystemHWND(PLFMHWND hWnd){
	struct WindowRecord * record;
	record = getWidnowRecord(hWnd);

	if(record == NULL)
		return INVALID_HANDLE_VALUE;
	
	return record->shWnd;
};

void freeWindows(){
	unsigned int i;
	HWND shWnd;

	for(i = 0; i < PLFM_MAX_WINDOWS; i++){
		shWnd = windowsRecords[i].shWnd;
		if(shWnd != NULL)
			DestroyWindow(shWnd);

	};
};

void freeTimers(){
	if(timer != 0)
		KillTimer(NULL, timer);
};

PLFMRESULT plfmInit(){
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

PLFMRESULT plfmFree()
{
	freeWindows();
	freeTimers();

	return PLFM_SUCCESS;	
};
	
PLFMRESULT plfmLoop()
{
	MSG message;

	while(GetMessage(&message, NULL, 0, 0)){
		TranslateMessage(&message);
		DispatchMessage(&message);
	};

	return PLFM_SUCCESS;
};

PLFMHWND plfmCreateWindow(unsigned int x, unsigned int y,
	unsigned int  width, unsigned int height){

	RECT areaRect;
	HWND shWnd;
	struct WindowRecord * pWndRecord;
	PLFMHWND newHWnd;

	newHWnd = getFreeHWND();
	if(newHWnd == PLFM_INVALID_HANDLE)
		return PLFM_INVALID_HANDLE;

	pWndRecord = getWidnowRecord(newHWnd);
	initWindowRecord(pWndRecord);

	ZeroMemory(&areaRect,sizeof(areaRect));
	areaRect.right = width;
	areaRect.bottom	= height;
	AdjustWindowRect(&areaRect, wndStyle, false);
	width = areaRect.right - areaRect.left;
	height = areaRect.bottom - areaRect.top;
	
	shWnd = CreateWindowEx(0, wndClassName, NULL, wndStyle, x, y, width, height,
		NULL, NULL, instance, NULL);

	if(shWnd == NULL)
		return PLFM_INVALID_HANDLE;

	pWndRecord->shWnd = shWnd;
	windowsCount++;

	return newHWnd;
};

PLFMRESULT plfmDestroyWindow(PLFMHWND hWnd){
	struct WindowRecord * pWndRecord;

	pWndRecord = getWidnowRecord(hWnd);
	if(pWndRecord == NULL || pWndRecord->shWnd == NULL)
		return PLFM_ERROR;

	DestroyWindow(pWndRecord->shWnd);
	pWndRecord->shWnd = NULL;	
	windowsCount--;

	if(windowsCount == 0)
		//PostQuitMessage(0);

	return PLFM_SUCCESS;
};

PLFMRESULT plfmShowWindow(PLFMHWND hWnd){
	HWND shWnd; 	

	shWnd = getSystemHWND(hWnd);

	if(shWnd == NULL)
		return PLFM_ERROR;

	ShowWindow(shWnd, SW_SHOW);

	return PLFM_SUCCESS;
};

PLFMRESULT plfmHideWindow(PLFMHWND hWnd){
	HWND shWnd;

	shWnd = getSystemHWND(hWnd);
	if(shWnd == NULL)
		return PLFM_ERROR;

	ShowWindow(shWnd, SW_HIDE);

	return PLFM_SUCCESS;
};

PLFMRESULT plfmRedrawWindow(PLFMHWND hWnd){
	HWND shWnd;

	shWnd = getSystemHWND(hWnd);
	if(shWnd == NULL)
		return PLFM_ERROR;

	RedrawWindow(shWnd, NULL, NULL, RDW_INVALIDATE);

	return PLFM_SUCCESS;
};

const char * plfmExpandPath(const char * const path)
{
	static char buffer[MAX_PATH];

	if(GetFullPathName(path, MAX_PATH, buffer, NULL) != 0)
		return (const char*)buffer;
	
	return NULL;
};

PLFMRESULT plfmSetRenderProc(PLFMHWND hWnd, PlfmRenderProc proc){
	struct WindowRecord * pWndRecord;

	pWndRecord = getWidnowRecord(hWnd);
	if(pWndRecord == NULL || pWndRecord->shWnd == NULL)
		return PLFM_ERROR;

	pWndRecord->renderProc = proc;
	return PLFM_SUCCESS;
};

PLFMRESULT plfmSetReshapeProc(PLFMHWND hWnd, PlfmReshapeProc proc)
{
	struct WindowRecord * pWndRecord;

	pWndRecord = getWidnowRecord(hWnd);
	if(pWndRecord == NULL || pWndRecord->shWnd == NULL)
		return PLFM_ERROR;

	pWndRecord->reshapeProc = proc;
	return PLFM_SUCCESS;
};

PLFMRESULT plfmSetKeyProc(PLFMHWND hWnd, PlfmKeyProc proc){
	struct WindowRecord * pWndRecord;

	pWndRecord = getWidnowRecord(hWnd);
	if(pWndRecord == NULL || pWndRecord->shWnd == NULL)
		return PLFM_ERROR;

	pWndRecord->keyProc = proc;
	return PLFM_SUCCESS;
};

PLFMRESULT plfmSetMouseProc(PLFMHWND hWnd, PlfmMouseProc proc){
	struct WindowRecord * pWndRecord;

	pWndRecord = getWidnowRecord(hWnd);
	if(pWndRecord == NULL || pWndRecord->shWnd == NULL)
		return PLFM_ERROR;

	pWndRecord->mouseProc = proc;
	return PLFM_SUCCESS;
};

PLFMRESULT plfmSetCloseProc(PLFMHWND hWnd, PlfmCloseProc proc){
	struct WindowRecord * pWndRecord;

	pWndRecord = getWidnowRecord(hWnd);
	if(pWndRecord == NULL || pWndRecord->shWnd == NULL)
		return PLFM_ERROR;

	pWndRecord->closeProc = proc;

	return PLFM_SUCCESS;
};

void mainTimerProc(HWND hwnd, UINT par1, UINT_PTR par2, DWORD par3){
	if(timerProc != NULL)
		timerProc(0);
};

PLFMRESULT plfmSetTimer(unsigned int period, PlfmTimerProc proc){
	timerProc = proc;

	if(timer == 0){
		timer = SetTimer(NULL, 0, period, mainTimerProc);
	}else{
		KillTimer(NULL, timer);
		timer = SetTimer(NULL, 0, period, mainTimerProc);
	};

	return PLFM_SUCCESS;
};

LRESULT CALLBACK winProcedure(HWND shWnd, UINT msg, 
	WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT paintStruct;
	PLFMHWND hWnd;
	struct WindowRecord * pWndRecord;
	
	hWnd = findWindow(shWnd);
	if(hWnd == PLFM_INVALID_HANDLE)
		return DefWindowProc(shWnd, msg, wParam, lParam);

	pWndRecord = getWidnowRecord(hWnd);
	switch(msg){
		case WM_CLOSE:
			if(pWndRecord->closeProc != NULL){
				pWndRecord->closeProc(hWnd);

			}else
				return DefWindowProc(shWnd, msg, wParam, lParam);

		break;
		case WM_PAINT:
			if(pWndRecord->renderProc != NULL){
				BeginPaint(shWnd, &paintStruct);
				pWndRecord->renderProc(hWnd);
				EndPaint(shWnd, &paintStruct);

			}else
				return DefWindowProc(shWnd, msg, wParam, lParam);

		break;
		case WM_MOUSEMOVE:
			if(pWndRecord->mouseProc != NULL){
				pWndRecord->mouseProc(hWnd, LOWORD(lParam), HIWORD(lParam), 
					wParam);

			}else
				return DefWindowProc(shWnd, msg, wParam, lParam);

		break;
		case WM_CHAR:
			if(pWndRecord->keyProc != NULL){
				pWndRecord->keyProc(hWnd, wParam);

			}else
				return DefWindowProc(shWnd, msg, wParam, lParam);

		break;
		case WM_SIZE:
			if(pWndRecord->reshapeProc != NULL){
				pWndRecord->reshapeProc(hWnd, LOWORD(lParam), HIWORD(lParam));

			}else
				return DefWindowProc(shWnd, msg, wParam, lParam);

		break;
		default:
			return DefWindowProc(shWnd, msg, wParam, lParam);
	};

	return 0;
};

PLFMRESULT plfmPostQuitMessage(unsigned int exitCode){
	PostQuitMessage(exitCode);
	return PLFM_SUCCESS;
};

