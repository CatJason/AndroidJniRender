#include <assimpPinkFoxModel.h>
#include <assimp/Importer.hpp>
#include <utils/assetManager.h>
#include <opencv2/opencv.hpp>
#include <memory>
#include <vector>
#include <algorithm>

ModelAssimp::ModelAssimp()
        : initsDone(false),
          myGLCamera(std::make_unique<GLCamera>()),
          assimpLoader(nullptr),
          rotationAngle(0.0f) {

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

void ModelAssimp::PerformGLInits() {
    MyLOGI("ModelAssimp::PerformGLInits - 执行OpenGL初始化操作");
    GLConfigInit();

    assimpLoader = std::make_unique<AssimpLoader>();

    std::vector<std::string> filenames = LoadAssetPaths();

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

void ModelAssimp::Render() {
    ClearScreen();
    UpdateRotation();
    RenderModel();
    CheckGLError("ModelAssimp::Render");
}

void ModelAssimp::ClearScreen() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void ModelAssimp::RenderModel() {
    glm::mat4 mvpMat = myGLCamera->GetMVP() * modelMatrix;
    assimpLoader->Render3DModel(&mvpMat);
}

void ModelAssimp::SetViewport(int width, int height) {
    MyLOGI("ModelAssimp::SetViewport - 设置视口: width=%d, height=%d", width, height);
    screenHeight = height;
    screenWidth = width;
    glViewport(0, 0, width, height);
    CheckGLError("ModelAssimp::SetViewport");

    myGLCamera->SetAspectRatio(static_cast<float>(width) / height);
}

void ModelAssimp::DoubleTapAction() {
    MyLOGI("ModelAssimp::DoubleTapAction - 双击操作重置模型位置");
    myGLCamera->SetModelPosition(modelDefaultPosition);
}

void ModelAssimp::ScrollAction(float distanceX, float distanceY, float positionX, float positionY) {
    MyLOGI("ModelAssimp::ScrollAction - 滚动操作: distanceX=%f, distanceY=%f, positionX=%f, positionY=%f",
           distanceX, distanceY, positionX, positionY);
    myGLCamera->RotateModel(distanceX, distanceY, positionX, positionY);
}

void ModelAssimp::ScaleAction(float scaleFactor) {
    MyLOGI("ModelAssimp::ScaleAction - 缩放操作: scaleFactor=%f", scaleFactor);
    myGLCamera->ScaleModel(scaleFactor);
}

void ModelAssimp::MoveAction(float distanceX, float distanceY) {
    MyLOGI("ModelAssimp::MoveAction - 移动操作: distanceX=%f, distanceY=%f", distanceX, distanceY);
    myGLCamera->TranslateModel(distanceX, distanceY);
}

void ModelAssimp::UpdateRotation() {
    rotationAngle += 0.01f; // 调整旋转速度
    modelMatrix = glm::rotate(glm::mat4(1.0f), rotationAngle, glm::vec3(0.0f, 1.0f, 0.0f));
}