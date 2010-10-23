#include "DSVideoRenderer.h"

DSVideoRenderer::DSVideoRenderer(HWND windowIn):filter(NULL), window(windowIn) { 
}


DSVideoRenderer::~DSVideoRenderer() {
	if(this->filter != NULL) {
		this->filter->Release();
	}
}

IBaseFilter* DSVideoRenderer::getFilter() {
	return filter; 
}

HWND DSVideoRenderer::getWindow() {
	return window; 
}

void DSVideoRenderer::setFilter(IBaseFilter* filterIn) { 
	this->filter = filterIn; 
}