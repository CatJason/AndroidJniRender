package com.anandmuralidhar.assimpandroid

import android.annotation.SuppressLint
import android.app.Activity
import android.content.res.AssetManager
import android.opengl.GLSurfaceView
import android.os.Bundle
import com.hyq.hm.test.assimp.R

class AssimpActivity : Activity() {
    private var mGLView: GLSurfaceView? = null
    private external fun createObjectNative(assetManager: AssetManager, pathToInternalDir: String)
    private external fun deleteObjectNative()
    var mGestureControl: GestureClass? = null

    public override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        val assetManager = assets
        val pathToInternalDir = filesDir.absolutePath

        // 调用本地构造函数创建一个对象
        createObjectNative(assetManager, pathToInternalDir)

        // 布局只包含两个组件，一个GLSurfaceView和一个TextView
        setContentView(R.layout.assimp_layout)
        initGLView()
    }

    private fun initGestureControl() {
        if (mGestureControl == null) {
            mGestureControl = GestureClass(this)
        }
    }

    @SuppressLint("ClickableViewAccessibility")
    private fun initGLView() {
        mGLView = findViewById<GLSurfaceView?>(R.id.gl_surface_view).apply {
            initGestureControl()
            mGestureControl?.TwoFingerGestureListener?.let {
                setOnTouchListener(it)
            }
        }
    }

    override fun onResume() {
        super.onResume()
        mGLView?.onResume()
    }

    override fun onPause() {
        super.onPause()
        mGLView?.onPause()
    }

    override fun onDestroy() {
        super.onDestroy()
        deleteObjectNative()
    }

    companion object {
        /**
         * 加载libModelAssimpNative.so，因为它包含了所有的本地函数
         */
        init {
            System.loadLibrary("native-lib")
        }
    }
}