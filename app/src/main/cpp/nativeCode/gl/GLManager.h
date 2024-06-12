#ifndef MY_GL_FUNCTIONS_H
#define MY_GL_FUNCTIONS_H

#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <stdio.h>
#include <string>

/**
 * 初始化OpenGL的基本配置。
 */
void GLConfigInit();

/**
 * 检查OpenGL错误。
 * @param functionName 调用该函数的函数名称
 */
void CheckGLError(const std::string& functionName);

void CheckGLESSupport();

void LogGLError(GLenum err);

#endif //MY_GL_FUNCTIONS_H
