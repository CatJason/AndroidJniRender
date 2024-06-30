#include <jni.h> // 引入JNI头文件，用于实现Java与本地C/C++代码之间的互操作性。
#include <modelLoader/AssimpPinkFoxModel.h> // 引入Assimp模型加载功能的特定头文件。
#include <utils/assetManager.h>
#include <modelLoader/AssimpYinModel.h> // 引入Assimp模型加载功能的特定头文件。

#ifdef __cplusplus
extern "C" { // 确保与C语言的兼容性，因为JNI使用C语言命名和链接约定。
#endif

// 使用全局指针在JNI调用中引用同一个类型为Cube的对象
AssimpPinkFoxModel *gAssimpPinkFoxModel = nullptr;
AssimpYinModel *gAssimpYinModel = nullptr;

// 使用全局指针引用MyJNIHelper的实例，该实例用于从assets读取资源
AssetManagerUtils *gHelperObject = nullptr;
/**
 * 创建3D模型对象和辅助资源管理对象，并将它们分别赋值给全局指针。
 *
 * @param env JNI接口指针，用于函数调用。
 * @param instance Java层传递的对象实例。
 * @param assetManager Java层传递的AssetManager对象，用于访问assets文件夹。
 * @param pathToInternalDir Java层传递的应用内部目录路径，用于数据存储。
 */
JNIEXPORT void JNICALL
Java_com_anandmuralidhar_assimpandroid_AssimpActivity_createObjectNative(
        JNIEnv *env,
        jobject instance,
        jobject assetManager,
        jstring pathToInternalDir
) {
    // 创建AssetManagerUtils对象并将其赋值给全局变量gHelperObject，用于读取assets
    gHelperObject = new AssetManagerUtils(env, instance, assetManager, pathToInternalDir);
    // 创建ModelAssimp对象并将其赋值给全局变量gAssimpObject，用于3D模型处理
    gAssimpPinkFoxModel = new AssimpPinkFoxModel();
    gAssimpYinModel = new AssimpYinModel();
}

/**
 * 删除先前创建的3D模型对象和辅助资源管理对象，并清理它们占用的资源。
 *
 * @param env JNI接口指针，用于函数调用。
 * @param instance Java层传递的对象实例。
 */
JNIEXPORT void JNICALL
Java_com_anandmuralidhar_assimpandroid_AssimpActivity_deleteObjectNative(
        JNIEnv *env,
        jobject instance
) {
    // 如果gAssimpObject不是nullptr（即已分配内存），则删除gAssimpObject指向的对象
    if (gAssimpPinkFoxModel != nullptr) {
        delete gAssimpPinkFoxModel;
    }
    // 将gAssimpObject指针置为nullptr，表示不再指向任何对象
    gAssimpPinkFoxModel = nullptr;

    if (gAssimpYinModel != nullptr) {
        delete gAssimpYinModel;
    }
    gAssimpYinModel = nullptr;

    // 如果gHelperObject不是nullptr（即已分配内存），则删除gHelperObject指向的对象
    if (gHelperObject != nullptr) {
        delete gHelperObject;
    }
    // 将gHelperObject指针置为nullptr，表示不再指向任何对象
    gHelperObject = nullptr;
}

#ifdef __cplusplus
}
#endif