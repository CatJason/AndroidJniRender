#include "modelAssimp.h"


#include "assimp/Importer.hpp"
#include "../utils/assetManager.h"
#include <opencv2/opencv.hpp>

/**
 * 类构造函数
 */
ModelAssimp::ModelAssimp() {
    initsDone = false;

    // 创建GLCamera对象，并设置对象的默认位置
    myGLCamera = new GLCamera();
    float pos[] = {0., 0., 0., 0.2, 0.5, 0.};
    std::copy(&pos[0], &pos[5], std::back_inserter(modelDefaultPosition));
    myGLCamera->SetModelPosition(modelDefaultPosition);

    assimpLoader = nullptr;
}

ModelAssimp::~ModelAssimp() {
    if (myGLCamera) {
        delete myGLCamera;
    }
    if (assimpLoader) {
        delete assimpLoader;
    }
}

/**
 * 执行初始化操作并将三角形的顶点/颜色加载到GLES中
 */
void ModelAssimp::PerformGLInits() {

    GLConfigInit();

    assimpLoader = new AssimpLoader();

    // extract the OBJ and companion files from assets
    std::string objFilename;
    bool isFilesPresent =
            gHelperObject->ExtractAssetReturnFilename("pinkfox/pinkFox.obj", objFilename);

    if (!isFilesPresent) {
        MyLOGE("Model %s does not exist!", objFilename.c_str());
        return;
    }
    std::string folderPath = "pinkfox/";
    std::vector<std::string> otherFilenames = {
            "pinkFox.mtl",
            "face.jpg",
            "body.jpg",
            "skin.jpg",
            "hair.jpg"
    };

    std::string mtlFilename;
    bool isMtlFilesPresent = gHelperObject->ExtractAssetReturnFilename("pinkfox/pinkFox.mtl", mtlFilename);
    if (!isMtlFilesPresent) {
        MyLOGE("Model %s does not exist!", mtlFilename.c_str());
        return;
    }

    std::string faceFilename;
    bool isFaceFilesPresent = gHelperObject->ExtractAssetReturnFilename("pinkfox/face.jpg", faceFilename);
    if (!isFaceFilesPresent) {
        MyLOGE("Model %s does not exist!", faceFilename.c_str());
        return;
    }

    std::string bodyFilename;
    bool isBodyFilesPresent = gHelperObject->ExtractAssetReturnFilename("pinkfox/body.jpg", bodyFilename);
    if (!isBodyFilesPresent) {
        MyLOGE("Model %s does not exist!", bodyFilename.c_str());
        return;
    }

    std::string skinFilename;
    bool isSkinFilesPresent = gHelperObject->ExtractAssetReturnFilename("pinkfox/skin.jpg", skinFilename);
    if (!isSkinFilesPresent) {
        MyLOGE("Model %s does not exist!", skinFilename.c_str());
        return;
    }

    std::string hairFilename;
    bool isHairFilesPresent = gHelperObject->ExtractAssetReturnFilename("pinkfox/hair.jpg", hairFilename);
    if (!isHairFilesPresent) {
        MyLOGE("Model %s does not exist!", hairFilename.c_str());
        return;
    }

    assimpLoader->Load3DModel(objFilename);

    CheckGLError("ModelAssimp::PerformGLInits");
    initsDone = true;
}


/**
 * 渲染到显示屏
 */
void ModelAssimp::Render() {
    // clear the screen
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

    myGLCamera->SetAspectRatio((float) width / height);
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