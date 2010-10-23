#include <jni.h>
#include <string>


class DSKLogger {
public:
	static void initialise(JavaVM* vm);
	static void log(const std::string message);

private:
	static JavaVM* vm;
	static jclass dskNativeLoggerClass;
	static jmethodID logMethod;
};