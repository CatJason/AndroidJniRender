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
import android.view.ViewGroup
import com.anandmuralidhar.assimpandroid.MyGLRenderer
import java.lang.Exception

internal class MyGLSurfaceView(context: Context?, attrs: AttributeSet?) :
    GLSurfaceView(context, attrs) {
    private val mRenderer: MyGLRenderer = MyGLRenderer()

    init {
        try {
            setEGLContextClientVersion(2)
            setRenderer(mRenderer)
            renderMode = RENDERMODE_CONTINUOUSLY

        } catch (e: Exception) {
            // 出现问题，出错了！
            Log.e("MyGLSurfaceView", "无法创建GLES上下文！", e)
        }
    }
}
