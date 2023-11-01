/*
 *    Copyright 2016 Anand Muralidhar
 *
 *    Licensed under the Apache License, Version 2.0 (the "License");
 *    you may not use this file except in compliance with the License.
 *    You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *    Unless required by applicable law or agreed to in writing, software
 *    distributed under the License is distributed on an "AS IS" BASIS,
 *    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *    See the License for the specific language governing permissions and
 *    limitations under the License.
 */
package com.anandmuralidhar.assimpandroid

import android.opengl.GLSurfaceView
import android.util.Log
import javax.microedition.khronos.egl.EGLConfig
import javax.microedition.khronos.opengles.GL10

internal class MyGLRenderer : GLSurfaceView.Renderer {
    private external fun drawFrameNative()
    private external fun surfaceCreatedNative()
    private external fun surfaceChangedNative(width: Int, height: Int)

    // 当表面创建或重新创建时调用
    override fun onSurfaceCreated(gl: GL10, config: EGLConfig) {
        // 创建（或重新创建）用于渲染的本地对象
        Log.d("MyGLRenderer", "onSurfaceCreated")
        surfaceCreatedNative()
    }

    // 用于绘制当前帧
    override fun onDrawFrame(unused: GL10) {
        // 调用本地渲染函数
        drawFrameNative()
    }

    // 在表面创建后和GLES表面大小发生变化时调用，这通常会在设备方向改变时发生
    override fun onSurfaceChanged(unused: GL10, width: Int, height: Int) {
        // 我们需要设置GLES视口来处理大小变化
        Log.d("MyGLRenderer", "onSurfaceChanged")
        surfaceChangedNative(width, height)
    }
}