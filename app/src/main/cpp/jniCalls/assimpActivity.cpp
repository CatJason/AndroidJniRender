#include <jni.h>
#include "../nativeCode/modelLoader/modelAssimp.h"
#include "../nativeCode/utils/assetManager.h"

#ifdef __cplusplus
extern "C" {
#endif

// global pointer is used in JNI calls to reference to same object of type Cube
ModelAssimp *gAssimpObject = nullptr;

// global pointer to instance of MyJNIHelper that is used to read from assets
AssetManagerUtils *gHelperObject = nullptr;

JNIEXPORT void JNICALL
Java_com_anandmuralidhar_assimpandroid_AssimpActivity_createObjectNative(
        JNIEnv *env,
        jobject instance,
        jobject assetManager,
        jstring pathToInternalDir
) {
    // 创建一个MyJNIHelper对象并将其赋值给全局变量gHelperObject
    gHelperObject = new AssetManagerUtils(env, instance, assetManager, pathToInternalDir);
    // 创建一个ModelAssimp对象并将其赋值给全局变量gAssimpObject
    gAssimpObject = new ModelAssimp();
}

JNIEXPORT void JNICALL
Java_com_anandmuralidhar_assimpandroid_AssimpActivity_deleteObjectNative(
        JNIEnv *env,
        jobject instance
) {
    // 检查gAssimpObject是否为nullptr（即是否已分配内存）
    if (gAssimpObject != nullptr) {
        // 如果已分配内存，则删除gAssimpObject指向的对象
        delete gAssimpObject;
    }
    // 将gAssimpObject指针置为空指针
    gAssimpObject = nullptr;
    // 检查gHelperObject是否为nullptr（即是否已分配内存）
    if (gHelperObject != nullptr) {
        // 如果已分配内存，则删除gHelperObject指向的对象
        delete gHelperObject;
    }
    // 将gHelperObject指针置为空指针
    gHelperObject = nullptr;
}

#ifdef __cplusplus
}
#endif
