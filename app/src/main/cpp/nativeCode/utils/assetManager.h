#ifndef MY_JNI_HELPER_H
#define MY_JNI_HELPER_H

#include "logUtils.h"
#include <pthread.h>
#include <string>
#include <jni.h>
#include <vector>
#include <android/asset_manager.h>

#ifdef __cplusplus
extern "C" {
#endif

class AssetManagerUtils {

private:
    mutable pthread_mutex_t threadMutex{};
    // 用于保存从 JNI 中获取的应用的内部数据存储路径的字符串表示。
    std::string apkInternalPath;
    AAssetManager *apkAssetManager;

public:
    AssetManagerUtils(JNIEnv *env, jobject obj, jobject assetManager, jstring pathToInternalDir);

    ~AssetManagerUtils();

    /**
    * 在assets中搜索文件，提取文件并保存到内部存储中，并返回新的文件路径
    *
    * @param assetName 要提取的文件名（相对路径）
    * @param filename 保存提取后的文件名的引用
    * @param checkIfFileIsAvailable 是否检查文件是否可用
    * @return 提取是否成功的布尔值，true表示成功提取文件，false表示提取失败
    */
    bool ExtractAssetReturnFilename(
            std::string assetName,
            std::string &filename,
            bool checkIfFileIsAvailable = false
    );

    bool ExtractAssetReturnFilename(std::string assetName, bool checkIfFileIsAvailable = false);
};

extern AssetManagerUtils *gHelperObject;

#ifdef __cplusplus
}
#endif

#endif // #define MY_JNI_HELPER_H
