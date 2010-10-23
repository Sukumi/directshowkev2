#include "DSKLogger.h"
#include <iostream>
#include <sstream>

JavaVM* DSKLogger::vm;
jclass DSKLogger::dskNativeLoggerClass;
jmethodID DSKLogger::logMethod;

void DSKLogger::initialise(JavaVM* vmIn) {
	DSKLogger::vm = vmIn;
	JNIEnv* env;

	jint attached = DSKLogger::vm->GetEnv((void**)&env, JNI_VERSION_1_4);
	bool isAlreadyAttached = (attached == JNI_OK);

	jint result = DSKLogger::vm->AttachCurrentThread((void**)&env, NULL);
	if(result != 0) {
		throw std::exception("AttachCurrentThread failed");
	}

	DSKLogger::dskNativeLoggerClass = env->FindClass("com/kps/dsk/DSKNativeLogger");
	if (DSKLogger::dskNativeLoggerClass == NULL) {
		return;
	}

	DSKLogger::dskNativeLoggerClass = (jclass) env->NewGlobalRef(DSKLogger::dskNativeLoggerClass); // Make a global reference so that subsequent calls don't fail
	if (DSKLogger::dskNativeLoggerClass == NULL) {
		throw std::exception("NewGlobalRef(DSKLogger::dskNativeLoggerClass) failed");
	}

	DSKLogger::logMethod = env->GetStaticMethodID(DSKLogger::dskNativeLoggerClass, "log", "(Ljava/lang/String;)V");
	if (DSKLogger::logMethod == NULL) {
		return;
	}


	//TODO consider ensuring that this is attempted even if one of the above fails
	if(!isAlreadyAttached) {
		result = DSKLogger::vm->DetachCurrentThread();
	}

	DSKLogger::log("DSKLogger::initialise: Native DSKLogger initialised");
}

void DSKLogger::log(const std::string message) {
	JNIEnv* env;

	jint attached = DSKLogger::vm->GetEnv((void**)&env, JNI_VERSION_1_4);
	const bool isAlreadyAttached = (attached == JNI_OK);

	DSKLogger::vm->AttachCurrentThread((void**)&env, NULL);
	jstring string = env->NewStringUTF(message.c_str());
	env->CallStaticVoidMethod(DSKLogger::dskNativeLoggerClass, DSKLogger::logMethod, string);

	if(!isAlreadyAttached) {
		DSKLogger::vm->DetachCurrentThread();
	}
}