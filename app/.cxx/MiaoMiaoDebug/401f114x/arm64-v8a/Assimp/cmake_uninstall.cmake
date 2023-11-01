IF(NOT EXISTS "/Users/jason/AndroidStudioProjects/AndroidJniRender/app/.cxx/MiaoMiaoDebug/401f114x/arm64-v8a/Assimp/install_manifest.txt")
  MESSAGE(FATAL_ERROR "Cannot find install manifest: \"/Users/jason/AndroidStudioProjects/AndroidJniRender/app/.cxx/MiaoMiaoDebug/401f114x/arm64-v8a/Assimp/install_manifest.txt\"")
ENDIF(NOT EXISTS "/Users/jason/AndroidStudioProjects/AndroidJniRender/app/.cxx/MiaoMiaoDebug/401f114x/arm64-v8a/Assimp/install_manifest.txt")

FILE(READ "/Users/jason/AndroidStudioProjects/AndroidJniRender/app/.cxx/MiaoMiaoDebug/401f114x/arm64-v8a/Assimp/install_manifest.txt" files)
STRING(REGEX REPLACE "\n" ";" files "${files}")
FOREACH(file ${files})
  MESSAGE(STATUS "Uninstalling \"$ENV{DESTDIR}${file}\"")
  EXEC_PROGRAM(
    "/Users/jason/Library/Android/sdk/cmake/3.10.2.4988404/bin/cmake" ARGS "-E remove \"$ENV{DESTDIR}${file}\""
    OUTPUT_VARIABLE rm_out
    RETURN_VALUE rm_retval
    )
  IF(NOT "${rm_retval}" STREQUAL 0)
    MESSAGE(FATAL_ERROR "Problem when removing \"$ENV{DESTDIR}${file}\"")
  ENDIF(NOT "${rm_retval}" STREQUAL 0)
ENDFOREACH(file)
