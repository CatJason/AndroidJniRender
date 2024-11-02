#include <AssimpYinModel.h>
#include <assimp/Importer.hpp>
#include <utils/assetManager.h>
#include <memory>
#include <vector>
#include <algorithm>
#include <utils/uniquePtrUtils.h>

AssimpYinModel::AssimpYinModel()
        : initsDone(false),
          myGLCamera(std::make_unique<GLCamera>()),
          assimpLoader(nullptr),
          rotationAngle(0.0f) {

    MyLOGI("AssimpYinModel::AssimpYinModel - 构造函数初始化");
    InitializeModelDefaultPosition();
    myGLCamera->SetModelPosition(modelDefaultPosition);
}

AssimpYinModel::~AssimpYinModel() {
    MyLOGI("AssimpYinModel::~AssimpYinModel - 析构函数调用");
}

void AssimpYinModel::InitializeModelDefaultPosition() {
    MyLOGI("AssimpYinModel::InitializeModelDefaultPosition - 初始化模型默认位置");
    float pos[] = {5.0f, -13.0f, 0.0f, 0.0f, 0.0f, 0.0f}; // 向左移动
    std::copy(pos, pos + 6, std::back_inserter(modelDefaultPosition));
}

void AssimpYinModel::PerformGLInit(JNIEnv *env) {
    MyLOGI("AssimpYinModel::PerformGLInit - 执行OpenGL初始化操作");
    GLConfigInit();

    assimpLoader = std::make_unique<AssimpLoader>();

    std::vector<std::string> filenames = LoadAssetPaths();

    assimpLoader->Load3DModel(env, filenames[0]);

    CheckGLError("AssimpYinModel::PerformGLInit");
    initsDone = true;
}

std::vector<std::string> AssimpYinModel::LoadAssetPaths() {
    MyLOGI("AssimpYinModel::LoadAssetPaths - 加载资源路径");
    const std::vector<const char*> assetPaths = {
            "yin/yin.obj",
            "yin/yin.mtl",
            "yin/body.png",
            "yin/hair.png",
            "yin/skin.png",
            "yin/face.png",
            "yin/sph.png"
    };

    std::vector<std::string> filenames;
    filenames.reserve(assetPaths.size());

    for (const auto& path : assetPaths) {
        std::string filename;
        gHelperObject->ExtractAssetReturnFilename(path, filename);
        MyLOGI("AssimpYinModel::LoadAssetPaths - 提取资源文件: %s", filename.c_str());
        filenames.push_back(filename);
    }

    return filenames;
}

void AssimpYinModel::Render() {
    UpdateRotation();
    RenderModel();
    CheckGLError("AssimpYinModel::Render");
}

void AssimpYinModel::ClearScreen() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void AssimpYinModel::RenderModel() {
    glm::mat4 mvpMat = myGLCamera->GetMVP() * modelMatrix;
    assimpLoader->Render3DModel(&mvpMat);
}

void AssimpYinModel::SetViewport(int width, int height) {
    MyLOGI("AssimpYinModel::SetViewport - 设置视口: width=%d, height=%d", width, height);
    screenHeight = height;
    screenWidth = width;
    glViewport(0, 0, width, height);
    CheckGLError("AssimpYinModel::SetViewport");

    myGLCamera->SetAspectRatio(static_cast<float>(width) / height);
}

void AssimpYinModel::DoubleTapAction() {
    MyLOGI("AssimpYinModel::DoubleTapAction - 双击操作重置模型位置");
    myGLCamera->SetModelPosition(modelDefaultPosition);
}

void AssimpYinModel::ScrollAction(float distanceX, float distanceY, float positionX, float positionY) {
    MyLOGI("AssimpYinModel::ScrollAction - 滚动操作: distanceX=%f, distanceY=%f, positionX=%f, positionY=%f",
           distanceX, distanceY, positionX, positionY);
    myGLCamera->RotateModel(distanceX, distanceY, positionX, positionY);
}

void AssimpYinModel::ScaleAction(float scaleFactor) {
    MyLOGI("AssimpYinModel::ScaleAction - 缩放操作: scaleFactor=%f", scaleFactor);
    myGLCamera->ScaleModel(scaleFactor);
}

void AssimpYinModel::MoveAction(float distanceX, float distanceY) {
    MyLOGI("AssimpYinModel::MoveAction - 移动操作: distanceX=%f, distanceY=%f", distanceX, distanceY);
    myGLCamera->TranslateModel(distanceX, distanceY);
}

void AssimpYinModel::UpdateRotation() {
    rotationAngle += 0.01f; // 调整旋转速度
    modelMatrix = glm::rotate(glm::mat4(1.0f), rotationAngle, glm::vec3(0.0f, 1.0f, 0.0f));
}
