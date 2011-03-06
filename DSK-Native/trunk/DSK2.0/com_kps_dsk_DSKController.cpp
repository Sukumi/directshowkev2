#include "com_kps_dsk_DSKController.h"
#include <map>
#include "DSKController.h"
#include "DSKLogger.h"
#include <jawt.h>
#include <jawt_md.h>
#include <sstream>
#include "DSKUtilities.h"

/*
* jmethodID for Java DSKController.getNativeId() method.
*/
jmethodID getNativeIdMethod;

/*
* Map from Java DSKController Native IDs to Native DSKControllers.
*/
std::map<jint, DSKController*> controllers;

/*
* Get a Java DSKController's corresponding Native DSKController.
* Returns NULL if a Java exception has been thrown. As such, methods which call this should return to
* Java if NULL is returned.
*/
DSKController* getController(JNIEnv* env, jobject jController) {
	jint nativeId = env->CallIntMethod(jController, getNativeIdMethod);
	std::map<jint, DSKController*>::iterator it = controllers.find(nativeId);
	if(it == controllers.end()) {
		return NULL;
	}
	return it->second;
}

/*
* A method for creating a Native DSKInitParameters from a Java DSKInitParameters.
* Returns NULL if a Java exception has been thrown. As such, methods which call this should return to
* Java if NULL is returned.
*/
DSKInitParameters* createInitParameters(JNIEnv* env, jobject jInitParameters) {
	// TODO All of these methods need error checking.
	// I.E. The JNI will fail without a nice Java Exception
	// if com.kps.dsk.DSVideoRendererType is refactored to com.kps.dsk.DSVideoRenderer

	jclass initParametersClass = env->GetObjectClass(jInitParameters);
	jmethodID getFilePathMethod = env->GetMethodID(initParametersClass, "getFilePath", "()Ljava/lang/String;");
	jmethodID getVideoRendererTypeMethod = env->GetMethodID(initParametersClass, "getVideoRendererType", "()Lcom/kps/dsk/DSVideoRendererType;");
	jmethodID getGraphTypeMethod = env->GetMethodID(initParametersClass, "getGraphType", "()Lcom/kps/dsk/DSGraphType;");

	jclass dsVideoRendererTypeClass = env->FindClass("com/kps/dsk/DSVideoRendererType");
	jfieldID vmr7Field = env->GetStaticFieldID(dsVideoRendererTypeClass, "VMR7", "Lcom/kps/dsk/DSVideoRendererType;");
	jfieldID vmr9Field = env->GetStaticFieldID(dsVideoRendererTypeClass, "VMR9", "Lcom/kps/dsk/DSVideoRendererType;");
	jfieldID evrField = env->GetStaticFieldID(dsVideoRendererTypeClass, "EVR", "Lcom/kps/dsk/DSVideoRendererType;");

	jobject vmr7 = env->GetStaticObjectField(dsVideoRendererTypeClass, vmr7Field); // DSVideoRendererType.VMR7
	jobject vmr9 = env->GetStaticObjectField(dsVideoRendererTypeClass, vmr9Field); // DSVideoRendererType.VMR9
	jobject evr = env->GetStaticObjectField(dsVideoRendererTypeClass, evrField); // DSVideoRendererType.EVR

	jclass dsGraphTypeClass = env->FindClass("com/kps/dsk/DSGraphType");
	jfieldID oggField = env->GetStaticFieldID(dsGraphTypeClass, "OGG", "Lcom/kps/dsk/DSGraphType;");
	jfieldID inteligentConnectField = env->GetStaticFieldID(dsGraphTypeClass, "INTELIGENT_CONNECT", "Lcom/kps/dsk/DSGraphType;");

	jobject ogg = env->GetStaticObjectField(dsGraphTypeClass, oggField); // DSGraphType.OGG
	jobject inteligentConnect = env->GetStaticObjectField(dsGraphTypeClass, inteligentConnectField); // DSGraphType.INTELIGENT_CONNECT

	jstring jFilePath = (jstring)env->CallObjectMethod(jInitParameters, getFilePathMethod); // jFilePath = jInitParameters.getFilePath();
	jobject jVideoRendererType = env->CallObjectMethod(jInitParameters, getVideoRendererTypeMethod); // jVideoRendererType = jInitParameters.getVideoRendererType();
	jobject jGraphType = env->CallObjectMethod(jInitParameters, getGraphTypeMethod); // jGraphType = jInitParameters.getGraphType();

	const char* filePath = env->GetStringUTFChars(jFilePath, NULL);
	if(filePath == NULL) {
		// JNI Documentation states that if GetStringUTFChars returns NULL,
		// a Java Exception has been thrown. Lets return now so that Java can handle it.
		// TODO: Investigate when GetStringUTFChars returns NULL and if a Java Exception is thrown or not.
		return NULL; 
	}

	DSVideoRendererType videoRendererType;
	if(env->IsSameObject(jVideoRendererType, vmr7)) {
		videoRendererType = VMR7;
	} else if(env->IsSameObject(jVideoRendererType, vmr9)) {
		videoRendererType = VMR9;
	} else if(env->IsSameObject(jVideoRendererType, evr)) {
		videoRendererType = EVR;
	} else {
		DSKUtilities::throwJavaDSKException(env, "jVideoRendererType is not a valid value");
		return NULL;
	}

	DSGraphType graphType;
	if(env->IsSameObject(jGraphType, ogg)) {
		graphType = OGG;
	} else 	if(env->IsSameObject(jGraphType, inteligentConnect)) {
		graphType = INTELIGENT_CONNECT;
	} else {
		DSKUtilities::throwJavaDSKException(env, "jGraphType is not a valid value");
		return NULL;
	}

	DSKInitParameters* initParameters = new DSKInitParameters(filePath, videoRendererType, graphType);

	env->ReleaseStringUTFChars(jFilePath, filePath);

	return initParameters;
}



/*
* Class:     com_kps_dsk_DSKController
* Method:    nativeStaticInitialise
* Signature: ()V
*/
JNIEXPORT void JNICALL Java_com_kps_dsk_DSKController_nativeStaticInitialise
	(JNIEnv* env, jclass thisClass) {
		getNativeIdMethod = env->GetMethodID(thisClass, "getNativeId", "()I");
}

/*
* Class:     com_kps_dsk_DSKController
* Method:    nativeInitialise
* Signature: (Lcom/kps/dsk/DSKInitParameters;Lcom/kps/dsk/DSKCanvas;)V
*/
JNIEXPORT void JNICALL Java_com_kps_dsk_DSKController_nativeInitialise
	(JNIEnv* env, jobject jController, jobject jInitParameters, jobject canvas) {
		DSKLogger::log("Entering Java_com_kps_dsk_DSKController_nativeInitialise");

		jint nativeId = env->CallIntMethod(jController, getNativeIdMethod);
		if (env->ExceptionCheck()) {
			// If calling the method throws a Java Exception, return now and let Java handle it.
			return;
		}

		//Check that a DSKController has not already been created for the given nativeId.
		DSKController* controller = getController(env, jController);
		if (controller != NULL) {
			DSKUtilities::throwJavaDSKException(env, "DSKController already created");
			return ;
		}

		JAWT awt;
		JAWT_DrawingSurface* surface;
		JAWT_DrawingSurfaceInfo* surfaceInfo;
		JAWT_Win32DrawingSurfaceInfo* windowsSurfaceInfo;
		jboolean result;
		jint lock; 

		awt.version = JAWT_VERSION_1_4;
		result = JAWT_GetAWT(env, &awt);
		if(result == JNI_FALSE) {
			DSKUtilities::throwJavaDSKException(env, "JAWT_GetAWT failed");
			return;
		}

		surface = awt.GetDrawingSurface(env, canvas);
		if(surface == NULL) {
			DSKUtilities::throwJavaDSKException(env, "GetDrawingSurface failed");
			return;
		}

		lock = surface->Lock(surface);
		if((lock & JAWT_LOCK_ERROR) != 0) {
			awt.FreeDrawingSurface(surface);
			DSKUtilities::throwJavaDSKException(env, "Lock failed");
			return;
		}

		surfaceInfo = surface->GetDrawingSurfaceInfo(surface);
		if(surfaceInfo == NULL) {
			surface->Unlock(surface);
			awt.FreeDrawingSurface(surface);
			DSKUtilities::throwJavaDSKException(env, "GetDrawingSurfaceInfo failed");
			return;
		}

		windowsSurfaceInfo = (JAWT_Win32DrawingSurfaceInfo*)surfaceInfo->platformInfo;

		HWND hwnd = windowsSurfaceInfo->hwnd;

		JavaVM* vm = NULL;
		env->GetJavaVM(&vm);

		jobject jControllerRef = env->NewGlobalRef(jController);

		DSKInitParameters* initParameters = createInitParameters(env, jInitParameters);
		if(initParameters == NULL) {
			surface->FreeDrawingSurfaceInfo(surfaceInfo);
			surface->Unlock(surface);
			awt.FreeDrawingSurface(surface);
			return;
		}

		surface->FreeDrawingSurfaceInfo(surfaceInfo);
		surface->Unlock(surface);
		awt.FreeDrawingSurface(surface);

		try {
			DSKController* controller = new DSKController(initParameters, hwnd, vm, jControllerRef);
			controllers.insert(std::make_pair(nativeId, controller));
		} catch (std::exception e) {
			DSKUtilities::throwJavaDSKException(env, e.what());
			return;
		}

		DSKLogger::log("Completed Java_com_kps_dsk_DSKController_nativeInitialise");
}

/*
* Class:     com_kps_dsk_DSKController
* Method:    nativeGetDuration
* Signature: ()J
*/
JNIEXPORT jlong JNICALL Java_com_kps_dsk_DSKController_nativeGetDuration
	(JNIEnv* env, jobject jController) {
		DSKController* controller = getController(env, jController);
		if (controller == NULL) {
			DSKUtilities::throwJavaDSKException(env, "Could not find a corresponding DSKController");
			return NULL;
		}
		try {
			return controller->getDuration();
		} catch (std::exception e) {
			DSKUtilities::throwJavaDSKException(env, e.what());
			return NULL;
		}
}

/*
* Class:     com_kps_dsk_DSKController
* Method:    nativeGetTime
* Signature: ()J
*/
JNIEXPORT jlong JNICALL Java_com_kps_dsk_DSKController_nativeGetTime
	(JNIEnv* env, jobject jController) {
		DSKController* controller = getController(env, jController);
		if (controller == NULL) {
			DSKUtilities::throwJavaDSKException(env, "Could not find a corresponding DSKController");
			return NULL;
		}
		try {
			return controller->getTime();
		} catch (std::exception e) {
			DSKUtilities::throwJavaDSKException(env, e.what());
			return NULL;
		}
}

/*
* Class:     com_kps_dsk_DSKController
* Method:    nativeSetTime
* Signature: (J)V
*/
JNIEXPORT void JNICALL Java_com_kps_dsk_DSKController_nativeSetTime
	(JNIEnv* env, jobject jController, jlong time) {
		DSKController* controller = getController(env, jController);
		if (controller == NULL) {
			DSKUtilities::throwJavaDSKException(env, "Could not find a corresponding DSKController");
			return;
		}
		try {
			controller->setTime(time);
		} catch (std::exception e) {
			DSKUtilities::throwJavaDSKException(env, e.what());
			return;
		}
}

/*
* Class:     com_kps_dsk_DSKController
* Method:    nativeSetSize
* Signature: (II)J
*/
JNIEXPORT void JNICALL Java_com_kps_dsk_DSKController_nativeSetSize
	(JNIEnv* env, jobject jController, jint width, jint height) {
		DSKController* controller = getController(env, jController);
		if (controller == NULL) {
			DSKUtilities::throwJavaDSKException(env, "Could not find a corresponding DSKController");
			return;
		}
		try {
			controller->setSize(width, height);
		} catch (std::exception e) {
			DSKUtilities::throwJavaDSKException(env, e.what());
			return;
		}
}

/*
* Class:     com_kps_dsk_DSKController
* Method:    nativePlay
* Signature: ()V
*/
JNIEXPORT void JNICALL Java_com_kps_dsk_DSKController_nativePlay
	(JNIEnv* env, jobject jController) {
		DSKController* controller = getController(env, jController);
		if (controller == NULL) {
			DSKUtilities::throwJavaDSKException(env, "Could not find a corresponding DSKController");
			return;
		}
		try {
			controller->play();
		} catch (std::exception e) {
			DSKUtilities::throwJavaDSKException(env, e.what());
			return;
		}
}

/*
* Class:     com_kps_dsk_DSKController
* Method:    nativePause
* Signature: ()V
*/
JNIEXPORT void JNICALL Java_com_kps_dsk_DSKController_nativePause
	(JNIEnv* env, jobject jController) {
		DSKController* controller = getController(env, jController);
		if (controller == NULL) {
			DSKUtilities::throwJavaDSKException(env, "Could not find a corresponding DSKController");
			return;
		}
		try {
			controller->pause();
		} catch (std::exception e) {
			DSKUtilities::throwJavaDSKException(env, e.what());
			return;
		}
}

/*
* Class:     com_kps_dsk_DSKController
* Method:    nativeStop
* Signature: ()V
*/
JNIEXPORT void JNICALL Java_com_kps_dsk_DSKController_nativeStop
	(JNIEnv* env, jobject jController) {
		DSKController* controller = getController(env, jController);
		if (controller == NULL) {
			DSKUtilities::throwJavaDSKException(env, "Could not find a corresponding DSKController");
			return;
		}
		try {
			controller->stop();
		} catch (std::exception e) {
			DSKUtilities::throwJavaDSKException(env, e.what());
			return;
		}
}

/*
* Class:     com_kps_dsk_DSKController
* Method:    nativePaint
* Signature: (Lcom/kps/dsk/DSKCanvas;)V
*/
JNIEXPORT void JNICALL Java_com_kps_dsk_DSKController_nativePaint
	(JNIEnv* env, jobject jController, jobject canvas) {
		JAWT awt;
		JAWT_DrawingSurface* surface;
		JAWT_DrawingSurfaceInfo* surfaceInfo;
		JAWT_Win32DrawingSurfaceInfo* windowsSurfaceInfo;
		jboolean result;
		jint lock; 

		awt.version = JAWT_VERSION_1_4;
		result = JAWT_GetAWT(env, &awt);
		if(result == JNI_FALSE) {
			DSKUtilities::throwJavaDSKException(env, "JAWT_GetAWT failed");
			return;
		}

		surface = awt.GetDrawingSurface(env, canvas);
		if(surface == NULL) {
			DSKUtilities::throwJavaDSKException(env, "GetDrawingSurface failed");
			return;
		}

		lock = surface->Lock(surface);
		if((lock & JAWT_LOCK_ERROR) != 0) {
			awt.FreeDrawingSurface(surface);
			DSKUtilities::throwJavaDSKException(env, "Lock failed");
			return;
		}

		surfaceInfo = surface->GetDrawingSurfaceInfo(surface);
		if(surfaceInfo == NULL) {
			surface->Unlock(surface);
			awt.FreeDrawingSurface(surface);
			DSKUtilities::throwJavaDSKException(env, "GetDrawingSurfaceInfo failed");
			return;
		}

		windowsSurfaceInfo = (JAWT_Win32DrawingSurfaceInfo*)surfaceInfo->platformInfo;

		HDC hdc = windowsSurfaceInfo->hdc;

		DSKController* controller = getController(env, jController);
		if (controller == NULL) {
			surface->FreeDrawingSurfaceInfo(surfaceInfo);
			surface->Unlock(surface);
			awt.FreeDrawingSurface(surface);
			DSKUtilities::throwJavaDSKException(env, "Could not find a corresponding DSKController");
			return;
		}

		try {
			controller->paint(hdc);
		} catch (std::exception e) {
			surface->FreeDrawingSurfaceInfo(surfaceInfo);
			surface->Unlock(surface);
			awt.FreeDrawingSurface(surface);
			DSKUtilities::throwJavaDSKException(env, e.what());
			return;
		}

		surface->FreeDrawingSurfaceInfo(surfaceInfo);
		surface->Unlock(surface);
		awt.FreeDrawingSurface(surface);
}

/*
* Class:     com_kps_dsk_DSKController
* Method:    nativeGetVideoSize
* Signature: ()Ljava/awt/Dimension;
*/
JNIEXPORT jobject JNICALL Java_com_kps_dsk_DSKController_nativeGetVideoSize
	(JNIEnv* env, jobject jController) {
		DSKController* controller = getController(env, jController);
		if (controller == NULL) {
			DSKUtilities::throwJavaDSKException(env, "Could not find a corresponding DSKController");
			return NULL;
		}

		LONG width;
		LONG height;
		try {
			controller->getVideoSize(&width, &height);
		} catch (std::exception e) {
			DSKUtilities::throwJavaDSKException(env, e.what());
			return NULL;
		}

		jclass dimensionClass = env->FindClass("java/awt/Dimension");
		jmethodID dimensionConstructor = env->GetMethodID(dimensionClass, "<init>", "(II)V");
		jobject dimension = env->NewObject(dimensionClass, dimensionConstructor, width, height);

		return dimension;
}