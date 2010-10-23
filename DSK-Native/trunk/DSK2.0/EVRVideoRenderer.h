#include "DSVideoRenderer.h"
#include <d3d9.h>
#include <evr.h>

class EVRVideoRenderer : public DSVideoRenderer {
public:
	EVRVideoRenderer(HWND window);
	~EVRVideoRenderer();
	void paint(HDC deviceContext);
	void setSize(int width, int height);
	void getNativeSize(LONG* width, LONG* height);
private:
	IMFVideoDisplayControl* displayControl;
};
