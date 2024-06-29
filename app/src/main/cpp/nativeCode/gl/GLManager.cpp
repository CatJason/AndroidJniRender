#include <GLManager.h>
#include <sstream>
#include <utils/logUtils.h>

/**
 * 初始化OpenGL的基本配置。
 */
void GLConfigInit() {
    MyLOGI("GLConfigInit - 初始化OpenGL配置");
    // 设置背景颜色为白色
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    MyLOGI("GLConfigInit - 设置背景颜色为白色");

    // 启用深度测试并设置深度测试函数为小于等于
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    MyLOGI("GLConfigInit - 启用深度测试并设置深度测试函数为小于等于");

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
    MyLOGI("CheckGLESSupport - 检查设备是否支持GLES 3或GLES 2");
    const char* versionStr = reinterpret_cast<const char*>(glGetString(GL_VERSION));
    if (strstr(versionStr, "OpenGL ES 3.")) {
        MyLOGD("CheckGLESSupport - 设备支持GLES 3");
    } else {
        MyLOGD("CheckGLESSupport - 设备支持GLES 2");
    }
}

/**
 * 检查OpenGL错误。
 * @param funcName 函数名称
 */
void CheckGLError(const std::string& funcName) {
    // MyLOGI("CheckGLError - 检查OpenGL错误: %s", funcName.c_str());
    GLenum err = glGetError();
    if (err == GL_NO_ERROR) {
        // MyLOGI("CheckGLError - 没有检测到错误");
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
    MyLOGI("LogGLError - 记录OpenGL错误信息: %d", err);
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