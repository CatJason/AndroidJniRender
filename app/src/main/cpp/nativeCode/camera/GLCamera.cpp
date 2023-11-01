#include "GLCamera.h"
#include "../utils/logUtils.h"
#include <cmath>

/**
 *
 * @param FOV
 * @param zPosition
 * @param nearPlaneDistance
 * @param farPlaneDistance
 * 该构造函数初始化了GLCamera类的成员变量。
 * 它设置了相机的位置和视图矩阵，以及其他与模型位置和变换相关的变量。
 * 最后，它将MVP矩阵初始化为单位矩阵，因为投影矩阵此时未知。
 */

GLCamera::GLCamera(
        float FOV,
        float zPosition,
        float nearPlaneDistance,
        float farPlaneDistance
) {
    MyLOGF("GLCamera::GLCamera");
    // 设置相机位置为固定值
    glm::vec3 cameraPosition = glm::vec3(0, 0, zPosition);

    // 构建视图矩阵，定义相机的位置、朝向和上向量
    viewMat = glm::lookAt(
            cameraPosition,        // 相机在世界空间中的位置
            glm::vec3(0, 0, -1),   // 相机指向的方向
            glm::vec3(0, 1, 0)
    );   // 相机的上方向

    this->nearPlaneDistance = nearPlaneDistance;
    this->farPlaneDistance = farPlaneDistance;
    this->FOV = FOV;

    // 6DOF（6 Degrees of Freedom）描述模型的位置
    deltaX = deltaY = deltaZ = 0;                  // 平移量
    modelQuaternion = glm::quat(glm::vec3(0, 0, 0)); // 旋转四元数

    modelMat = glm::mat4(1.0f);
    translateMat = glm::mat4(1.0f);
    rotateMat = glm::mat4(1.0f);
    mvpMat = glm::mat4(1.0f); // 投影矩阵未知 -> 将MVP初始化为单位矩阵
}


/**
 * @param aspect
 * 使用显示器的纵横比计算投影矩阵
 */
void GLCamera::SetAspectRatio(float aspect) {
    MyLOGF("GLCamera::SetAspectRatio");
    glm::mat4 projectionMat;
    projectionMat = glm::perspective(
            FOV * float(M_PI / 180), // 相机的视野角度
            aspect,                  // 显示器的纵横比
            nearPlaneDistance,       // 近裁剪面距离
            farPlaneDistance    // 远裁剪面距离
    );
    projectionViewMat = projectionMat * viewMat;
    ComputeMVPMatrix();
}

/**
 * 模型的位置有6个自由度：3个用于x-y-z位置，3个用于alpha-beta-gamma欧拉角
 * 将欧拉角转换为四元数并更新MVP矩阵
 */
void GLCamera::SetModelPosition(std::vector<float> modelPosition) {
    MyLOGF("GLCamera::SetModelPosition");
// 更新模型的位置
    deltaX = modelPosition[0];
    deltaY = modelPosition[1];
    deltaZ = modelPosition[2];
    float pitchAngle = modelPosition[3];
    float yawAngle = modelPosition[4];
    float rollAngle = modelPosition[5];

// 将欧拉角转换为四元数
    modelQuaternion = glm::quat(glm::vec3(pitchAngle, yawAngle, rollAngle));
// 将四元数转换为旋转矩阵
    rotateMat = glm::toMat4(modelQuaternion);

// 更新MVP矩阵
    ComputeMVPMatrix();
}

/**
 * 根据x-y-z位置计算平移矩阵，根据描述旋转的四元数计算旋转矩阵
 * MVP = Projection * View * (Translation * Rotation)
 */
void GLCamera::ComputeMVPMatrix() {
    MyLOGF("myGLCamera GLCamera::ComputeMVPMatrix");
    // 计算平移矩阵
    translateMat = glm::mat4(
            1, 0, 0, 0,                  // 列0
            0, 1, 0, 0,                  // 列1
            0, 0, 1, 0,                  // 列2
            deltaX, deltaY, deltaZ, 1
    );  // 列3

    // 计算模型矩阵
    modelMat = translateMat * rotateMat;

    // 计算MVP矩阵
    mvpMat = projectionViewMat * modelMat;
}

/**
 * 通过沿Z轴推拉模型来模拟缩放的变化
 */
void GLCamera::ScaleModel(float scaleFactor) {
    MyLOGF("myGLCamera GLCamera::ScaleModel");
    // 根据缩放因子计算沿Z轴的平移量，以模拟缩放
    deltaZ += SCALE_TO_Z_TRANSLATION * (scaleFactor - 1);

    // 重新计算MVP矩阵
    ComputeMVPMatrix();
}

/**
 * Finger drag movements are converted to rotation of model by deriving a
 * quaternion from the drag movement
 */
float CalculatePositionZ(float endPositionX, float endPositionY) {
    float dist = sqrt(fmin(1, endPositionX * endPositionX + endPositionY * endPositionY));
    return sqrt(1 - dist * dist);
}

glm::vec3 CreateNormalizedVector(float x, float y, float z) {
    glm::vec3 vec = glm::vec3(x, y, z);
    return glm::normalize(vec);
}

glm::vec3 CalculateRotationAxis(const glm::vec3& beginVec, const glm::vec3& endVec) {
    return glm::normalize(glm::cross(beginVec, endVec));
}

float CalculateRotationAngle(const glm::vec3& beginVec, const glm::vec3& endVec) {
    float dotProduct = fmax(fmin(glm::dot(beginVec, endVec), 1.), -1.);
    return TRANSLATION_TO_ANGLE * acos(dotProduct);
}

glm::quat CreateQuaternion(float angle, const glm::vec3& axis) {
    return glm::angleAxis(angle, axis);
}

void GLCamera::RotateModel(float distanceX, float distanceY, float endPositionX, float endPositionY) {
    // 计算拖动结束位置在虚拟球表面上的Z坐标
    float positionZ = CalculatePositionZ(endPositionX, endPositionY);
    // 创建结束向量，包括X、Y和Z坐标，并归一化
    glm::vec3 endVec = CreateNormalizedVector(endPositionX, endPositionY, positionZ);

    // 根据拖动距离更新拖动起始位置
    endPositionX += distanceX;
    endPositionY += distanceY;
    // 计算拖动起始位置在虚拟球表面上的Z坐标
    positionZ = CalculatePositionZ(endPositionX, endPositionY);
    // 创建起始向量，包括X、Y和Z坐标，并归一化
    glm::vec3 beginVec = CreateNormalizedVector(endPositionX, endPositionY, positionZ);

    // 计算旋转轴
    glm::vec3 rotationAxis = CalculateRotationAxis(beginVec, endVec);
    // 计算旋转角度
    float rotationAngle = CalculateRotationAngle(beginVec, endVec);

    // 创建旋转四元数
    modelQuaternion = CreateQuaternion(rotationAngle, rotationAxis);
    // 将四元数转换为旋转矩阵，并与当前的旋转矩阵相乘
    rotateMat = glm::toMat4(modelQuaternion) * rotateMat;

    // 更新MVP矩阵
    ComputeMVPMatrix();
}

/**
 * displace model by changing x-y coordinates
 */
void GLCamera::TranslateModel(float distanceX, float distanceY) {
    deltaX += XY_TRANSLATION_FACTOR * distanceX;
    deltaY += XY_TRANSLATION_FACTOR * distanceY;
    ComputeMVPMatrix();
}