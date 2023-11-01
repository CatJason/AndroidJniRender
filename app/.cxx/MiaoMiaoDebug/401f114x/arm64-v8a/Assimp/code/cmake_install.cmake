# Install script for directory: /Users/jason/AndroidStudioProjects/AndroidJniRender/app/src/main/thirdpartCpp/Assimp/code

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/usr/local")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "MiaoMiaoDebug")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Install shared libraries without execute permission?
if(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)
  set(CMAKE_INSTALL_SO_NO_EXE "0")
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "TRUE")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE SHARED_LIBRARY FILES "/Users/jason/AndroidStudioProjects/AndroidJniRender/app/build/intermediates/cxx/MiaoMiaoDebug/401f114x/obj/arm64-v8a/libassimp.so")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libassimp.so" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libassimp.so")
    if(CMAKE_INSTALL_DO_STRIP)
      execute_process(COMMAND "/Users/jason/Library/Android/sdk/ndk/21.4.7075529/toolchains/llvm/prebuilt/darwin-x86_64/bin/aarch64-linux-android-strip" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libassimp.so")
    endif()
  endif()
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xassimp-devx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/assimp" TYPE FILE FILES
    "/Users/jason/AndroidStudioProjects/AndroidJniRender/app/src/main/thirdpartCpp/Assimp/code/../include/assimp/anim.h"
    "/Users/jason/AndroidStudioProjects/AndroidJniRender/app/src/main/thirdpartCpp/Assimp/code/../include/assimp/ai_assert.h"
    "/Users/jason/AndroidStudioProjects/AndroidJniRender/app/src/main/thirdpartCpp/Assimp/code/../include/assimp/camera.h"
    "/Users/jason/AndroidStudioProjects/AndroidJniRender/app/src/main/thirdpartCpp/Assimp/code/../include/assimp/color4.h"
    "/Users/jason/AndroidStudioProjects/AndroidJniRender/app/src/main/thirdpartCpp/Assimp/code/../include/assimp/color4.inl"
    "/Users/jason/AndroidStudioProjects/AndroidJniRender/app/.cxx/MiaoMiaoDebug/401f114x/arm64-v8a/Assimp/code/../include/assimp/config.h"
    "/Users/jason/AndroidStudioProjects/AndroidJniRender/app/src/main/thirdpartCpp/Assimp/code/../include/assimp/defs.h"
    "/Users/jason/AndroidStudioProjects/AndroidJniRender/app/src/main/thirdpartCpp/Assimp/code/../include/assimp/Defines.h"
    "/Users/jason/AndroidStudioProjects/AndroidJniRender/app/src/main/thirdpartCpp/Assimp/code/../include/assimp/cfileio.h"
    "/Users/jason/AndroidStudioProjects/AndroidJniRender/app/src/main/thirdpartCpp/Assimp/code/../include/assimp/light.h"
    "/Users/jason/AndroidStudioProjects/AndroidJniRender/app/src/main/thirdpartCpp/Assimp/code/../include/assimp/material.h"
    "/Users/jason/AndroidStudioProjects/AndroidJniRender/app/src/main/thirdpartCpp/Assimp/code/../include/assimp/material.inl"
    "/Users/jason/AndroidStudioProjects/AndroidJniRender/app/src/main/thirdpartCpp/Assimp/code/../include/assimp/matrix3x3.h"
    "/Users/jason/AndroidStudioProjects/AndroidJniRender/app/src/main/thirdpartCpp/Assimp/code/../include/assimp/matrix3x3.inl"
    "/Users/jason/AndroidStudioProjects/AndroidJniRender/app/src/main/thirdpartCpp/Assimp/code/../include/assimp/matrix4x4.h"
    "/Users/jason/AndroidStudioProjects/AndroidJniRender/app/src/main/thirdpartCpp/Assimp/code/../include/assimp/matrix4x4.inl"
    "/Users/jason/AndroidStudioProjects/AndroidJniRender/app/src/main/thirdpartCpp/Assimp/code/../include/assimp/mesh.h"
    "/Users/jason/AndroidStudioProjects/AndroidJniRender/app/src/main/thirdpartCpp/Assimp/code/../include/assimp/postprocess.h"
    "/Users/jason/AndroidStudioProjects/AndroidJniRender/app/src/main/thirdpartCpp/Assimp/code/../include/assimp/quaternion.h"
    "/Users/jason/AndroidStudioProjects/AndroidJniRender/app/src/main/thirdpartCpp/Assimp/code/../include/assimp/quaternion.inl"
    "/Users/jason/AndroidStudioProjects/AndroidJniRender/app/src/main/thirdpartCpp/Assimp/code/../include/assimp/scene.h"
    "/Users/jason/AndroidStudioProjects/AndroidJniRender/app/src/main/thirdpartCpp/Assimp/code/../include/assimp/metadata.h"
    "/Users/jason/AndroidStudioProjects/AndroidJniRender/app/src/main/thirdpartCpp/Assimp/code/../include/assimp/texture.h"
    "/Users/jason/AndroidStudioProjects/AndroidJniRender/app/src/main/thirdpartCpp/Assimp/code/../include/assimp/types.h"
    "/Users/jason/AndroidStudioProjects/AndroidJniRender/app/src/main/thirdpartCpp/Assimp/code/../include/assimp/vector2.h"
    "/Users/jason/AndroidStudioProjects/AndroidJniRender/app/src/main/thirdpartCpp/Assimp/code/../include/assimp/vector2.inl"
    "/Users/jason/AndroidStudioProjects/AndroidJniRender/app/src/main/thirdpartCpp/Assimp/code/../include/assimp/vector3.h"
    "/Users/jason/AndroidStudioProjects/AndroidJniRender/app/src/main/thirdpartCpp/Assimp/code/../include/assimp/vector3.inl"
    "/Users/jason/AndroidStudioProjects/AndroidJniRender/app/src/main/thirdpartCpp/Assimp/code/../include/assimp/version.h"
    "/Users/jason/AndroidStudioProjects/AndroidJniRender/app/src/main/thirdpartCpp/Assimp/code/../include/assimp/cimport.h"
    "/Users/jason/AndroidStudioProjects/AndroidJniRender/app/src/main/thirdpartCpp/Assimp/code/../include/assimp/importerdesc.h"
    "/Users/jason/AndroidStudioProjects/AndroidJniRender/app/src/main/thirdpartCpp/Assimp/code/../include/assimp/Importer.hpp"
    "/Users/jason/AndroidStudioProjects/AndroidJniRender/app/src/main/thirdpartCpp/Assimp/code/../include/assimp/DefaultLogger.hpp"
    "/Users/jason/AndroidStudioProjects/AndroidJniRender/app/src/main/thirdpartCpp/Assimp/code/../include/assimp/ProgressHandler.hpp"
    "/Users/jason/AndroidStudioProjects/AndroidJniRender/app/src/main/thirdpartCpp/Assimp/code/../include/assimp/IOStream.hpp"
    "/Users/jason/AndroidStudioProjects/AndroidJniRender/app/src/main/thirdpartCpp/Assimp/code/../include/assimp/IOSystem.hpp"
    "/Users/jason/AndroidStudioProjects/AndroidJniRender/app/src/main/thirdpartCpp/Assimp/code/../include/assimp/Logger.hpp"
    "/Users/jason/AndroidStudioProjects/AndroidJniRender/app/src/main/thirdpartCpp/Assimp/code/../include/assimp/LogStream.hpp"
    "/Users/jason/AndroidStudioProjects/AndroidJniRender/app/src/main/thirdpartCpp/Assimp/code/../include/assimp/NullLogger.hpp"
    "/Users/jason/AndroidStudioProjects/AndroidJniRender/app/src/main/thirdpartCpp/Assimp/code/../include/assimp/cexport.h"
    "/Users/jason/AndroidStudioProjects/AndroidJniRender/app/src/main/thirdpartCpp/Assimp/code/../include/assimp/Exporter.hpp"
    "/Users/jason/AndroidStudioProjects/AndroidJniRender/app/src/main/thirdpartCpp/Assimp/code/../include/assimp/DefaultIOStream.h"
    "/Users/jason/AndroidStudioProjects/AndroidJniRender/app/src/main/thirdpartCpp/Assimp/code/../include/assimp/DefaultIOSystem.h"
    "/Users/jason/AndroidStudioProjects/AndroidJniRender/app/src/main/thirdpartCpp/Assimp/code/../include/assimp/SceneCombiner.h"
    )
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xassimp-devx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/assimp/Compiler" TYPE FILE FILES
    "/Users/jason/AndroidStudioProjects/AndroidJniRender/app/src/main/thirdpartCpp/Assimp/code/../include/assimp/Compiler/pushpack1.h"
    "/Users/jason/AndroidStudioProjects/AndroidJniRender/app/src/main/thirdpartCpp/Assimp/code/../include/assimp/Compiler/poppack1.h"
    "/Users/jason/AndroidStudioProjects/AndroidJniRender/app/src/main/thirdpartCpp/Assimp/code/../include/assimp/Compiler/pstdint.h"
    )
endif()

