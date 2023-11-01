cmake_minimum_required(VERSION 3.4.1)

add_library(
        native-lib
        SHARED
        ../cpp/jniCalls/assimpActivity.cpp
        ../cpp/jniCalls/gestureClass.cpp
        ../cpp/jniCalls/myGLRenderer.cpp
        ../cpp/nativeCode/modelLoader/assimpLoader.cpp
        ../cpp/nativeCode/modelLoader/assimpLoader.h
        ../cpp/nativeCode/utils/fileUtils.cpp
        ../cpp/nativeCode/utils/fileUtils.h
        ../cpp/nativeCode/camera/GLCamera.cpp
        ../cpp/nativeCode/camera/GLCamera.h
        ../cpp/nativeCode/gl/GLManager.cpp
        ../cpp/nativeCode/gl/GLManager.h
        ../cpp/nativeCode/gl/myGLM.h
        ../cpp/nativeCode/utils/assetManager.cpp
        ../cpp/nativeCode/utils/assetManager.h
        ../cpp/nativeCode/utils/logUtils.h
        ../cpp/nativeCode/shader/shaderLoader.cpp
        ../cpp/nativeCode/shader/shaderLoader.h
        ../cpp/nativeCode/modelLoader/modelAssimp.cpp
        ../cpp/nativeCode/modelLoader/modelAssimp.h
)

find_library(log-lib log)

aux_source_directory(${CMAKE_SOURCE_DIR}/glm/ GLM)
aux_source_directory(${GLM}/gtc GTC)
aux_source_directory(${GLM}/gtx GTX)
aux_source_directory(${GLM}/detail DETAIL)

target_include_directories(native-lib PRIVATE ${GLM} ${GTC} ${DETAIL} ${GTX})

add_subdirectory(${CMAKE_SOURCE_DIR}/Assimp)
include_directories(${CMAKE_SOURCE_DIR}/Assimp/include ${CMAKE_SOURCE_DIR}/opencv/include)
set(ATLAS_ENGINE_LIBS assimp)
set(ATLAS_ENGINE_SYSTEM_LIBS android EGL GLESv2 GLESv3 m)

set(distribution_DIR ../../../../libs)
add_library(opencv_java3 SHARED IMPORTED)
set_target_properties(
        opencv_java3 PROPERTIES IMPORTED_LOCATION
        ${distribution_DIR}/${ANDROID_ABI}/libopencv_java3.so
)
target_link_libraries(
        native-lib
        opencv_java3
        ${ATLAS_ENGINE_LIBS}
        ${ATLAS_ENGINE_SYSTEM_LIBS}
        ${log-lib}
)