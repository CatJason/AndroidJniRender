#include "assetManager.h"
#include "fileUtils.h"
#include <android/asset_manager_jni.h>

/**
 * JniUtils 构造函数
 *
 * @param env JNIEnv 指针，Java Native Interface 的环境指针
 * @param instance jobject 对象，Java 调用本地方法的对象实例
 *
 * @param assetManager jobject 对象，AssetManager 的实例对象
 * @param pathToInternalDir jstring 字符串，应用的内部数据存储路径
 */
AssetManagerUtils::AssetManagerUtils(
        JNIEnv *env,
        jobject instance,
        jobject assetManager,
        jstring pathToInternalDir
) {
    MyLOGI("AssetManagerUtils::AssetManagerUtils - 构造函数初始化");
    // 构造函数，接收JNIEnv指针、jobject对象、assetManager对象和pathToInternalDir字符串作为参数

    // 获取asset manager的本地实例
    // assetManager是从Java传递过来的对象，不应该被垃圾回收!
    apkAssetManager = AAssetManager_fromJava(env, assetManager);

    // 保存应用的内部数据存储路径，我们将从这里提取资产并保存
    // 从 Java 的 jstring 对象获取内部数据存储路径的 C 字符串表示
    const char *cPathToInternalDir;
    cPathToInternalDir = env->GetStringUTFChars(pathToInternalDir, nullptr);

    // 将 C 字符串转换为 std::string，并保存为 apkInternalPath
    apkInternalPath = std::string(cPathToInternalDir);

    // 释放 GetStringUTFChars 函数获取的 C 字符串，防止内存泄漏
    env->ReleaseStringUTFChars(pathToInternalDir, cPathToInternalDir);

    // 用于线程安全的互斥锁
    pthread_mutex_init(&threadMutex, nullptr);
}

AssetManagerUtils::~AssetManagerUtils() {
    MyLOGI("AssetManagerUtils::~AssetManagerUtils - 析构函数调用");
    // 析构函数，用于清理资源和对象的销毁

    // 销毁互斥锁对象
    pthread_mutex_destroy(&threadMutex);
}

/**
* 在assets中搜索文件，提取文件并保存到内部存储中，并返回新的文件路径
*
* @param assetName 要提取的文件名（相对路径）
* @param filename 保存提取后的文件名的引用
* @param checkIfFileIsAvailable 是否检查文件是否可用
* @return 提取是否成功的布尔值，true表示成功提取文件，false表示提取失败
*/
bool AssetManagerUtils::ExtractAssetReturnFilename(std::string assetName, bool checkIfFileIsAvailable) {
    MyLOGI("AssetManagerUtils::ExtractAssetReturnFilename - 提取资产文件：%s", assetName.c_str());
    std::string filename;
    return ExtractAssetReturnFilename(assetName, filename, checkIfFileIsAvailable);
}

bool AssetManagerUtils::ExtractAssetReturnFilename(
        std::string assetName,
        std::string &filename,
        bool checkIfFileIsAvailable
) {
    MyLOGI("AssetManagerUtils::ExtractAssetReturnFilename - 提取资产文件：%s", assetName.c_str());
    filename = apkInternalPath + "/" + ExtractFileName(assetName);
    FILE *file = fopen(filename.c_str(), "rb");
    if (file && checkIfFileIsAvailable) {
        MyLOGI("在assets中找到已提取的文件：%s", filename.c_str());
        fclose(file);  // 关闭文件
        pthread_mutex_unlock(&threadMutex);  // 解锁互斥锁
        return true;  // 返回true，表示文件已存在且可用
    }

    // 在assets中查找文件
    // AAsset对象在多线程环境中不是线程安全的，需要使用互斥锁进行保护
    pthread_mutex_lock(&threadMutex);

    // 打开文件
    AAsset *asset = AAssetManager_open(apkAssetManager, assetName.c_str(), AASSET_MODE_STREAMING);
    if (asset != nullptr) {
        MyLOGI("AssetManagerUtils::ExtractAssetReturnFilename - 资产文件打开成功：%s", assetName.c_str());
        char buf[BUFSIZ];
        int nb_read = 0;
        // 打开文件以便写入提取的资源数据
        FILE *out = fopen(filename.c_str(), "w");

        // 从AAsset中读取数据，并写入到文件中
        // 循环读取数据直到读取完整个资源文件
        while ((nb_read = AAsset_read(asset, buf, BUFSIZ)) > 0) {
            fwrite(buf, nb_read, 1, out);  // 将读取的数据写入文件
        }

        fclose(out);  // 关闭文件
        AAsset_close(asset);  // 关闭AAsset资源对象

        MyLOGI("提取资源文件：%s", filename.c_str());
        pthread_mutex_unlock(&threadMutex);
        return true;

    }

    // 如果AAsset为空，说明找不到指定的资源文件
    MyLOGE("找不到资源文件：%s", assetName.c_str());
    pthread_mutex_unlock(&threadMutex);
    return false;
}