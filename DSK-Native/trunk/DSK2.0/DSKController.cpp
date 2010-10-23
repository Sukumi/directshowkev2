#include "DSKController.h"
#include <process.h>
#include "DSKLogger.h"
#include "DSKUtilities.h"
#include <cstdarg>
#include "VMR9VideoRenderer.h"
#include "EVRVideoRenderer.h"
#include "DirectShowUtilities.h"

DSKController::DSKController(DSKInitParameters* initParameters, HWND hwnd, JavaVM* vm, jobject controller) {
	HRESULT result;

	result = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
	DSKUtilities::checkResult(result, "CoInitializeEx");

	IGraphBuilder* graphBuilder = NULL;

	result = CoCreateInstance(
		CLSID_FilterGraph,
		NULL, 
		CLSCTX_INPROC_SERVER,
		IID_IGraphBuilder,
		(void**)&graphBuilder);
	DSKUtilities::checkResult(result, "CoCreateInstance CLSID_FilterGraph");

	IMediaEventEx* mediaEvent = NULL;
	result = graphBuilder->QueryInterface(IID_IMediaEventEx, (void**)&mediaEvent);
	DSKUtilities::checkResult(result, "QueryInterface IID_IMediaEventEx");

	startEventThread(mediaEvent, vm, controller);

	result = graphBuilder->QueryInterface(IID_IMediaControl, (void**)&this->mediaControl);
	DSKUtilities::checkResult(result, "QueryInterface IID_IMediaControl");

	result = graphBuilder->QueryInterface(IID_IBasicAudio, (void**)&this->basicAudio);
	DSKUtilities::checkResult(result, "QueryInterface IID_IBasicAudio");

	result = graphBuilder->QueryInterface(IID_IMediaSeeking, (void**)&this->mediaSeeking);
	DSKUtilities::checkResult(result, "QueryInterface IID_IMediaSeeking");

	IBaseFilter* source = NULL;
	this->addSourceFilter(initParameters, graphBuilder, &source);

	if(initParameters->getVideoRendererType() == EVR) {
		this->videoRenderer = new EVRVideoRenderer(hwnd);
	} else {
		this->videoRenderer = new VMR9VideoRenderer(hwnd);
	}

	if(initParameters->getGraphType() == OGG) {
		this->addFiltersOGG(initParameters, graphBuilder, source);
	} else {
		this->addFiltersINTELIGENT(initParameters, graphBuilder, source);
	}

	this->pause();
}

void DSKController::addFiltersOGG(DSKInitParameters* initParameters, IGraphBuilder* graphBuilder, IBaseFilter* source) {
	IBaseFilter* oggDemuxer = NULL;
	IBaseFilter* theoraDecoder = NULL;
	IBaseFilter* vorbisDecoder = NULL;
	IBaseFilter* soundRenderer = NULL;

	LPWSTR oggDemuxerCLSIDString  = L"{C9361F5A-3282-4944-9899-6D99CDC5370B}";
	CLSID oggDemuxerCLSID;
	CLSIDFromString(oggDemuxerCLSIDString, &oggDemuxerCLSID);
	HRESULT result = CoCreateInstance(
		oggDemuxerCLSID,
		NULL,
		CLSCTX_INPROC_SERVER,
		IID_IBaseFilter,
		(void**)&oggDemuxer);
	DSKUtilities::checkResult(result, "CoCreateInstance oggDemuxerCLSID");

	LPWSTR theoraDecoderCLSIDString  = L"{05187161-5C36-4324-A734-22BF37509F2D}";
	CLSID theoraDecoderCLSID;
	CLSIDFromString(theoraDecoderCLSIDString, &theoraDecoderCLSID);
	result = CoCreateInstance(
		theoraDecoderCLSID,
		NULL,
		CLSCTX_INPROC_SERVER,
		IID_IBaseFilter,
		(void**)&theoraDecoder);
	DSKUtilities::checkResult(result, "CoCreateInstance theoraDecoderCLSID");

	LPWSTR vorbisDecoderCLSIDString  = L"{05A1D945-A794-44EF-B41A-2F851A117155}";
	CLSID vorbisDecoderCLSID;
	CLSIDFromString(vorbisDecoderCLSIDString, &vorbisDecoderCLSID);
	result = CoCreateInstance(
		vorbisDecoderCLSID,
		NULL,
		CLSCTX_INPROC_SERVER,
		IID_IBaseFilter,
		(void**)&vorbisDecoder);
	DSKUtilities::checkResult(result, "CoCreateInstance vorbisDecoderCLSID");

	result = CoCreateInstance(
		CLSID_DSoundRender, 
		NULL, 
		CLSCTX_INPROC_SERVER, 
		IID_IBaseFilter, 
		(void**)&soundRenderer);
	DSKUtilities::checkResult(result, "CoCreateInstance CLSID_DSoundRender");

	result = graphBuilder->AddFilter(oggDemuxer, L"OggDemuxer");
	DSKUtilities::checkResult(result, "AddFilter OggDemuxer");

	result = graphBuilder->AddFilter(theoraDecoder, L"TheoraDecoder");
	DSKUtilities::checkResult(result, "AddFilter TheoraDecoder");

	result = graphBuilder->AddFilter(vorbisDecoder, L"VorbisDecoder");
	DSKUtilities::checkResult(result, "AddFilter VorbisDecoder");

	result = graphBuilder->AddFilter(soundRenderer, L"SoundRenderer");
	DSKUtilities::checkResult(result, "AddFilter SoundRenderer");

	result = graphBuilder->AddFilter(this->videoRenderer->getFilter(), L"A VideoRenderer");
	DSKUtilities::checkResult(result, "AddFilter videoRenderer->getFilter()");

	result = DirectShowUtilities::connectFilters(graphBuilder, source, oggDemuxer);
	DSKUtilities::checkResult(result, "connectFilters source oggDemuxer");

	result = DirectShowUtilities::connectFilters(graphBuilder, oggDemuxer, theoraDecoder);
	DSKUtilities::checkResult(result, "connectFilters oggDemuxer theoraDecoder");

	result = DirectShowUtilities::connectFilters(graphBuilder, oggDemuxer, vorbisDecoder);
	DSKUtilities::checkResult(result, "connectFilters oggDemuxer vorbisDecoder");

	result = DirectShowUtilities::connectFilters(graphBuilder, theoraDecoder, this->videoRenderer->getFilter());
	DSKUtilities::checkResult(result, "connectFilters theoraDecoder this->videoRenderer->getFilter()");

	result = DirectShowUtilities::connectFilters(graphBuilder, vorbisDecoder, soundRenderer);
	DSKUtilities::checkResult(result, "connectFilters vorbisDecoder soundRenderer");
}

void DSKController::addFiltersINTELIGENT(DSKInitParameters* initParameters, IGraphBuilder* graphBuilder, IBaseFilter* source) {
	IBaseFilter* soundRenderer = NULL;
	HRESULT result = CoCreateInstance(
		CLSID_DSoundRender, 
		NULL, 
		CLSCTX_INPROC_SERVER, 
		IID_IBaseFilter, 
		(void**)&soundRenderer);
	DSKUtilities::checkResult(result, "CoCreateInstance CLSID_DSoundRender");

	result = graphBuilder->AddFilter(soundRenderer, L"SoundRenderer");
	DSKUtilities::checkResult(result, "AddFilter SoundRenderer");

	result = graphBuilder->AddFilter(this->videoRenderer->getFilter(), L"A VideoRenderer");
	DSKUtilities::checkResult(result, "AddFilter videoRenderer->getFilter()");

	IPin* sourceOutputPin = NULL;
	result = DirectShowUtilities::getOutputPin(source, &sourceOutputPin);
	DSKUtilities::checkResult(result, "getOutputPin source");

	result = graphBuilder->Render(sourceOutputPin);
	DSKUtilities::checkResult(result, "graphBuilder->Render(sourceOutputPin)");
}

void DSKController::addSourceFilter(DSKInitParameters* initParameters, IGraphBuilder* graphBuilder, IBaseFilter** source) {
	std::string filePath = initParameters->getFilePath();
	const char* cFilePath = filePath.c_str();

	int length = MultiByteToWideChar(CP_ACP, 0, cFilePath, -1, NULL, 0);
	LPWSTR wideString = new WCHAR[length];
	MultiByteToWideChar(CP_ACP, 0, cFilePath, -1, wideString, length);
	HRESULT result = graphBuilder->AddSourceFilter(wideString, NULL, source);
	delete[] wideString;
	std::string message = DSKUtilities::format("AddSourceFilter \"%s\"", cFilePath);
	DSKUtilities::checkResult(result, message.c_str());
}

DSKController::~DSKController() {
}

void DSKController::play() {
	HRESULT result = this->mediaControl->Run();
	DSKUtilities::checkResult(result, "mediaControl->Run()");
}

void DSKController::pause() {
	HRESULT result = this->mediaControl->Pause();
	DSKUtilities::checkResult(result, "mediaControl->Pause()");
}

void DSKController::stop() {
	HRESULT result = this->mediaControl->Stop();
	DSKUtilities::checkResult(result, "mediaControl->Stop()");
}

void DSKController::paint(HDC hdc) {
	this->videoRenderer->paint(hdc);
}

void DSKController::getVideoSize(long* width, long* height) {
	this->videoRenderer->getNativeSize(width, height);
}

LONGLONG DSKController::getTime() {
	LONGLONG time;
	HRESULT result = this->mediaSeeking->GetCurrentPosition(&time);
	DSKUtilities::checkResult(result, "mediaSeeking->GetCurrentPosition");
	return time;
}

void DSKController::setTime(LONGLONG time) {
	HRESULT result = this->mediaSeeking->SetPositions(
		&time, AM_SEEKING_AbsolutePositioning,
		NULL, AM_SEEKING_NoPositioning);
	DSKUtilities::checkResult(result, "mediaSeeking->SetPositions");
}

LONGLONG DSKController::getDuration() {
	LONGLONG duration;
	HRESULT result = this->mediaSeeking->GetDuration(&duration);
	DSKUtilities::checkResult(result, "mediaSeeking->GetDuration");
	return duration;
}

void DSKController::setSize(int width, int height) {
	this->videoRenderer->setSize(width, height);
}

double DSKController::getVolume() {
	long* volume = NULL;
	HRESULT result = this->basicAudio->get_Volume(volume);
	DSKUtilities::checkResult(result, "basicAudio->get_Volume");
	// Convert the volume value from long[-10000, 0] to double[0.0, 1.0]
	return ((*volume + 10000) / 10000.0); 
}

void DSKController::setVolume(double volume) {
	// Convert the volume value from double[0.0, 1.0] to long[-10000, 0]
	long directShowVolume = (long)((volume * 10000) - 10000);
	HRESULT result = this->basicAudio->put_Volume(directShowVolume);
	std::string message = DSKUtilities::format("basicAudio->put_Volume(%d)", directShowVolume);
	DSKUtilities::checkResult(result, message);
}

void DSKController::startEventThread(IMediaEventEx* mediaEvent, JavaVM *vm, jobject controller) {
	HANDLE handle = CreateEvent(NULL, TRUE, FALSE, NULL);
	EventThreadArguments eventThreadArguments;
	eventThreadArguments.vm = vm;
	eventThreadArguments.handle = &handle;
	eventThreadArguments.controller = controller;
	eventThreadArguments.mediaEvent = mediaEvent;
	_beginthread(&eventThreadEntry, 0, (void*) &eventThreadArguments);
	WaitForSingleObject(handle, INFINITE);
}

void DSKController::eventThreadEntry(void *argument) {
	EventThreadArguments *arguments = (EventThreadArguments*)argument;

	// Unpack the EventThreadArguments to local variables.
	JavaVM *vm = arguments->vm;
	jobject controller = arguments->controller;
	IMediaEventEx *mediaEvent = arguments->mediaEvent;

	// Notify the other thread that we have unpacked the EventThreadArguments.
	// Soon after this call, the struct pointed to by 'arguments' will fall out of scope.
	SetEvent(*arguments->handle); 

	JNIEnv *env;
	JavaVMAttachArgs attachArgs;
	attachArgs.version = JNI_VERSION_1_4;
	attachArgs.name = "DSKController IMediaEventEx Thread";
	attachArgs.group = NULL;

	jint result = vm->AttachCurrentThread((void**)&env, &attachArgs);

	jclass controllerClass = env->GetObjectClass(controller);
	jmethodID mediaEventMethod = env->GetMethodID(controllerClass, "mediaEvent", "(IJJ)V");

	HANDLE eventHandle;
	mediaEvent->GetEventHandle((OAEVENT*)&eventHandle);

	DSKLogger::log("DSKController Event Thread has started");

	while(true) {
		// TODO Consider waiting on multiple objects (eventHandle and exitHandle)
		// so that we can request the thread be stopped
		// (This may be a requirement to correctly release all native resources)
		WaitForSingleObject(eventHandle, INFINITE);
		long eventCode;
		LONG_PTR parameter1;
		LONG_PTR parameter2;
		mediaEvent->GetEvent(&eventCode, &parameter1, &parameter2, 0);
		env->CallVoidMethod(controller, mediaEventMethod, eventCode, parameter1, parameter2);
	}

	vm->DetachCurrentThread();
}