#include "JNI.h"
#include "DSKLogger.h"
#include <exception>

JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *vm, void *reserved) {
	try {
		DSKLogger::initialise(vm);
	} catch(std::exception e) {
		JNIEnv* env;
		vm->AttachCurrentThread((void**)&env, NULL);
		env->ThrowNew(env->FindClass("com/kps/dsk/DSKException"), e.what());
		vm->DetachCurrentThread();
	}
	return JNI_VERSION_1_4;
}

JNIEXPORT void JNICALL JNI_OnUnload(JavaVM *vm, void *reserved) { }