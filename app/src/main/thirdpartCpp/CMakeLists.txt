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

#cmake_minimum_required(VERSION 3.4.1)
#这条指令设定了CMake的最低版本要求为3.4.1。这是因为脚本可能使用了一些在这个或更高版本中才有的特性。
#
#add_library
#这个命令定义了一个新的库（native-lib），指定它是一个共享库（SHARED），并列出了构成该库的源文件。这些源文件包括.cpp源代码文件和.h头文件，分布在不同的目录下。
#我们在 native-lib
#
#find_library
#find_library用于查找并设置一个库的路径，这里用它来查找日志库（log-lib log），在Android系统中用于提供日志功能的库。
#
#aux_source_directory
#这个命令将指定目录下的所有源文件赋值给一个变量（在这里是GLM, GTC, GTX, DETAIL等）。这些目录包含了GLM库的源码，它是一个流行的OpenGL数学库。
#
#target_include_directories
#此指令为目标库（native-lib）添加了包含目录，使得编译器可以找到这些目录下的头文件。
#
#add_subdirectory
#add_subdirectory将指定的目录添加到构建中，这通常是为了构建该目录下的CMake项目。这里它被用来添加Assimp库的源码目录。
#
#include_directories
#这个命令添加了编译器搜索头文件的路径。这里指定的是Assimp和OpenCV的include目录，这样编译器就可以找到这两个库的头文件。
#
#set
#set命令用于设置变量的值。这里它被用来设置库（ATLAS_ENGINE_LIBS，ATLAS_ENGINE_SYSTEM_LIBS）和OpenCV的IMPORTED_LOCATION，即导入库的位置。
#
#add_library (IMPORTED)
#这里又用add_library定义了一个导入库（opencv_java3），表示该库是预先编译好的，通过设置IMPORTED_LOCATION来指定库文件的位置。
#
#target_link_libraries
#最后，target_link_libraries命令将上述定义的库（OpenCV，Assimp，系统库等）与目标库（native-lib）链接起来。这确保了在构建native-lib时，所有相关依赖都会被正确地链接。