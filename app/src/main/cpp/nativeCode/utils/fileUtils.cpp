#include "fileUtils.h"

/**
 * 去除路径并返回文件名
 * @param fileName 文件名（包含路径）
 * @return 文件名（不包含路径）
 */
std::string ExtractFileName(std::string fileName) {
    std::size_t slashIndex = fileName.find_last_of("/");
    std::string onlyName;

    if (slashIndex == std::string::npos) {
        onlyName = fileName;
    } else {
        onlyName = fileName.substr(slashIndex + 1);
    }

    return onlyName;
}

/**
 * 从文件名中提取目录部分
 * @param fullFileName 完整的文件名（包含路径）
 * @return 目录部分（不包含文件名）
 */std::string ExtractDirectoryName(std::string fullFileName) {
    std::string directoryName;
    std::size_t slashIndex = fullFileName.find_last_of("/");

    if (slashIndex == std::string::npos) {
        directoryName = ".";
    } else {
        directoryName = fullFileName.substr(0, slashIndex);
    }

    return directoryName;
}

