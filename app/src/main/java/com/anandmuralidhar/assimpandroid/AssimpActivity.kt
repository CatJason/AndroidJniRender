package com.anandmuralidhar.assimpandroid

import android.annotation.SuppressLint
import android.app.Activity
import android.content.res.AssetManager
import android.opengl.GLSurfaceView
import android.os.Bundle
import android.util.Log
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

        Log.d("AssimpActivity", "onCreate: 调用本地构造函数创建一个对象")
        // 调用本地构造函数创建一个对象
        createObjectNative(assetManager, pathToInternalDir)

        Log.d("AssimpActivity", "onCreate: 设置布局文件")
        // 布局只包含两个组件，一个GLSurfaceView和一个TextView
        setContentView(R.layout.assimp_layout)
        initGLView()
    }

    private fun initGestureControl() {
        if (mGestureControl == null) {
            Log.d("AssimpActivity", "initGestureControl: 初始化GestureClass")
            mGestureControl = GestureClass(this)
        }
    }

    @SuppressLint("ClickableViewAccessibility")
    private fun initGLView() {
        Log.d("AssimpActivity", "initGLView: 初始化GLSurfaceView")
        mGLView = findViewById<GLSurfaceView?>(R.id.gl_surface_view).apply {
            initGestureControl()
            mGestureControl?.TwoFingerGestureListener?.let {
                Log.d("AssimpActivity", "initGLView: 设置双指手势监听器")
                setOnTouchListener(it)
            }
        }
    }

    override fun onResume() {
        super.onResume()
        Log.d("AssimpActivity", "onResume: 恢复GLSurfaceView")
        mGLView?.onResume()
    }

    override fun onPause() {
        super.onPause()
        Log.d("AssimpActivity", "onPause: 暂停GLSurfaceView")
        mGLView?.onPause()
    }

    override fun onDestroy() {
        super.onDestroy()
        Log.d("AssimpActivity", "onDestroy: 调用本地删除函数销毁对象")
        deleteObjectNative()
    }

    companion object {
        /**
         * 加载libModelAssimpNative.so，因为它包含了所有的本地函数
         */
        init {
            Log.d("AssimpActivity", "init: 加载本地库libModelAssimpNative.so")
            System.loadLibrary("native-lib")
        }
    }
}