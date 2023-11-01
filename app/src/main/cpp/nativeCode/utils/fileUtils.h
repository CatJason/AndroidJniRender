#ifndef MISC_H
#define MISC_H

#include <stdio.h>
#include <string>
#include "logUtils.h"
#include "../gl/myGLM.h"

/**
 * 去除路径并返回文件名
 * @param fileName 文件名（包含路径）
 * @return 文件名（不包含路径）
 */
std::string ExtractFileName(std::string fileName);

/**
 * 从文件名中提取目录部分
 * @param fullFileName 完整的文件名（包含路径）
 * @return 目录部分（不包含文件名）
 */
std::string ExtractDirectoryName(std::string fullFileName);

#endif //MISC_H
