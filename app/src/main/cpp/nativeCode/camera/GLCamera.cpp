#include "GLCamera.h"
#include "../utils/logUtils.h"
#include <cmath>

/**
 * @param FOV 相机的视野角度
 * @param zPosition 相机在Z轴上的位置
 * @param nearPlaneDistance 近裁剪面距离
 * @param farPlaneDistance 远裁剪面距离
 * 该构造函数初始化了GLCamera类的成员变量。
 * 它设置了相机的位置和视图矩阵，以及其他与模型位置和变换相关的变量。
 * 最后，它将MVP矩阵初始化为单位矩阵，因为投影矩阵此时未知。
 */
GLCamera::GLCamera(float FOV, float zPosition, float nearPlaneDistance, float farPlaneDistance) {
    MyLOGF("GLCamera::GLCamera");
    MyLOGF("GLCamera::GLCamera - 初始化相机");
    glm::vec3 cameraPosition = glm::vec3(0, 0, zPosition);

    // 设置视图矩阵
    viewMat = glm::lookAt(cameraPosition, glm::vec3(0, 0, -1), glm::vec3(0, 1, 0));

    this->nearPlaneDistance = nearPlaneDistance;
    this->farPlaneDistance = farPlaneDistance;
    this->FOV = FOV;

    // 初始化位置增量和模型四元数
    deltaX = deltaY = deltaZ = 0;
    modelQuaternion = glm::quat(glm::vec3(0, 0, 0));

    // 初始化矩阵
    modelMat = glm::mat4(1.0f);
    translateMat = glm::mat4(1.0f);
    rotateMat = glm::mat4(1.0f);
    mvpMat = glm::mat4(1.0f);
}

/**
 * @param aspect 显示器的纵横比
 * 使用显示器的纵横比计算投影矩阵
 */
void GLCamera::SetAspectRatio(float aspect) {
    MyLOGF("GLCamera::SetAspectRatio");
    MyLOGF("GLCamera::SetAspectRatio - 设置纵横比: %f", aspect);
    glm::mat4 projectionMat;
    projectionMat = glm::perspective(FOV * float(M_PI / 180), aspect, nearPlaneDistance, farPlaneDistance);
    projectionViewMat = projectionMat * viewMat;
    ComputeMVPMatrix();
}

/**
 * @param modelPosition 模型的位置和旋转，包含6个浮点数值
 * 设置模型的位置，有6个自由度：3个用于x-y-z位置，3个用于alpha-beta-gamma欧拉角
 * 将欧拉角转换为四元数并更新MVP矩阵
 */
void GLCamera::SetModelPosition(std::vector<float> modelPosition) {
    MyLOGF("GLCamera::SetModelPosition");
    MyLOGF("GLCamera::SetModelPosition - 设置模型位置: x=%f, y=%f, z=%f, pitch=%f, yaw=%f, roll=%f",
           modelPosition[0], modelPosition[1], modelPosition[2], modelPosition[3], modelPosition[4], modelPosition[5]);
    UpdatePosition(modelPosition[0], modelPosition[1], modelPosition[2]);
    UpdateRotation(modelPosition[3], modelPosition[4], modelPosition[5]);
    ComputeMVPMatrix();
}

/**
 * @param x 模型在X轴上的位置
 * @param y 模型在Y轴上的位置
 * @param z 模型在Z轴上的位置
 * 更新模型的位置
 */
void GLCamera::UpdatePosition(float x, float y, float z) {
    MyLOGF("GLCamera::UpdatePosition - 更新位置: x=%f, y=%f, z=%f", x, y, z);
    deltaX = x;
    deltaY = y;
    deltaZ = z;
}

/**
 * @param pitch 模型的俯仰角
 * @param yaw 模型的偏航角
 * @param roll 模型的滚转角
 * 更新模型的旋转
 */
void GLCamera::UpdateRotation(float pitch, float yaw, float roll) {
    MyLOGF("GLCamera::UpdateRotation - 更新旋转: pitch=%f, yaw=%f, roll=%f", pitch, yaw, roll);
    modelQuaternion = glm::quat(glm::vec3(pitch, yaw, roll));
    rotateMat = glm::toMat4(modelQuaternion);
}

/**
 * 计算MVP矩阵
 * 根据x-y-z位置计算平移矩阵，根据描述旋转的四元数计算旋转矩阵
 * MVP = Projection * View * (Translation * Rotation)
 */
void GLCamera::ComputeMVPMatrix() {
    MyLOGF("GLCamera::ComputeMVPMatrix - 计算MVP矩阵");
    translateMat = glm::mat4(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, deltaX, deltaY, deltaZ, 1);

    modelMat = translateMat * rotateMat;

    mvpMat = projectionViewMat * modelMat;
}

/**
 * @param scaleFactor 缩放因子
 * 通过沿Z轴推拉模型来模拟缩放的变化
 */
void GLCamera::ScaleModel(float scaleFactor) {
    MyLOGF("GLCamera::ScaleModel - 缩放模型: scaleFactor=%f", scaleFactor);
    deltaZ += SCALE_TO_Z_TRANSLATION * (scaleFactor - 1);
    ComputeMVPMatrix();
}

/**
 * @param distanceX 拖动的X距离
 * @param distanceY 拖动的Y距离
 * @param endPositionX 拖动结束时的X位置
 * @param endPositionY 拖动结束时的Y位置
 * 模拟模型旋转的变化，拖动操作转换为模型旋转，计算旋转轴和角度并更新旋转矩阵
 */
void GLCamera::RotateModel(float distanceX, float distanceY, float endPositionX, float endPositionY) {
    MyLOGF("GLCamera::RotateModel - 旋转模型: distanceX=%f, distanceY=%f, endPositionX=%f, endPositionY=%f",
           distanceX, distanceY, endPositionX, endPositionY);
    glm::vec3 endVec = CalculateEndVector(endPositionX, endPositionY);
    glm::vec3 beginVec = CalculateBeginVector(distanceX, distanceY, endPositionX, endPositionY);

    glm::vec3 rotationAxis = CalculateRotationAxis(beginVec, endVec);
    float rotationAngle = CalculateRotationAngle(beginVec, endVec);

    modelQuaternion = CreateQuaternion(rotationAngle, rotationAxis);
    rotateMat = glm::toMat4(modelQuaternion) * rotateMat;

    ComputeMVPMatrix();
}

/**
 * @param endPositionX 拖动结束时的X位置
 * @param endPositionY 拖动结束时的Y位置
 * 计算拖动结束位置的归一化向量
 * @return 归一化后的结束向量
 */
glm::vec3 GLCamera::CalculateEndVector(float endPositionX, float endPositionY) {
    MyLOGF("GLCamera::CalculateEndVector - 计算结束向量: endPositionX=%f, endPositionY=%f", endPositionX, endPositionY);
    float positionZ = CalculatePositionZ(endPositionX, endPositionY);
    return CreateNormalizedVector(endPositionX, endPositionY, positionZ);
}

/**
 * @param distanceX 拖动的X距离
 * @param distanceY 拖动的Y距离
 * @param endPositionX 拖动结束时的X位置（引用）
 * @param endPositionY 拖动结束时的Y位置（引用）
 * 计算拖动开始位置的归一化向量
 * @return 归一化后的开始向量
 */
glm::vec3 GLCamera::CalculateBeginVector(float distanceX, float distanceY, float& endPositionX, float& endPositionY) {
    MyLOGF("GLCamera::CalculateBeginVector - 计算开始向量: distanceX=%f, distanceY=%f, endPositionX=%f, endPositionY=%f",
           distanceX, distanceY, endPositionX, endPositionY);
    endPositionX += distanceX;
    endPositionY += distanceY;
    float positionZ = CalculatePositionZ(endPositionX, endPositionY);
    return CreateNormalizedVector(endPositionX, endPositionY, positionZ);
}

/**
 * @param distanceX 平移的X距离
 * @param distanceY 平移的Y距离
 * 通过改变模型的x-y坐标来移动模型
 */
void GLCamera::TranslateModel(float distanceX, float distanceY) {
    MyLOGF("GLCamera::TranslateModel - 平移模型: distanceX=%f, distanceY=%f", distanceX, distanceY);
    deltaX += XY_TRANSLATION_FACTOR * distanceX;
    deltaY += XY_TRANSLATION_FACTOR * distanceY;
    ComputeMVPMatrix();
}

/**
 * @param endPositionX 拖动结束时的X位置
 * @param endPositionY 拖动结束时的Y位置
 * 计算拖动结束位置在虚拟球表面上的Z坐标
 * @return 计算后的Z坐标
 */
float CalculatePositionZ(float endPositionX, float endPositionY) {
    MyLOGF("CalculatePositionZ - 计算位置Z: endPositionX=%f, endPositionY=%f", endPositionX, endPositionY);
    float dist = sqrt(fmin(1, endPositionX * endPositionX + endPositionY * endPositionY));
    return sqrt(1 - dist * dist);
}

/**
 * @param x 向量的X分量
 * @param y 向量的Y分量
 * @param z 向量的Z分量
 * 创建归一化向量
 * @return 归一化后的向量
 */
glm::vec3 CreateNormalizedVector(float x, float y, float z) {
    MyLOGF("CreateNormalizedVector - 创建归一化向量: x=%f, y=%f, z=%f", x, y, z);
    glm::vec3 vec = glm::vec3(x, y, z);
    return glm::normalize(vec);
}

/**
 * @param beginVec 拖动开始的向量
 * @param endVec 拖动结束的向量
 * 计算旋转轴
 * @return 计算后的旋转轴
 */
glm::vec3 CalculateRotationAxis(const glm::vec3& beginVec, const glm::vec3& endVec) {
    MyLOGF("CalculateRotationAxis - 计算旋转轴");
    return glm::normalize(glm::cross(beginVec, endVec));
}

/**
 * @param beginVec 拖动开始的向量
 * @param endVec 拖动结束的向量
 * 计算旋转角度
 * @return 计算后的旋转角度
 */
float CalculateRotationAngle(const glm::vec3& beginVec, const glm::vec3& endVec) {
    MyLOGF("CalculateRotationAngle - 计算旋转角度");
    float dotProduct = fmax(fmin(glm::dot(beginVec, endVec), 1.), -1.);
    return TRANSLATION_TO_ANGLE * acos(dotProduct);
}

/**
 * @param angle 旋转角度
 * @param axis 旋转轴
 * 创建四元数
 * @return 创建的四元数
 */
glm::quat CreateQuaternion(float angle, const glm::vec3& axis) {
    MyLOGF("CreateQuaternion - 创建四元数: angle=%f", angle);
    return glm::angleAxis(angle, axis);
}