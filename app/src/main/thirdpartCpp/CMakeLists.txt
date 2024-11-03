cmake_minimum_required(VERSION 3.4.1)

# Define variables for common paths
set(JNI_CALLS_DIR ${CMAKE_SOURCE_DIR}/../cpp/jniCalls)
set(NATIVE_CODE_DIR ${CMAKE_SOURCE_DIR}/../cpp/nativeCode)
set(MODEL_LOADER_DIR ${NATIVE_CODE_DIR}/modelLoader)
set(UTILS_DIR ${NATIVE_CODE_DIR}/utils)
set(CAMERA_DIR ${NATIVE_CODE_DIR}/camera)
set(GL_DIR ${NATIVE_CODE_DIR}/gl)
set(SHADER_DIR ${NATIVE_CODE_DIR}/shader)
set(GLM_DIR ${CMAKE_SOURCE_DIR}/glm)

# Add the native library
add_library(
        native-lib
        SHARED
        ${JNI_CALLS_DIR}/assimpActivity.cpp
        ${JNI_CALLS_DIR}/gestureClass.cpp
        ${JNI_CALLS_DIR}/myGLRenderer.cpp
        ${MODEL_LOADER_DIR}/assimpLoader.cpp
        ${UTILS_DIR}/fileUtils.cpp
        ${CAMERA_DIR}/GLCamera.cpp
        ${GL_DIR}/GLManager.cpp
        ${UTILS_DIR}/assetManager.cpp
        ${SHADER_DIR}/shaderLoader.cpp
        ${MODEL_LOADER_DIR}/AssimpPinkFoxModel.cpp
        ${MODEL_LOADER_DIR}/AssimpYinModel.cpp
)

# Include directories
target_include_directories(native-lib PRIVATE
        ${NATIVE_CODE_DIR}
        ${NATIVE_CODE_DIR}/modelLoader
        ${NATIVE_CODE_DIR}/utils
        ${NATIVE_CODE_DIR}/camera
        ${NATIVE_CODE_DIR}/gl
        ${NATIVE_CODE_DIR}/shader
        ${GLM_DIR}
        ${GLM_DIR}/gtc
        ${GLM_DIR}/gtx
        ${GLM_DIR}/detail
)

# Find libraries
find_library(log-lib log)
find_library(android-lib android)

# Add Assimp directories
add_subdirectory(${CMAKE_SOURCE_DIR}/Assimp)
include_directories(${CMAKE_SOURCE_DIR}/Assimp/include)

# Set libraries
set(ATLAS_ENGINE_LIBS assimp)
set(ATLAS_ENGINE_SYSTEM_LIBS android EGL GLESv2 GLESv3 m)

# Link libraries
target_link_libraries(
        native-lib
        jnigraphics
        ${ATLAS_ENGINE_LIBS}
        ${ATLAS_ENGINE_SYSTEM_LIBS}
        ${log-lib}
)