#ifndef MODELASSIMP_H
#define MODELASSIMP_H

#include "../utils/logUtils.h"
#include "../gl/GLManager.h"
#include "../camera/GLCamera.h"
#include "assimpLoader.h"
#include <sstream>
#include <iostream>
#include <stdio.h>
#include <string>
#include <memory>
#include <vector>

// 自定义 make_unique 实现
namespace std {
    template<typename T, typename... Args>
    std::unique_ptr<T> make_unique(Args&&... args) {
        return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
    }
}

class ModelAssimp {
public:
    ModelAssimp();
    ~ModelAssimp();

    void PerformGLInits();
    void Render();
    void SetViewport(int width, int height);
    void DoubleTapAction();
    void ScrollAction(float distanceX, float distanceY, float positionX, float positionY);
    void ScaleAction(float scaleFactor);
    void MoveAction(float distanceX, float distanceY);

    int GetScreenWidth() const { return screenWidth; }
    int GetScreenHeight() const { return screenHeight; }

private:
    bool initsDone;
    int screenWidth, screenHeight;

    std::vector<float> modelDefaultPosition;
    std::unique_ptr<GLCamera> myGLCamera;
    std::unique_ptr<AssimpLoader> assimpLoader;

    void ClearScreen();

    void RenderModel();

    std::vector<std::string> LoadAssetPaths();

    void InitializeModelDefaultPosition();
};

#endif // MODELASSIMP_H
