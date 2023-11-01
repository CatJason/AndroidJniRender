#include <iostream>
#include <fstream>
#include "shaderLoader.h"
#include "../utils/assetManager.h"

/**
 * 从assets中读取着色器代码
 *
 * @param shaderCode 输出参数，用于存储读取的着色器代码
 * @param shaderFileName 着色器文件名
 * @return 是否成功读取着色器代码的布尔值，true表示成功，false表示失败
 */
bool ReadShaderCode(std::string &shaderCode, std::string &shaderFileName) {
    MyLOGI("正在读取着色器：%s", shaderFileName.c_str());

    // Android中的着色器存储在assets中
    // 使用MyJNIHelper从assets中读取着色器代码
    bool isFilePresent = gHelperObject->ExtractAssetReturnFilename(shaderFileName, shaderFileName);
    if (!isFilePresent) {
        return false;
    }

    std::ifstream shaderStream(shaderFileName.c_str(), std::ios::in);
    if (shaderStream.is_open()) {
        // 如果成功打开文件，则逐行读取文件内容，并将每行添加到shaderCode中
        std::string Line;
        while (getline(shaderStream, Line)) {
            shaderCode += "\n" + Line;
        }
        shaderStream.close();
    } else {
        // 如果无法打开文件，则记录错误日志并返回false
        MyLOGF("无法打开文件：%s", shaderFileName.c_str());
        return false;
    }

    MyLOGI("读取成功");
    return true;
}

/**
* 编译着色器对象
*
* @param shaderID 着色器对象的标识符（传入时为0，函数内部将创建着色器对象并返回标识符）
* @param shaderType 着色器类型，可以是以下值之一：
 * GL_VERTEX_SHADER              顶点着色器
 * GL_FRAGMENT_SHADER            片段着色器
 * GL_GEOMETRY_SHADER            几何着色器
 * GL_TESS_CONTROL_SHADER        Tessellation 控制着色器
 * GL_TESS_EVALUATION_SHADER     Tessellation 评估着色器
 * GL_COMPUTE_SHADER             计算着色器
* @param shaderCode 着色器源代码
* @return 编译是否成功的布尔值，true表示编译成功，false表示编译失败
*/
bool CompileShader(GLuint &shaderID, const GLenum shaderType, std::string shaderCode) {
    // 创建着色器对象
    shaderID = glCreateShader(shaderType);

    // 编译着色器
    MyLOGI("正在编译着色器");
    // 将 shaderCode.c_str() 的指针赋值给 sourcePointer，这里使用了 const char* 类型
    char const *sourcePointer = shaderCode.c_str();
    // 将着色器源码指针传递给着色器对象
    glShaderSource(shaderID, 1, &sourcePointer, nullptr);
    // 编译着色器对象
    glCompileShader(shaderID);

    // 检查着色器编译结果
    GLint result = GL_FALSE;
    int infoLogLength;

    glGetShaderiv(shaderID, GL_COMPILE_STATUS, &result);
    glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &infoLogLength);
    if (result == 0) {
        MyLOGI("着色器编译失败");
        std::vector<char> shaderErrorMessage(infoLogLength + 1);
        glGetShaderInfoLog(shaderID, infoLogLength, nullptr, &shaderErrorMessage[0]);
        MyLOGI("%s", &shaderErrorMessage[0]);
        return false;
    } else {
        MyLOGI("着色器编译成功");
    }

    return true;
}

/**
 * 将顶点着色器和片段着色器链接到一起
 *
 * @param programID 着色器程序的标识符
 * @param vertexShaderID 顶点着色器的标识符
 * @param fragmentShaderID 片段着色器的标识符
 * @return 链接是否成功的布尔值，true表示链接成功，false表示链接失败
 */
bool LinkProgram(
        GLuint programID,
        GLuint vertexShaderID,
        GLuint fragmentShaderID
) {
    GLint result = GL_FALSE;
    int infoLogLength;

    MyLOGI("正在链接程序");

    glAttachShader(programID, vertexShaderID);
    glAttachShader(programID, fragmentShaderID);
    glLinkProgram(programID);

    // 检查程序
    glGetProgramiv(programID, GL_LINK_STATUS, &result);
    glGetProgramiv(programID, GL_INFO_LOG_LENGTH, &infoLogLength);

    // 通用删除
    if (vertexShaderID) {
        glDeleteShader(vertexShaderID);
    }
    if (fragmentShaderID) {
        glDeleteShader(fragmentShaderID);
    }

    if (result == 0) {
        MyLOGI("链接程序失败：%d", programID);
        std::vector<char> programErrorMessage(infoLogLength + 1);
        glGetProgramInfoLog(programID, infoLogLength, nullptr,
                            &programErrorMessage[0]);
        MyLOGI("%s", &programErrorMessage[0]);
        if (programID) {
            glDeleteProgram(programID);
        }
        return false;
    }
    MyLOGI("链接成功");

    return true;
}

/**
* 读取顶点着色器和片段着色器，编译并链接它们，返回程序ID
*
* @param vertexShaderFilename 顶点着色器文件的文件名（相对路径）
* @param fragmentShaderFilename 片段着色器文件的文件名（相对路径）
* @return 创建的着色器程序的标识符，如果加载和编译出错则返回0
*/
GLuint LoadShaders(
        std::string vertexShaderFilename,
        std::string fragmentShaderFilename
) {
    GLuint vertexShaderID, fragmentShaderID, programID;
    programID = glCreateProgram();

    // 读取并编译顶点着色器
    std::string vertexShaderCode;
    if (!ReadShaderCode(vertexShaderCode, vertexShaderFilename)) {
        MyLOGE("读取顶点着色器出错");
        return 0;
    }
    if (!CompileShader(vertexShaderID, GL_VERTEX_SHADER, vertexShaderCode)) {
        MyLOGE("编译顶点着色器出错");
        return 0;
    }

    // 读取并编译片段着色器
    std::string fragmentShaderCode;
    if (!ReadShaderCode(fragmentShaderCode, fragmentShaderFilename)) {
        MyLOGE("读取片段着色器出错");
        return 0;
    }
    if (!CompileShader(fragmentShaderID, GL_FRAGMENT_SHADER, fragmentShaderCode)) {
        MyLOGE("编译片段着色器出错");
        return 0;
    }

    // 将两个着色器链接在一起
    if (!LinkProgram(programID, vertexShaderID, fragmentShaderID)) {
        MyLOGE("链接着色器出错");
        return 0;
    }

    return programID;
}

/**
 * 获取着色器中输入变量的属性位置
 *
 * @param programID 着色器程序的标识符
 * @param variableName 属性的名称
 * @return 属性的位置（标识符），如果获取失败则返回0
 */
GLuint GetAttributeLocation(GLuint programID, std::string variableName) {
    GLint loc = glGetAttribLocation(programID, variableName.c_str());
    if (loc == -1) {
        MyLOGF("获取属性失败：%s", variableName.c_str());
        return (0);
    } else {
        return ((GLuint) loc);
    }
}

/**
 * 获取着色器程序中指定 uniform 变量的位置
 *
 * @param programID 着色器程序的标识符
 * @param uniformName uniform 变量的名称
 * @return uniform 变量的位置，如果获取失败返回 -1
 */
GLint GetUniformLocation(GLuint programID, std::string uniformName) {
    GLint loc = glGetUniformLocation(programID, uniformName.c_str());
    if (loc == -1) {
        MyLOGF("获取uniform失败：%s", uniformName.c_str());
    } else {
        return loc;
    }
}
