#include "DirectShowUtilities.h"

HRESULT DirectShowUtilities::getOutputPin(IBaseFilter* filter, IPin** outputPin) {
	IEnumPins* pins = NULL;
	HRESULT result = filter->EnumPins(&pins);
	if(FAILED(result)) {
		return result;
	}

	IPin* pin = NULL;
	while(pins->Next(1, &pin, NULL) == S_OK) {
		PIN_DIRECTION direction;
		result = pin->QueryDirection(&direction);
		if(direction == PINDIR_OUTPUT) {
			*outputPin = pin;
			pins->Release();
			return S_OK;
		} else {
			pin->Release();
		}
	}

	return E_FAIL;
}



HRESULT DirectShowUtilities::connectFilters(IGraphBuilder* builder, IBaseFilter* out, IBaseFilter* in) {
	HRESULT result = 0;

	IEnumPins* outPins = NULL;
	IEnumPins* inPins = NULL;

	result = out->EnumPins(&outPins);

	if(FAILED(result)) {
		return result;
	}

	result = in->EnumPins(&inPins);

	if(FAILED(result)) {
		outPins->Release(); // TODO ensure all releases are done when returning
		return result;
	}

	IPin* outPin = NULL;
	while(outPins->Next(1, &outPin, NULL) == S_OK) {
		PIN_DIRECTION outDirection;
		result = outPin->QueryDirection(&outDirection);
		if(FAILED(result)) {
			outPin->Release();
			outPins->Release();
			inPins->Release();
			return result;
		}

		if(outDirection == PINDIR_OUTPUT) {
			IPin* inPin = NULL;;
			while(inPins->Next(1, &inPin, NULL) == S_OK) {		
				PIN_DIRECTION inDirection;
				result = inPin->QueryDirection(&inDirection);
				if(FAILED(result)) {
					outPin->Release();
					inPin->Release();
					outPins->Release();
					inPins->Release();
					return result;
				}

				if(inDirection == PINDIR_INPUT) {
					result = builder->Connect(outPin, inPin);
					if (result == S_OK) {
						outPin->Release();
						inPin->Release();
						outPins->Release();
						inPins->Release();
						return result;
					}
				}
				inPin->Release();
			}
			inPins->Reset(); // Wasted a bit of time on this bug :)
		}
		outPin->Release();
	}

	outPins->Release();
	inPins->Release();
	return E_FAIL;
}