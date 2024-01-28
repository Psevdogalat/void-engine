#include <platform.h>
#include <debug.h>
#include <windows.h>
#include <gl/glew.h>
#include <gl/wglew.h>

static PIXELFORMATDESCRIPTOR glPFD;
static unsigned int glPFI;
static GLint glMajorVersion;
static GLint glMinorVersion;
static GLint glProfileMask;

HWND getSystemHWND(PLFMHWND hWnd);

struct GCRecord{
	HWND shWnd;
	HGLRC shGLRC;
};

#define MAX_GC_NUMBER 10
static struct GCRecord gcRecords[MAX_GC_NUMBER];

PLFMHGC getFreeHGC(){
	unsigned int i;
	for(i = 0; i < MAX_GC_NUMBER; i++)
		if( gcRecords[i].shGLRC == NULL )
			return i+1;

	return PLFM_INVALID_HANDLE;
};

struct GCRecord * getGCRecord(PLFMHGC hGC){
	if(hGC - 1 >= MAX_GC_NUMBER || hGC == PLFM_INVALID_HANDLE)
		return NULL;

	return &gcRecords[hGC-1];
};


PLFMRESULT plfmInitGL(){
	PLFMHWND dummyWnd;
	HWND 	shDummyWnd;
	HDC 	shDummyDC;
	HGLRC 	shDummyGLRC;

	//init GC storage
	unsigned int i;
	for(i = 0; i < MAX_GC_NUMBER; i++)
		gcRecords[i] = (struct GCRecord){NULL, NULL};

	//===============

	dummyWnd = plfmCreateWindow(0, 0, 100, 100);

	if(dummyWnd == PLFM_INVALID_HANDLE)
		return PLFM_ERROR;

	shDummyWnd = getSystemHWND(dummyWnd);
			
	ZeroMemory(&glPFD, sizeof(PIXELFORMATDESCRIPTOR));
	glPFD.nSize 	= sizeof(PIXELFORMATDESCRIPTOR);
	glPFD.nVersion	= 1;
	glPFD.dwFlags	= PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | 
		PFD_DOUBLEBUFFER;
	glPFD.iLayerType	= PFD_MAIN_PLANE;
	glPFD.iPixelType	= PFD_TYPE_RGBA;
	glPFD.cColorBits	= 24;
	glPFD.cDepthBits 	= 16;
	
	shDummyDC = GetDC(shDummyWnd);
	glPFI = ChoosePixelFormat(shDummyDC, &glPFD);
	if(glPFI == 0){
		ReleaseDC(shDummyWnd, shDummyDC);
		plfmDestroyWindow(dummyWnd);
		return PLFM_ERROR;
	};

	SetPixelFormat(shDummyDC, glPFI, &glPFD);
	shDummyGLRC = wglCreateContext(shDummyDC);
	wglMakeCurrent(shDummyDC, shDummyGLRC);

	glewInit();

	glGetIntegerv(GL_MAJOR_VERSION, &glMajorVersion);
	glGetIntegerv(GL_MINOR_VERSION, &glMinorVersion);
	DEBUG("Avalable GL version %d.%d\n", glMajorVersion, glMinorVersion);
	
	wglMakeCurrent(NULL, NULL);
	wglDeleteContext(shDummyGLRC);

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
	
	unsigned int formatsNumber;
	wglChoosePixelFormatARB(shDummyDC, piAttribIList, pfAttribFList, 
		1, &glPFI, &formatsNumber);

	ReleaseDC(shDummyWnd, shDummyDC);
	plfmDestroyWindow(dummyWnd);

	return PLFM_SUCCESS;
};

PLFMHGC plfmGLCreateGC(PLFMHWND hWnd){
	HWND 	shWnd;
	HDC 	shDC;
	HGLRC 	shGLRC;
	PLFMHGC newHGC;
	struct GCRecord * pGCRecord;

	newHGC = getFreeHGC();
	if(newHGC == PLFM_INVALID_HANDLE)
		return PLFM_INVALID_HANDLE;

	pGCRecord = getGCRecord(newHGC);

	shWnd = getSystemHWND(hWnd);
	if(shWnd == NULL)
		return PLFM_INVALID_HANDLE;

	shDC = GetDC(shWnd);
	SetPixelFormat(shDC, glPFI, &glPFD);

	int glCA[] = {
		WGL_CONTEXT_MAJOR_VERSION_ARB, 	glMajorVersion,
		WGL_CONTEXT_MINOR_VERSION_ARB, 	glMinorVersion,
		0, 0
	};

	shGLRC = wglCreateContextAttribsARB(shDC, NULL, glCA);
	ReleaseDC(shWnd, shDC);

	pGCRecord->shWnd = shWnd;
	pGCRecord->shGLRC = shGLRC;

	return newHGC;
};

PLFMRESULT plfmGLDestroyGC(PLFMHGC hGC){
	struct GCRecord * pGCRecord;

	pGCRecord = getGCRecord(hGC);
	if(pGCRecord == NULL || pGCRecord->shGLRC == NULL)
		return PLFM_ERROR;

	
	wglDeleteContext(pGCRecord->shGLRC);
	pGCRecord->shGLRC = NULL;

	return PLFM_SUCCESS;
};


PLFMRESULT plfmGLMakeCurrent(PLFMHGC hGC){
	struct GCRecord * pGCRecord;
	HDC shDC;

	if(hGC != PLFM_NULL_HANDLE){
		pGCRecord = getGCRecord(hGC);
		if(pGCRecord == NULL || pGCRecord->shGLRC == NULL)
			return PLFM_ERROR;
	
			
		shDC = GetDC(pGCRecord->shWnd);
		wglMakeCurrent(shDC, pGCRecord->shGLRC);

	}else
		wglMakeCurrent(NULL, NULL);

	return PLFM_SUCCESS;
};
