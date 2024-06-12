#include "GLManager.h"
#include <sstream>
#include "../utils/logUtils.h"

/**
 * 初始化OpenGL的基本配置。
 */
void GLConfigInit() {
    // 设置背景颜色为白色
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

    // 启用深度测试并设置深度测试函数为小于等于
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    // 打印OpenGL版本和GLSL版本信息
    MyLOGI("OpenGL %s, GLSL %s", glGetString(GL_VERSION), glGetString(GL_SHADING_LANGUAGE_VERSION));

    // 检查设备是否支持GLES 3或GLES 2
    CheckGLESSupport();

    // 检查OpenGL错误
    CheckGLError("GLConfigInit");
}

/**
 * 检查设备是否支持GLES 3或GLES 2。
 */
void CheckGLESSupport() {
    const char* versionStr = reinterpret_cast<const char*>(glGetString(GL_VERSION));
    if (strstr(versionStr, "OpenGL ES 3.")) {
        MyLOGD("Device supports GLES 3");
    } else {
        MyLOGD("Device supports GLES 2");
    }
}

/**
 * 检查OpenGL错误。
 * @param funcName 函数名称
 */
void CheckGLError(const std::string& funcName) {
    GLenum err = glGetError();
    if (err == GL_NO_ERROR) {
        return;
    }

    MyLOGF("[FAIL GL] %s", funcName.c_str());
    LogGLError(err);
}

/**
 * 记录OpenGL错误信息。
 * @param err OpenGL错误码
 */
void LogGLError(GLenum err) {
    switch(err) {
        case GL_INVALID_ENUM:
            MyLOGE("GL_INVALID_ENUM: 参数超出范围");
            break;
        case GL_INVALID_VALUE:
            MyLOGE("GL_INVALID_VALUE: 数值参数超出范围");
            break;
        case GL_INVALID_OPERATION:
            MyLOGE("GL_INVALID_OPERATION: 在当前状态下非法操作");
            break;
        case GL_INVALID_FRAMEBUFFER_OPERATION:
            MyLOGE("GL_INVALID_FRAMEBUFFER_OPERATION: 帧缓冲对象不完整");
            break;
        case GL_OUT_OF_MEMORY:
            MyLOGE("GL_OUT_OF_MEMORY: 内存不足，无法执行命令");
            break;
        default:
            MyLOGE("未知错误");
            break;
    }
}