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

import android.content.Context
import android.opengl.GLSurfaceView
import android.util.AttributeSet
import android.util.Log
import com.anandmuralidhar.assimpandroid.MyGLRenderer
import java.lang.Exception

internal class MyGLSurfaceView(context: Context?, attrs: AttributeSet?) :
    GLSurfaceView(context, attrs) {
    private val mRenderer: MyGLRenderer = MyGLRenderer()

    init {
        try {
            // 创建GLES上下文。即使我们指定了OpenGL ES 2，它将尝试在手机上创建最高可能的上下文
            setEGLContextClientVersion(2)

            // 设置我们自定义的Renderer用于在创建的SurfaceView上绘
            setRenderer(mRenderer)

            // 连续调用onDrawFrame(...)
            //设置渲染模式为RENDERMODE_CONTINUOUSLY，即连续渲染模式。在此模式下，渲染器将被重复调用以持续重新渲染场景。
            renderMode = RENDERMODE_CONTINUOUSLY
        } catch (e: Exception) {

            // 出现问题，出错了！
            Log.e("MyGLSurfaceView", "无法创建GLES上下文！", e)
        }
    }
}