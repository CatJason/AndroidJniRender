#include "modelAssimp.h"
#include "assimp/Importer.hpp"
#include "../utils/assetManager.h"
#include <opencv2/opencv.hpp>
#include <memory>
#include <vector>
#include <algorithm>

/**
 * 类构造函数
 */
ModelAssimp::ModelAssimp()
        : initsDone(false),
          myGLCamera(std::make_unique<GLCamera>()),
          assimpLoader(nullptr) {

    MyLOGI("ModelAssimp::ModelAssimp - 构造函数初始化");
    InitializeModelDefaultPosition();
    myGLCamera->SetModelPosition(modelDefaultPosition);
}

ModelAssimp::~ModelAssimp() {
    MyLOGI("ModelAssimp::~ModelAssimp - 析构函数调用");
}

void ModelAssimp::InitializeModelDefaultPosition() {
    MyLOGI("ModelAssimp::InitializeModelDefaultPosition - 初始化模型默认位置");
    float pos[] = {0.0f, 0.0f, 0.0f, 0.2f, 0.5f, 0.0f};
    std::copy(pos, pos + 6, std::back_inserter(modelDefaultPosition));
}

/**
 * 执行初始化操作并将三角形的顶点/颜色加载到GLES中
 */
void ModelAssimp::PerformGLInits() {
    MyLOGI("ModelAssimp::PerformGLInits - 执行OpenGL初始化操作");
    GLConfigInit();

    assimpLoader = std::make_unique<AssimpLoader>();

    std::vector<std::string> filenames = LoadAssetPaths();

    // 加载3D模型
    assimpLoader->Load3DModel(filenames[0]);

    CheckGLError("ModelAssimp::PerformGLInits");
    initsDone = true;
}

std::vector<std::string> ModelAssimp::LoadAssetPaths() {
    MyLOGI("ModelAssimp::LoadAssetPaths - 加载资源路径");
    const std::vector<const char*> assetPaths = {
            "pinkfox/pinkFox.obj",
            "pinkfox/pinkFox.mtl",
            "pinkfox/body.jpg",
            "pinkfox/hair.jpg",
            "pinkfox/skin.jpg",
            "pinkfox/face.jpg"
    };

    std::vector<std::string> filenames;
    filenames.reserve(assetPaths.size());

    for (const auto& path : assetPaths) {
        std::string filename;
        gHelperObject->ExtractAssetReturnFilename(path, filename);
        MyLOGI("ModelAssimp::LoadAssetPaths - 提取资源文件: %s", filename.c_str());
        filenames.push_back(filename);
    }

    return filenames;
}

/**
 * 渲染到显示屏
 */
void ModelAssimp::Render() {
    // MyLOGI("ModelAssimp::Render - 渲染到显示屏");
    ClearScreen();
    RenderModel();
    CheckGLError("ModelAssimp::Render");
}

void ModelAssimp::ClearScreen() {
    // MyLOGI("ModelAssimp::ClearScreen - 清除屏幕");
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void ModelAssimp::RenderModel() {
    // MyLOGI("ModelAssimp::RenderModel - 渲染模型");
    glm::mat4 mvpMat = myGLCamera->GetMVP();
    assimpLoader->Render3DModel(&mvpMat);
}

/**
 * 设置视口，该函数在用户改变设备方向时也会被调用。
 */
void ModelAssimp::SetViewport(int width, int height) {
    MyLOGI("ModelAssimp::SetViewport - 设置视口: width=%d, height=%d", width, height);
    screenHeight = height;
    screenWidth = width;
    glViewport(0, 0, width, height);
    CheckGLError("ModelAssimp::SetViewport");

    myGLCamera->SetAspectRatio(static_cast<float>(width) / height);
}

/**
 * 在双击操作中重置模型的位置。
 */
void ModelAssimp::DoubleTapAction() {
    MyLOGI("ModelAssimp::DoubleTapAction - 双击操作重置模型位置");
    myGLCamera->SetModelPosition(modelDefaultPosition);
}

/**
 * 如果用户用一根手指滚动，旋转模型。
 */
void ModelAssimp::ScrollAction(float distanceX, float distanceY, float positionX, float positionY) {
    MyLOGI("ModelAssimp::ScrollAction - 滚动操作: distanceX=%f, distanceY=%f, positionX=%f, positionY=%f",
           distanceX, distanceY, positionX, positionY);
    myGLCamera->RotateModel(distanceX, distanceY, positionX, positionY);
}

/**
 * 捏合缩放：将模型拉近或拉远。
 */
void ModelAssimp::ScaleAction(float scaleFactor) {
    MyLOGI("ModelAssimp::ScaleAction - 缩放操作: scaleFactor=%f", scaleFactor);
    myGLCamera->ScaleModel(scaleFactor);
}

/**
 * 双指拖动：通过改变模型的 x 和 y 坐标来移动模型位置。
 */
void ModelAssimp::MoveAction(float distanceX, float distanceY) {
    MyLOGI("ModelAssimp::MoveAction - 移动操作: distanceX=%f, distanceY=%f", distanceX, distanceY);
    myGLCamera->TranslateModel(distanceX, distanceY);
}