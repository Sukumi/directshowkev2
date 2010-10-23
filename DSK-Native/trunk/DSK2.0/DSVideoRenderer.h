#ifndef DSVIDEORENDERER_H
#define DSVIDEORENDERER_H

#include <Windows.h>
#include <DShow.h> 

class DSVideoRenderer {
public:
	DSVideoRenderer(HWND windowIn);
	~DSVideoRenderer();
	IBaseFilter* getFilter();
	virtual void paint(HDC deviceContext) = 0;
	virtual void setSize(int width, int height) = 0;
	virtual void getNativeSize(LONG* width, LONG* height) = 0;
protected:
	HWND getWindow();
	void setFilter(IBaseFilter* filterIn);
private:
	IBaseFilter* filter;
	HWND window;
};

#endif
