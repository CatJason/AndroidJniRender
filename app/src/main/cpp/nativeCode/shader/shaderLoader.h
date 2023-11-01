#ifndef MY_SHADER_H
#define MY_SHADER_H
#include "../gl/GLManager.h"
#include "../utils/logUtils.h"
#include <string>

/**
* 读取顶点着色器和片段着色器，编译并链接它们，返回程序ID
*
* @param vertexShaderFilename 顶点着色器文件的文件名（相对路径）
* @param fragmentShaderFilename 片段着色器文件的文件名（相对路径）
* @return 创建的着色器程序的标识符，如果加载和编译出错则返回0
*/
GLuint LoadShaders(std::string vertexShaderCode, std::string fragmentShaderCode);

/**
 * 获取着色器中输入变量的属性位置
 *
 * @param programID 着色器程序的标识符
 * @param variableName 属性的名称
 * @return 属性的位置（标识符），如果获取失败则返回0
 */
GLuint GetAttributeLocation(GLuint programID, std::string variableName);

/**
 * 获取着色器程序中指定 uniform 变量的位置
 *
 * @param programID 着色器程序的标识符
 * @param uniformName uniform 变量的名称
 * @return uniform 变量的位置，如果获取失败返回 -1
 */
GLint GetUniformLocation(GLuint programID, std::string uniformName);

#endif //MY_SHADER_H
