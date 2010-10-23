#include <jni.h>
#include <string>
#include <Windows.h>

class DSKUtilities {
public:
	static std::string format(std::string format, ...);
	static void checkResult(HRESULT result, std::string message);
	static void throwJavaDSKException(JNIEnv* env, std::string message);
};