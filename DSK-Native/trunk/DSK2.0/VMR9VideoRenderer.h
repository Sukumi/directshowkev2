#include "DSVideoRenderer.h"
#include <d3d9.h>
#include <Vmr9.h>

class VMR9VideoRenderer : public DSVideoRenderer {
public:
	VMR9VideoRenderer(HWND window);
	~VMR9VideoRenderer();
	void paint(HDC deviceContext);
	void setSize(int width, int height);
	void getNativeSize(LONG* width, LONG* height);
private:
	IVMRWindowlessControl9* windowlessControl;
};
