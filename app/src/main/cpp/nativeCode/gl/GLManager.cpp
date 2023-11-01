#include "GLManager.h"
#include <sstream>
#include "../utils/logUtils.h"

/**
 * Basic initializations for GL.
 */
void GLConfigInit() {
    // 设置白色背景
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

    // 启用深度测试
    glEnable(GL_DEPTH_TEST);
    // 设置深度测试函数为小于等于关系
    glDepthFunc(GL_LEQUAL);

    // 打印OpenGL版本和GLSL版本信息
    MyLOGI("OpenGL %s, GLSL %s", glGetString(GL_VERSION), glGetString(GL_SHADING_LANGUAGE_VERSION));

    // 检查设备是否支持GLES 3或GLES 2
    const char* versionStr = (const char*)glGetString(GL_VERSION);
    if (strstr(versionStr, "OpenGL ES 3.")) {
        MyLOGD("Device supports GLES 3");
    } else {
        MyLOGD("Device supports GLES 2");
    }

    // 检查OpenGL错误
    CheckGLError("MyGLInits");
}

/**
 * 检查OpenGL错误。
 */
void CheckGLError(std::string funcName) {
    GLenum err = glGetError();  // 获取当前的OpenGL错误状态
    if (err == GL_NO_ERROR) {  // 如果错误状态为GL_NO_ERROR，表示没有错误发生
        return;  // 函数直接返回
    } else {
        MyLOGF("[FAIL GL] %s", funcName.c_str());  // 如果有错误发生，使用MyLOGF函数打印错误信息，并使用传入的funcName参数作为函数名称
    }

    switch(err) {
        case GL_INVALID_ENUM:
            MyLOGE("GL_INVALID_ENUM: 参数超出范围");  // 参数超出范围的错误信息
            break;
        case GL_INVALID_VALUE:
            MyLOGE("GL_INVALID_VALUE: 数值参数超出范围");  // 数值参数超出范围的错误信息
            break;
        case GL_INVALID_OPERATION:
            MyLOGE("GL_INVALID_OPERATION: 在当前状态下非法操作");  // 在当前状态下非法操作的错误信息
            break;
        case GL_INVALID_FRAMEBUFFER_OPERATION:
            MyLOGE("GL_INVALID_FRAMEBUFFER_OPERATION: 帧缓冲对象不完整");  // 帧缓冲对象不完整的错误信息
            break;
        case GL_OUT_OF_MEMORY:
            MyLOGE("GL_OUT_OF_MEMORY: 内存不足，无法执行命令");  // 内存不足，无法执行命令的错误信息
            break;
        default:
            MyLOGE("未知错误");  // 其他未知错误的错误信息
            break;
    }
}