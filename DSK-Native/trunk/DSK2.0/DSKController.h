#include <DShow.h>
#include <d3d9.h>
#include <Vmr9.h>
#include <exception>
#include <jni.h>
#include <string>
#include "DSKInitParameters.h"
#include "DSVideoRenderer.h"

class DSKController {
public:
	DSKController(DSKInitParameters* initParameters, HWND hwnd, JavaVM* vm, jobject controller);
	~DSKController();
	void play();
	void pause();
	void stop();
	void setSize(int width, int height);
	void paint(HDC hdc);
	LONGLONG getTime();
	void setTime(LONGLONG time);
	void getVideoSize(long* width, long* height);
	LONGLONG getDuration();
	double getVolume();
	void setVolume(double volume);

private:
	IMediaControl* mediaControl;
	IMediaSeeking* mediaSeeking;
	IBasicAudio* basicAudio;
	DSVideoRenderer* videoRenderer;
	struct EventThreadArguments {
        JavaVM* vm;
		HANDLE* handle;
		jobject controller;
		IMediaEventEx* mediaEvent;
	};
	void addSourceFilter(DSKInitParameters* initParameters, IGraphBuilder* graphBuilder, IBaseFilter** source);
	void addFiltersOGG(DSKInitParameters* initParameters, IGraphBuilder* graphBuilder, IBaseFilter* source);
	void addFiltersINTELIGENT(DSKInitParameters* initParameters, IGraphBuilder* graphBuilder, IBaseFilter* source);
	void startEventThread(IMediaEventEx* mediaEvent, JavaVM* vm, jobject controller);
	void stopEventThread();
	static void eventThreadEntry(void* eventThreadArgument);
};
