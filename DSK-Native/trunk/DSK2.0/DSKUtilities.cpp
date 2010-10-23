#include "DSKUtilities.h"
#include "DSKLogger.h"

std::string DSKUtilities::format(std::string format, ...) {
	va_list arguments;
	va_start(arguments, format);
	int bufferSize = 200;
	char* buffer = new char[bufferSize];
	vsprintf_s(buffer, bufferSize, format.c_str(), arguments);
	va_end(arguments);
	std::string formatted = std::string(buffer);
	delete[] buffer;
	return formatted;
}

void DSKUtilities::checkResult(HRESULT result, std::string message) {
	if(FAILED(result)) {
		// TODO Try to get this working so we can get nice descriptions:
		// const char* description = DXGetErrorDescription(result);
		// const char* errorString = DXGetErrorString(result);
		std::string failedMessage = DSKUtilities::format("FAILED(%d) %s ", result, message.c_str());
		throw std::exception(failedMessage.c_str());
	} else {
		std::string successMessage = DSKUtilities::format("SUCCESS(%d) %s ", result, message.c_str());
		DSKLogger::log(successMessage.c_str());
	}
}

void DSKUtilities::throwJavaDSKException(JNIEnv* env, std::string message) {
	//TODO possibly able to cache the jclass (It should be valid across all threads)
	env->ThrowNew(env->FindClass("com/kps/dsk/DSKException"), message.c_str());
}


