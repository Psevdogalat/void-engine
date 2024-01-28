#ifndef __PLATFORM_H__
#define __PLATFORM_H__

#define PLFM_SUCCESS 0
#define PLFM_ERROR -1
#define PLFM_ERROR_INVALIDARG 1

#define PLFMRESULT unsigned int 
#define PLFMHANDLE unsigned int 
#define PLFMHWND PLFMHANDLE
#define PLFMHGC	PLFMHANDLE
#define PLFMHTIMER PLFMHANDLE

#define PLFM_INVALID_HANDLE (PLFMHANDLE) -1
#define PLFM_NULL_HANDLE (PLFMHANDLE) 0

#define PLFM_MAX_WINDOWS 10

#ifdef __cplusplus
extern "C"{
#endif

typedef void (*PlfmRenderProc)(PLFMHWND);
typedef void (*PlfmReshapeProc)(PLFMHWND, unsigned int, unsigned int);
typedef void (*PlfmKeyProc)(PLFMHWND, const char);
typedef void (*PlfmMouseProc)(PLFMHWND, unsigned int, unsigned int, 
	unsigned int keys);
typedef void (*PlfmCloseProc)(PLFMHWND );
typedef void (*PlfmTimerProc)(PLFMHTIMER);

PLFMRESULT plfmInit();
PLFMRESULT plfmFree();
PLFMRESULT plfmLoop();

PLFMHWND plfmCreateWindow(unsigned int x, unsigned int y, 
	unsigned int width, unsigned int height);
PLFMRESULT plfmDestroyWindow(PLFMHWND hWnd);
PLFMRESULT plfmShowWindow(PLFMHWND hWnd);
PLFMRESULT plfmHideWindow(PLFMHWND hWnd);
PLFMRESULT plfmRedrawWindow(PLFMHWND hWnd);
PLFMRESULT plfmPostQuitMessage(unsigned int exitCode);

PLFMRESULT plfmSetRenderProc(PLFMHWND hWnd, PlfmRenderProc proc);
PLFMRESULT plfmSetReshapeProc(PLFMHWND hWnd, PlfmReshapeProc proc);
PLFMRESULT plfmSetKeyProc(PLFMHWND hWnd, PlfmKeyProc proc);
PLFMRESULT plfmSetMouseProc(PLFMHWND hWnd, PlfmMouseProc proc);
PLFMRESULT plfmSetCloseProc(PLFMHWND hWnd, PlfmCloseProc proc);

PLFMRESULT 	plfmInitGL();
PLFMHGC 	plfmGLCreateGC(PLFMHWND hWnd);
PLFMRESULT	plfmGLDestroyGC(PLFMHGC hGC);
PLFMRESULT 	plfmGLMakeCurrent(PLFMHGC hGC);

PLFMRESULT 	plfmSetTimer(unsigned int delay, PlfmTimerProc proc);
const char * plfmExpandPath(const char * const path);

#ifdef __cplusplus
}
#endif

#endif
