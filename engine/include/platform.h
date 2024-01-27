#ifndef __PLATFORM_H__
#define __PLATFORM_H__

#define PLFM_SUCCESS 0
#define PLFM_ERROR -1
#define PLFM_ERROR_INVALIDARG 1

#define PLFM_INVALID_ID -1

#define PLFM_MAX_WINDOWS 10

#ifdef __cplusplus
extern "C"{
#endif

typedef void (*RenderProcedure)(unsigned int wndId);
typedef void (*ReshapeProcedure)(unsigned int wndId, unsigned int width,
	unsigned int height);
typedef void (*KeyProcedure)(unsigned int wndId, const char key);
typedef void (*MouseProcedure)(unsigned int wndId, 
	const unsigned int x, unsigned int y, unsigned int keys);
typedef void (*TimerProcedure)(unsigned int id);

unsigned int plfmInit();
unsigned int plfmFree();
unsigned int plfmLoop();
unsigned int plfmCreateWindow(unsigned int width, unsigned int height);
unsigned int plfmDestroyWindow(unsigned int wndId);
unsigned int plfmShowWindow(unsigned int wndId);
unsigned int plfmHideWindow(unsigned int wndId);
unsigned int plfmSetRenderProcedure(unsigned int wndId, RenderProcedure proc);
unsigned int plfmSetReshapeProcedure(unsigned int wndId, ReshapeProcedure proc);
unsigned int plfmSetKeyProcedure(unsigned int wndId, KeyProcedure proc);
unsigned int plfmSetMouseProcedure(unsigned int wndId, MouseProcedure proc);
unsigned int plfmSetTimer(unsigned int delay, TimerProcedure proc);
const char * plfmExpandPath(const char * const path);

#ifdef __cplusplus
}
#endif

#endif
