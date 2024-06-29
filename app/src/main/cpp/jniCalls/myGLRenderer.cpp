#include <jni.h> // 引入JNI头文件，用于实现Java与本地C/C++代码之间的互操作性。
#include <modelLoader/AssimpPinkFoxModel.h>
#include <modelLoader/AssimpShenHeModel.h>

#ifdef __cplusplus
extern "C" { // 确保与C语言的兼容性，因为JNI使用C语言命名和链接约定。
#endif

extern AssimpPinkFoxModel *gAssimpPinkFoxModel; // 声明一个全局指针，指向Assimp模型对象，供各个本地方法使用。
/**
 * 当需要绘制新的一帧时由Java层调用。
 * 如果Assimp模型对象存在，则调用其渲染方法。
 */
JNIEXPORT void JNICALL
Java_com_anandmuralidhar_assimpandroid_MyGLRenderer_drawFrameNative(
        JNIEnv *env,
        jobject instance
) {

    if (gAssimpPinkFoxModel == nullptr) {
        return; // 如果全局Assimp对象未初始化，则不执行任何操作。
    }
    gAssimpPinkFoxModel->Render(); // 调用Assimp对象的渲染方法来绘制一帧。
}

/**
 * 在OpenGL表面创建时由Java层调用。
 * 如果Assimp模型对象存在，则执行必要的OpenGL初始化操作。
 */
JNIEXPORT void JNICALL
Java_com_anandmuralidhar_assimpandroid_MyGLRenderer_surfaceCreatedNative(
        JNIEnv *env,
        jobject instance
) {

    if (gAssimpPinkFoxModel == nullptr) {
        return; // 如果全局Assimp对象未初始化，则不执行任何操作。
    }
    gAssimpPinkFoxModel->PerformGLInit(); // 调用Assimp对象的OpenGL初始化方法。
}

/**
 * 当OpenGL表面的大小发生变化时由Java层调用。
 * 传入新的宽度和高度，并调整视口大小。
 */
JNIEXPORT void JNICALL
Java_com_anandmuralidhar_assimpandroid_MyGLRenderer_surfaceChangedNative(
        JNIEnv *env,
        jobject instance,
        jint width,
        jint height
) {

    if (gAssimpPinkFoxModel == nullptr) {
        return; // 如果全局Assimp对象未初始化，则不执行任何操作。
    }
    gAssimpPinkFoxModel->SetViewport(width, height); // 根据新的宽度和高度设置视口。
}

#ifdef __cplusplus
}
#endif