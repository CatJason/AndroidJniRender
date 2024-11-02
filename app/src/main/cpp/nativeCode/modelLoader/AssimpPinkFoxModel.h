#ifndef MODELASSIMP_H
#define MODELASSIMP_H

#include "../utils/logUtils.h"
#include "../gl/GLManager.h"
#include "../camera/GLCamera.h"
#include "AssimpLoader.h"
#include <sstream>
#include <iostream>
#include <stdio.h>
#include <string>
#include <memory>
#include <vector>

class AssimpPinkFoxModel {
public:
    AssimpPinkFoxModel();
    ~AssimpPinkFoxModel();

    void PerformGLInit(JNIEnv *env);
    void Render();
    void SetViewport(int width, int height);
    void DoubleTapAction();
    void ScrollAction(float distanceX, float distanceY, float positionX, float positionY);
    void ScaleAction(float scaleFactor);
    void MoveAction(float distanceX, float distanceY);

    int GetScreenWidth() const { return screenWidth; }
    int GetScreenHeight() const { return screenHeight; }
    void UpdateRotation();

private:
    bool initsDone;
    int screenWidth, screenHeight;

    std::vector<float> modelDefaultPosition;
    std::unique_ptr<GLCamera> myGLCamera;
    std::unique_ptr<AssimpLoader> assimpLoader;
    float rotationAngle; // 声明 rotationAngle;
    glm::mat4 modelMatrix;

    void ClearScreen();

    void RenderModel();

    std::vector<std::string> LoadAssetPaths();

    void InitializeModelDefaultPosition();

};

#endif // MODELASSIMP_H
