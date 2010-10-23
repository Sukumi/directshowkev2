#include <DShow.h>

class DSUtilities {
public:	
	static HRESULT getOutputPin(IBaseFilter* filter, IPin** outputPin);
	static HRESULT connectFilters(IGraphBuilder* builder, IBaseFilter* out, IBaseFilter* in);
};