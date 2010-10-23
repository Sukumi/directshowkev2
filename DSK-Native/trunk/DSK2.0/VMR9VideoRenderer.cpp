#include "VMR9VideoRenderer.h"
#include "DSKUtilities.h"

VMR9VideoRenderer::VMR9VideoRenderer(HWND window) : DSVideoRenderer(window) {
	IBaseFilter* vmr = NULL;
	HRESULT result = CoCreateInstance(
		CLSID_VideoMixingRenderer9,
		NULL,
		CLSCTX_INPROC_SERVER,
		IID_IBaseFilter,
		(void**)&vmr);
	DSKUtilities::checkResult(result, "CoCreateInstance CLSID_VideoMixingRenderer9");

	this->setFilter(vmr);

	IVMRFilterConfig9 *config = NULL;
	result = vmr->QueryInterface(IID_IVMRFilterConfig9, (void**)&config);
	DSKUtilities::checkResult(result, "QueryInterface IID_IVMRFilterConfig9");

	result = config->SetRenderingMode(VMR9Mode_Windowless);
	DSKUtilities::checkResult(result, "config->SetRenderingMode(VMR9Mode_Windowless)");

	config->Release();

	result = vmr->QueryInterface(IID_IVMRWindowlessControl9, (void**)&this->windowlessControl);
	DSKUtilities::checkResult(result, "QueryInterface IID_IVMRWindowlessControl9");

	result = this->windowlessControl->SetVideoClippingWindow(this->getWindow());
	DSKUtilities::checkResult(result, "windowlessControl->SetVideoClippingWindow(this->getWindow())");

	result = this->windowlessControl->SetAspectRatioMode(VMR_ARMODE_LETTER_BOX);
	DSKUtilities::checkResult(result, "windowlessControl->SetAspectRatioMode(VMR_ARMODE_LETTER_BOX)");

	result = this->windowlessControl->SetBorderColor(RGB(0, 0, 0));
	DSKUtilities::checkResult(result, "windowlessControl->SetBorderColor(RGB(0, 0, 0))");

	//this->setSize(10, 10);
}

VMR9VideoRenderer::~VMR9VideoRenderer() {
	this->windowlessControl->Release();
}

void VMR9VideoRenderer::paint(HDC deviceContext) {
	HRESULT result = this->windowlessControl->RepaintVideo(this->getWindow(), deviceContext);
	DSKUtilities::checkResult(result, "windowlessControl->RepaintVideo(this->getWindow(), deviceContext)");
}

void VMR9VideoRenderer::setSize(int width, int height) {
	RECT dstRect;
	dstRect.top = 0;
	dstRect.bottom = height;
	dstRect.left = 0;
	dstRect.right = width;
	HRESULT result = this->windowlessControl->SetVideoPosition(NULL, &dstRect);
	std::string message = DSKUtilities::format(
		"vmr9WindowlessControl->SetVideoPosition [%d, %d, %d, %d]",
		dstRect.top, dstRect.right, dstRect.bottom, dstRect.left);
	DSKUtilities::checkResult(result, message);
}

void VMR9VideoRenderer::getNativeSize(LONG* width, LONG* height) {
	LONG aspectWidth;
	LONG aspectHeight;
	HRESULT result = this->windowlessControl->GetNativeVideoSize(width, height, &aspectWidth, &aspectHeight);
	DSKUtilities::checkResult(result, "windowlessControl->GetNativeVideoSize");
}