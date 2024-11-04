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

/**
 * AssimpPinkFoxModel 类负责加载和渲染3D模型。
 */
class AssimpPinkFoxModel {
public:
    AssimpPinkFoxModel(); // 构造函数
    ~AssimpPinkFoxModel(); // 析构函数

    /**
     * 执行OpenGL初始化。
     * @param env JNIEnv 指针
     */
    void PerformGLInit(JNIEnv *env);

    /**
     * 渲染模型。
     */
    void Render();

    /**
     * 设置视口大小。
     * @param width 视口宽度
     * @param height 视口高度
     */
    void SetViewport(int width, int height);

    /**
     * 处理双击事件。
     */
    void DoubleTapAction();

    /**
     * 处理滚动事件。
     * @param distanceX X轴滚动距离
     * @param distanceY Y轴滚动距离
     * @param positionX 当前X位置
     * @param positionY 当前Y位置
     */
    void ScrollAction(float distanceX, float distanceY, float positionX, float positionY);

    /**
     * 处理缩放事件。
     * @param scaleFactor 缩放因子
     */
    void ScaleAction(float scaleFactor);

    /**
     * 处理移动事件。
     * @param distanceX X轴移动距离
     * @param distanceY Y轴移动距离
     */
    void MoveAction(float distanceX, float distanceY);

    /**
     * 获取屏幕宽度。
     * @return 当前屏幕宽度
     */
    int GetScreenWidth() const { return screenWidth; }

    /**
     * 获取屏幕高度。
     * @return 当前屏幕高度
     */
    int GetScreenHeight() const { return screenHeight; }

    /**
     * 更新模型旋转。
     */
    void UpdateRotation();

private:
    bool initsDone; // 初始化状态标志
    int screenWidth, screenHeight; // 屏幕宽度和高度

    std::vector<float> modelDefaultPosition; // 模型默认位置
    std::unique_ptr<GLCamera> myGLCamera; // 相机实例
    std::unique_ptr<AssimpLoader> assimpLoader; // Assimp加载器实例
    float rotationAngle; // 旋转角度
    glm::mat4 modelMatrix; // 模型矩阵

    /**
     * 清除屏幕。
     */
    void ClearScreen();

    /**
     * 渲染3D模型。
     */
    void RenderModel();

    /**
     * 加载资产路径。
     * @return 资产路径列表
     */
    std::vector<std::string> LoadAssetPaths();

    /**
     * 初始化模型默认位置。
     */
    void InitializeModelDefaultPosition();
};

#endif // MODELASSIMP_H