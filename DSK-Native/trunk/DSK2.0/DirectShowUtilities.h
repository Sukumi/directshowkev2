#include <DShow.h>

class DirectShowUtilities {
public:
	static HRESULT getOutputPin(IBaseFilter* filter, IPin** outputPin);
	static HRESULT connectFilters(IGraphBuilder* builder, IBaseFilter* out, IBaseFilter* in);
};