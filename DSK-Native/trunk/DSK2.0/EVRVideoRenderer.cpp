#include "EVRVideoRenderer.h"
#include "DSKUtilities.h"

EVRVideoRenderer::EVRVideoRenderer(HWND window) : DSVideoRenderer(window) {
	IBaseFilter* evr = NULL;
	HRESULT result = CoCreateInstance(
		CLSID_EnhancedVideoRenderer,
		NULL,
		CLSCTX_INPROC_SERVER,
		IID_IBaseFilter,
		(void**)&evr);
	DSKUtilities::checkResult(result, "CoCreateInstance CLSID_EnhancedVideoRenderer");

	this->setFilter(evr);

	IMFGetService *getService = NULL;
	result = evr->QueryInterface(IID_IMFGetService, (void**)&getService);
	DSKUtilities::checkResult(result, "QueryInterface IID_IMFGetService");

	result = getService->GetService(MR_VIDEO_RENDER_SERVICE, IID_IMFVideoDisplayControl, (void**)&this->displayControl);
	DSKUtilities::checkResult(result, "GetService MR_VIDEO_RENDER_SERVICE IID_IMFVideoDisplayControl");

	result = this->displayControl->SetVideoWindow(this->getWindow());
	DSKUtilities::checkResult(result, "displayControl->SetVideoWindow(this->getWindow())");

	result = this->displayControl->SetAspectRatioMode(MFVideoARMode_PreservePicture);
	DSKUtilities::checkResult(result, "displayControl->SetAspectRatioMode(MFVideoARMode_PreservePicture)");

	result = this->displayControl->SetBorderColor(RGB(0, 0, 0));
	DSKUtilities::checkResult(result, "displayControl->SetBorderColor(RGB(0, 0, 0))");

	//this->setSize(10,10);
}

EVRVideoRenderer::~EVRVideoRenderer() {
	this->displayControl->Release();
}

void EVRVideoRenderer::paint(HDC deviceContext) {
	HRESULT result = this->displayControl->RepaintVideo();
	DSKUtilities::checkResult(result, "displayControl->RepaintVideo()");
}

void EVRVideoRenderer::setSize(int width, int height) {
	RECT destRect;
	destRect.top = 0;
	destRect.bottom = height;
	destRect.left = 0;
	destRect.right = width;

	HRESULT result = this->displayControl->SetVideoPosition(NULL, &destRect);

	std::string message = DSKUtilities::format(
		"displayControl->SetVideoPosition [%d, %d, %d, %d]",
		destRect.top, destRect.right, destRect.bottom, destRect.left);
	DSKUtilities::checkResult(result, message);
}

void EVRVideoRenderer::getNativeSize(LONG* width, LONG* height) {
	SIZE* size = new SIZE();
	HRESULT result = this->displayControl->GetNativeVideoSize(size, NULL);
	DSKUtilities::checkResult(result, "displayControl->GetNativeVideoSize(size, NULL)");
	*width = size->cx;
	*height = size->cy;
}