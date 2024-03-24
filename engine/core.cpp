#include <engine.hpp>
#include <platform.h>
#include <exception>
#include <cstdlib>
#include <stdio.h>

#include <debug.h>

using namespace VoidEngine;

static const unsigned int mwndHeight = 400;
static const unsigned int mwndWidth = 600;

PLFMHWND hMainWnd;
PLFMHGC hGC;

static void keyProcedure(PLFMHWND hWnd, const char key){
	DEBUG("wnd %d key %c pressed\n", hWnd, key);
};

void drawProcedure(PLFMHWND hWnd){
	//render(hGC);
};

static void mouseProcedure(PLFMHWND hWnd, unsigned int x, unsigned int y,
	unsigned int keys){
	DEBUG("wnd %d mouse procedure %d %d keys %d\n", hWnd, x, y, keys);
};

static void reshapeProcedure(PLFMHWND hWnd, unsigned int width, 
	unsigned int height){
	plfmRedrawWindow(hMainWnd);
};

static void closeProc(PLFMHWND hWnd ){
	plfmPostQuitMessage(0);
};

static void timerProcedure(PLFMHTIMER hTimer){
	DEBUG("timer %d occure\n", hTimer);
	plfmRedrawWindow(hMainWnd);
};

void VoidEngine::init(){
	if(plfmInit() != PLFM_SUCCESS){
		printf("platform init error");
		exit(1);
	}

	if(plfmInitGL() != PLFM_SUCCESS){
		DEBUG("platform graphic library init error");
		exit(1);
	}

	hMainWnd = plfmCreateWindow(0, 0, mwndWidth, mwndHeight);
	if(hMainWnd == PLFM_INVALID_HANDLE){
		DEBUG("Can't create main window");
		plfmFree();
		exit(1);
	};

	plfmSetKeyProc(hMainWnd, keyProcedure);
	plfmSetRenderProc(hMainWnd, drawProcedure);
	plfmSetMouseProc(hMainWnd, mouseProcedure);
	plfmSetReshapeProc(hMainWnd, reshapeProcedure);
	plfmSetCloseProc(hMainWnd, closeProc);
	plfmSetTimer(1000, timerProcedure);

	//hGC = plfmGLCreateGC(hMainWnd);
	//plfmGLMakeCurrent(hGC);
	//plfmGLMakeCurrent(PLFM_NULL_HANDLE);

	plfmShowWindow(hMainWnd);
};

void VoidEngine::free(){
	plfmFree();
};

void VoidEngine::run(){
	plfmLoop();
};

