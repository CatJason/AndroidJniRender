package com.anandmuralidhar.assimpandroid

import android.R.attr.action
import android.annotation.SuppressLint
import android.app.Activity
import android.util.Log
import android.view.GestureDetector.SimpleOnGestureListener
import android.view.MotionEvent
import android.view.ScaleGestureDetector
import android.view.ScaleGestureDetector.SimpleOnScaleGestureListener
import android.view.View
import android.view.View.OnTouchListener
import androidx.core.view.GestureDetectorCompat
import androidx.core.view.MotionEventCompat

class GestureClass(activity: Activity) {
    // 实例化两个监听器，用于检测双击/拖动和缩放手势
    private val mTapScrollDetector: GestureDetectorCompat = GestureDetectorCompat(activity, MyTapScrollListener())
    private val mScaleDetector: ScaleGestureDetector = ScaleGestureDetector(
        activity.applicationContext,
        ScaleListener()
    )
    private external fun DoubleTapNative()
    private external fun ScrollNative(
        distanceX: Float,
        distanceY: Float,
        positionX: Float,
        positionY: Float
    )

    private external fun ScaleNative(scaleFactor: Float)
    private external fun MoveNative(distanceX: Float, distanceY: Float)
    private var mTwoFingerPointerId = INVALID_POINTER_ID

    // 此监听器检测两个手指的拖动手势
    var TwoFingerGestureListener: OnTouchListener = object : OnTouchListener {
        private var mLastTouchX = 0f
        private var mLastTouchY = 0f

        @SuppressLint("ClickableViewAccessibility")
        override fun onTouch(v: View, event: MotionEvent): Boolean {
            Log.d("GestureClass", "onTouch: 触摸事件 $event")
            mTapScrollDetector.onTouchEvent(event)
            mScaleDetector.onTouchEvent(event)

            when (event.action and MotionEvent.ACTION_MASK) {
                MotionEvent.ACTION_MOVE -> {
                    if (mTwoFingerPointerId != INVALID_POINTER_ID) {
                        val pointerIndex = event.findPointerIndex(mTwoFingerPointerId)
                        val x = event.getX(pointerIndex)
                        val y = event.getY(pointerIndex)
                        val dx = x - mLastTouchX
                        val dy = y - mLastTouchY
                        mLastTouchX = x
                        mLastTouchY = y
                        Log.d("GestureClass", "onTouch: 两指移动 dx=$dx, dy=$dy")
                        MoveNative(dx, dy)
                    }
                }
                MotionEvent.ACTION_UP, MotionEvent.ACTION_POINTER_UP, MotionEvent.ACTION_CANCEL -> {
                    Log.d("GestureClass", "onTouch: 触摸事件结束或取消")
                    mTwoFingerPointerId = INVALID_POINTER_ID
                }
                MotionEvent.ACTION_POINTER_DOWN -> {
                    mTwoFingerPointerId = event.getActionIndex()
                    val x = event.getX(mTwoFingerPointerId)
                    val y = event.getY(mTwoFingerPointerId)
                    mLastTouchX = x
                    mLastTouchY = y
                    Log.d("GestureClass", "onTouch: 两指按下 x=$x, y=$y")
                }
            }

            return true
        }
    }

    // 此类检测双击手势并跟踪单指拖动手势
    internal inner class MyTapScrollListener : SimpleOnGestureListener() {
        override fun onDoubleTap(event: MotionEvent): Boolean {
            Log.d("GestureClass", "onDoubleTap: 双击事件")
            DoubleTapNative()
            return true
        }

        // 如果用户使用一个或两个手指滚动，则调用此函数
        // 如果屏幕上放置了两个手指，则忽略该调用
        override fun onScroll(
            e1: MotionEvent,
            e2: MotionEvent,
            distanceX: Float,
            distanceY: Float
        ): Boolean {
            if (mTwoFingerPointerId == INVALID_POINTER_ID) {
                Log.d("GestureClass", "onScroll: 单指滚动 distanceX=$distanceX, distanceY=$distanceY, x=${e2.x}, y=${e2.y}")
                ScrollNative(distanceX, distanceY, e2.x, e2.y)
            }
            return true
        }
    }

    // 此类检测捏和缩放手势
    private inner class ScaleListener : SimpleOnScaleGestureListener() {
        override fun onScale(detector: ScaleGestureDetector): Boolean {
            Log.d("GestureClass", "onScale: 缩放手势 scaleFactor=${detector.scaleFactor}")
            ScaleNative(detector.scaleFactor)
            return true
        }
    }

    companion object {
        var INVALID_POINTER_ID = -100
    }
}