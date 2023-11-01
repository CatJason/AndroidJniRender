#include <jni.h>
#include "../nativeCode/modelLoader/modelAssimp.h"


#ifdef __cplusplus
extern "C" {
#endif

extern ModelAssimp *gAssimpObject;

JNIEXPORT void JNICALL
Java_com_anandmuralidhar_assimpandroid_MyGLRenderer_DrawFrameNative(
        JNIEnv *env,
        jobject instance
) {

    if (gAssimpObject == nullptr) {
        return;
    }
    gAssimpObject->Render();

}

JNIEXPORT void JNICALL
Java_com_anandmuralidhar_assimpandroid_MyGLRenderer_SurfaceCreatedNative(
        JNIEnv *env,
        jobject instance
) {

    if (gAssimpObject == nullptr) {
        return;
    }
    gAssimpObject->PerformGLInits();
}

JNIEXPORT void JNICALL
Java_com_anandmuralidhar_assimpandroid_MyGLRenderer_SurfaceChangedNative(
        JNIEnv *env,
        jobject instance,
        jint width,
        jint height
) {

    if (gAssimpObject == nullptr) {
        return;
    }
    gAssimpObject->SetViewport(width, height);

}

#ifdef __cplusplus
}
#endif

