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

import android.R.attr.action
import android.annotation.SuppressLint
import android.app.Activity
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
                        MoveNative(dx, dy)
                    }
                }
                MotionEvent.ACTION_UP, MotionEvent.ACTION_POINTER_UP, MotionEvent.ACTION_CANCEL -> {
                    mTwoFingerPointerId = INVALID_POINTER_ID
                }
                MotionEvent.ACTION_POINTER_DOWN -> {
                    mTwoFingerPointerId = event.getActionIndex()
                    val x = event.getX(mTwoFingerPointerId)
                    val y = event.getY(mTwoFingerPointerId)
                    mLastTouchX = x
                    mLastTouchY = y
                }
            }

            return true
        }
    }

    // 此类检测双击手势并跟踪单指拖动手势
    internal inner class MyTapScrollListener : SimpleOnGestureListener() {
        override fun onDoubleTap(event: MotionEvent): Boolean {
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
                ScrollNative(distanceX, distanceY, e2.x, e2.y)
            }
            return true
        }
    }

    // 此类检测捏和缩放手势
    private inner class ScaleListener : SimpleOnScaleGestureListener() {
        override fun onScale(detector: ScaleGestureDetector): Boolean {
            ScaleNative(detector.scaleFactor)
            return true
        }
    }

    companion object {
        var INVALID_POINTER_ID = -100
    }
}