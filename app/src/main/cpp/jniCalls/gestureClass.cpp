#include <jni.h>  // 引入JNI头文件，实现Java和本地C/C++代码之间的互操作。
#include "../nativeCode/modelLoader/AssimpPinkFoxModel.h"  // 引入Assimp模型加载功能的特定头文件。
#include "AssimpShenHeModel.h"

#ifdef __cplusplus
extern "C" {  // 确保与C的链接兼容性，因为JNI使用C的命名约定来命名本地方法。
#endif

extern AssimpPinkFoxModel *gAssimpPinkFoxModel;  // 声明一个指向Assimp模型对象的全局指针，用于不同的本地方法中。
extern AssimpShenHeModel *gAssimpShenHeModel;  // 声明一个指向Assimp模型对象的全局指针，用于不同的本地方法中。

/**
 * 响应屏幕上的双击手势。
 * 如果Assimp模型对象存在，触发模型的双击操作。
 */
JNIEXPORT void JNICALL
Java_com_anandmuralidhar_assimpandroid_GestureClass_DoubleTapNative(JNIEnv *env, jobject instance) {
    if (gAssimpPinkFoxModel == nullptr) {
        return;  // 如果全局Assimp对象未初始化，什么也不做。
    }
    gAssimpPinkFoxModel->DoubleTapAction();  // 调用Assimp对象的双击动作。
}

/**
 * 处理单指拖动手势。
 * 计算从前一个位置到当前位置的标准化位置和位移，
 * 然后将这些值传递给模型的滚动动作方法。
 */
JNIEXPORT void JNICALL
Java_com_anandmuralidhar_assimpandroid_GestureClass_ScrollNative(
        JNIEnv *env,
        jobject instance,
        jfloat distanceX, jfloat distanceY,
        jfloat positionX,
        jfloat positionY
) {

    if (gAssimpPinkFoxModel == nullptr) {
        return;  // 如果全局Assimp对象未初始化，什么也不做。
    }
    // 根据GL表面尺寸对屏幕上的移动进行标准化
    // 为了与OpenGL ES（GLES）约定一致，将dY翻转
    float dX = distanceX / gAssimpPinkFoxModel->GetScreenWidth();
    float dY = -distanceY / gAssimpPinkFoxModel->GetScreenHeight();
    float posX = 2 * positionX / gAssimpPinkFoxModel->GetScreenWidth() - 1.;
    float posY = -2 * positionY / gAssimpPinkFoxModel->GetScreenHeight() + 1.;
    // 将位置限制在[-1, 1]范围内，确保它们在GL视口内
    posX = fmax(-1., fmin(1., posX));
    posY = fmax(-1., fmin(1., posY));
    gAssimpPinkFoxModel->ScrollAction(dX, dY, posX, posY);  // 将标准化的值传递给模型的滚动动作方法。
}

/**
 * 响应捏合缩放手势。
 * 将缩放因子传递给模型的缩放动作方法，以调整模型的大小。
 */
JNIEXPORT void JNICALL
Java_com_anandmuralidhar_assimpandroid_GestureClass_ScaleNative(
        JNIEnv *env,
        jobject instance,
        jfloat scaleFactor
) {

    if (gAssimpPinkFoxModel == nullptr) {
        return;  // 如果全局Assimp对象未初始化，什么也不做。
    }
    gAssimpPinkFoxModel->ScaleAction((float) scaleFactor);  // 将缩放因子传递给模型的缩放动作方法。

}

/**
 * 两指拖动手势处理 - 根据GLES表面大小标准化移动距离。
 */
JNIEXPORT void JNICALL
Java_com_anandmuralidhar_assimpandroid_GestureClass_MoveNative(
        JNIEnv *env,
        jobject instance,
        jfloat distanceX,
        jfloat distanceY
) {

    if (gAssimpPinkFoxModel == nullptr) {
        return;  // 如果全局Assimp对象未初始化，什么也不做。
    }

    // 根据GL表面尺寸对屏幕上的移动进行标准化
    // 为了与OpenGL ES（GLES）约定一致，将dY翻转
    float dX = distanceX / gAssimpPinkFoxModel->GetScreenWidth();
    float dY = -distanceY / gAssimpPinkFoxModel->GetScreenHeight();
    gAssimpPinkFoxModel->MoveAction(dX, dY);  // 将标准化的移动距离传递给模型的移动动作方法。

}

#ifdef __cplusplus
}
#endif