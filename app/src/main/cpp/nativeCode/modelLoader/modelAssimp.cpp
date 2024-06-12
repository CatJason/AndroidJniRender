#include "modelAssimp.h"
#include "assimp/Importer.hpp"
#include "../utils/assetManager.h"
#include <opencv2/opencv.hpp>
#include <memory>

/**
 * 类构造函数
 */
ModelAssimp::ModelAssimp()
        : initsDone(false),
          myGLCamera(std::make_unique<GLCamera>()),
          assimpLoader(nullptr) {

    float pos[] = {0., 0., 0., 0.2, 0.5, 0.};
    std::copy(&pos[0], &pos[5], std::back_inserter(modelDefaultPosition));
    myGLCamera->SetModelPosition(modelDefaultPosition);
}

ModelAssimp::~ModelAssimp() = default;

/**
 * 执行初始化操作并将三角形的顶点/颜色加载到GLES中
 */
void ModelAssimp::PerformGLInits() {
    GLConfigInit();

    assimpLoader = std::make_unique<AssimpLoader>();

    const char* assetPaths[] = {
            "pinkfox/pinkFox.obj",
            "pinkfox/pinkFox.mtl",
            "pinkfox/body.jpg",
            "pinkfox/hair.jpg",
            "pinkfox/skin.jpg",
            "pinkfox/face.jpg"
    };

    std::vector<std::string> filenames(std::begin(assetPaths), std::end(assetPaths));

    for (auto& filename : filenames) {
        gHelperObject->ExtractAssetReturnFilename(filename.c_str(), filename);
    }

    // 加载3D模型
    assimpLoader->Load3DModel(filenames[0]);

    CheckGLError("ModelAssimp::PerformGLInits");
    initsDone = true;
}

/**
 * 渲染到显示屏
 */
void ModelAssimp::Render() {
    // 清除屏幕
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::mat4 mvpMat = myGLCamera->GetMVP();
    assimpLoader->Render3DModel(&mvpMat);

    CheckGLError("ModelAssimp::Render");
}

/**
 * 设置视口，该函数在用户改变设备方向时也会被调用。
 */
void ModelAssimp::SetViewport(int width, int height) {
    screenHeight = height;
    screenWidth = width;
    glViewport(0, 0, width, height);
    CheckGLError("Cube::SetViewport");

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
