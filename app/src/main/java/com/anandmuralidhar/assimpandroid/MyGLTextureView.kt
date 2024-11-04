package com.anandmuralidhar.assimpandroid

import android.content.Context
import android.os.Handler
import android.os.Looper
import android.util.AttributeSet
import com.anandmuralidhar.gltextureview.GLTextureView

class MyGLTextureView(context: Context?, attrs: AttributeSet?) :
    GLTextureView(context, attrs) {
    private val mRenderer: MyGLRenderer = MyGLRenderer()
    private val frameRate = 1000 / 30 // 每帧间隔33.33ms
    private val frameHandler: Handler = Handler(Looper.getMainLooper()) // 使用主线程的 Looper

    init {
        // 设置EGL上下文版本
        setEGLContextClientVersion(2)
        setRenderer(mRenderer)
        renderMode = RENDERMODE_WHEN_DIRTY // 手动刷新模式
        isOpaque = false
    }

    override fun onAttachedToWindow() {
        super.onAttachedToWindow()
        startRendering()
    }

    override fun onDetachedFromWindow() {
        stopRendering()
        super.onDetachedFromWindow()
    }

    // 开始手动刷新
    fun startRendering() {
        frameHandler.post(frameRunnable)
    }

    // 停止手动刷新
    fun stopRendering() {
        frameHandler.removeCallbacks(frameRunnable)
    }

    private val frameRunnable = object : Runnable {
        override fun run() {
            requestRender() // 请求重绘
            frameHandler.postDelayed(this, frameRate.toLong()) // 每隔指定时间执行
        }
    }
}
