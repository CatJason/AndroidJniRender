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

    InitializeModelDefaultPosition();
    myGLCamera->SetModelPosition(modelDefaultPosition);
}

ModelAssimp::~ModelAssimp() = default;

void ModelAssimp::InitializeModelDefaultPosition() {
    float pos[] = {0.0f, 0.0f, 0.0f, 0.2f, 0.5f, 0.0f};
    std::copy(pos, pos + 6, std::back_inserter(modelDefaultPosition));
}

/**
 * 执行初始化操作并将三角形的顶点/颜色加载到GLES中
 */
void ModelAssimp::PerformGLInits() {
    GLConfigInit();

    assimpLoader = std::make_unique<AssimpLoader>();

    std::vector<std::string> filenames = LoadAssetPaths();

    // 加载3D模型
    assimpLoader->Load3DModel(filenames[0]);

    CheckGLError("ModelAssimp::PerformGLInits");
    initsDone = true;
}

std::vector<std::string> ModelAssimp::LoadAssetPaths() {
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
        filenames.push_back(filename);
    }

    return filenames;
}

/**
 * 渲染到显示屏
 */
void ModelAssimp::Render() {
    ClearScreen();
    RenderModel();
    CheckGLError("ModelAssimp::Render");
}

void ModelAssimp::ClearScreen() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void ModelAssimp::RenderModel() {
    glm::mat4 mvpMat = myGLCamera->GetMVP();
    assimpLoader->Render3DModel(&mvpMat);
}

/**
 * 设置视口，该函数在用户改变设备方向时也会被调用。
 */
void ModelAssimp::SetViewport(int width, int height) {
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
    myGLCamera->SetModelPosition(modelDefaultPosition);
}

/**
 * 如果用户用一根手指滚动，旋转模型。
 */
void ModelAssimp::ScrollAction(float distanceX, float distanceY, float positionX, float positionY) {
    myGLCamera->RotateModel(distanceX, distanceY, positionX, positionY);
}

/**
 * 捏合缩放：将模型拉近或拉远。
 */
void ModelAssimp::ScaleAction(float scaleFactor) {
    myGLCamera->ScaleModel(scaleFactor);
}

/**
 * 双指拖动：通过改变模型的 x 和 y 坐标来移动模型位置。
 */
void ModelAssimp::MoveAction(float distanceX, float distanceY) {
    myGLCamera->TranslateModel(distanceX, distanceY);
}
