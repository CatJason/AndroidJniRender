#ifndef GLCAMERA_H
#define GLCAMERA_H

#include <vector>
#include "../utils/fileUtils.h"

// 将手势转换为模型移动的敏感系数
#define SCALE_TO_Z_TRANSLATION  20
#define TRANSLATION_TO_ANGLE    5
#define XY_TRANSLATION_FACTOR   10

class GLCamera {
public:
    /**
     * @param FOV 视野角度，以度为单位
     * @param zPosition 相机在Z轴上的位置
     * @param nearPlaneDistance 近裁剪面距离
     * @param farPlaneDistance 远裁剪面距离
     * 构造函数，初始化GLCamera类的成员变量
     */
    GLCamera(
            float FOV = 45,          // 视野角度，以度为单位
            float zPosition = 100,    // 相机在Z轴上的位置
            float nearPlaneDistance = 1.0f, // 近裁剪面距离
            float farPlaneDistance = 2000.0f // 远裁剪面距离
    );

    /**
     * @param modelPosition 模型的位置和旋转，包含6个浮点数值
     * 设置模型的位置和旋转
     */
    void SetModelPosition(std::vector<float> modelPosition);

    /**
     * @param aspect 显示器的纵横比
     * 设置显示器的纵横比以计算投影矩阵
     */
    void SetAspectRatio(float aspect);

    /**
     * 获取MVP矩阵
     * @return MVP矩阵
     */
    glm::mat4 GetMVP() { return mvpMat; }

    /**
     * @param distanceX 拖动的X距离
     * @param distanceY 拖动的Y距离
     * @param endPositionX 拖动结束时的X位置
     * @param endPositionY 拖动结束时的Y位置
     * 模拟模型旋转的变化
     */
    void RotateModel(float distanceX, float distanceY, float endPositionX, float endPositionY);

    /**
     * @param scaleFactor 缩放因子
     * 缩放模型
     */
    void ScaleModel(float scaleFactor);

    /**
     * @param distanceX 平移的X距离
     * @param distanceY 平移的Y距离
     * 平移模型
     */
    void TranslateModel(float distanceX, float distanceY);

private:
    /**
     * 计算MVP矩阵
     */
    void ComputeMVPMatrix();

    /**
     * @param x 模型在X轴上的位置
     * @param y 模型在Y轴上的位置
     * @param z 模型在Z轴上的位置
     * 更新模型的位置
     */
    void UpdatePosition(float x, float y, float z);

    /**
     * @param pitch 模型的俯仰角
     * @param yaw 模型的偏航角
     * @param roll 模型的滚转角
     * 更新模型的旋转
     */
    void UpdateRotation(float pitch, float yaw, float roll);

    /**
     * @param endPositionX 拖动结束时的X位置
     * @param endPositionY 拖动结束时的Y位置
     * 计算拖动结束位置的归一化向量
     * @return 归一化后的结束向量
     */
    glm::vec3 CalculateEndVector(float endPositionX, float endPositionY);

    /**
     * @param distanceX 拖动的X距离
     * @param distanceY 拖动的Y距离
     * @param endPositionX 拖动结束时的X位置（引用）
     * @param endPositionY 拖动结束时的Y位置（引用）
     * 计算拖动开始位置的归一化向量
     * @return 归一化后的开始向量
     */
    glm::vec3 CalculateBeginVector(float distanceX, float distanceY, float& endPositionX, float& endPositionY);

    float FOV;
    float nearPlaneDistance, farPlaneDistance;

    glm::mat4 projectionViewMat;
    glm::mat4 rotateMat, translateMat;
    glm::mat4 modelMat;
    glm::mat4 viewMat;
    glm::mat4 mvpMat; // ModelViewProjection: 通过投影、视图和模型矩阵相乘获得

    glm::quat modelQuaternion;
    float deltaX, deltaY, deltaZ;
};

/**
 * @param endPositionX 拖动结束时的X位置
 * @param endPositionY 拖动结束时的Y位置
 * 计算拖动结束位置在虚拟球表面上的Z坐标
 * @return 计算后的Z坐标
 */
float CalculatePositionZ(float endPositionX, float endPositionY);

/**
 * @param x 向量的X分量
 * @param y 向量的Y分量
 * @param z 向量的Z分量
 * 创建归一化向量
 * @return 归一化后的向量
 */
glm::vec3 CreateNormalizedVector(float x, float y, float z);

/**
 * @param beginVec 拖动开始的向量
 * @param endVec 拖动结束的向量
 * 计算旋转轴
 * @return 计算后的旋转轴
 */
glm::vec3 CalculateRotationAxis(const glm::vec3& beginVec, const glm::vec3& endVec);

/**
 * @param beginVec 拖动开始的向量
 * @param endVec 拖动结束的向量
 * 计算旋转角度
 * @return 计算后的旋转角度
 */
float CalculateRotationAngle(const glm::vec3& beginVec, const glm::vec3& endVec);

/**
 * @param angle 旋转角度
 * @param axis 旋转轴
 * 创建四元数
 * @return 创建的四元数
 */
glm::quat CreateQuaternion(float angle, const glm::vec3& axis);

#endif //GLCAMERA_H
