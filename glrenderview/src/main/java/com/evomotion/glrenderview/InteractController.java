package com.evomotion.glrenderview;

import android.content.Context;
import android.content.res.Configuration;
import android.hardware.Sensor;
import android.hardware.SensorEvent;
import android.hardware.SensorEventListener;
import android.hardware.SensorManager;
import android.util.Log;
import android.view.GestureDetector;
import android.view.MotionEvent;
import android.view.ScaleGestureDetector;
import android.view.View;

import com.evomotion.glrenderview.interfaces.IOnTapListener;
import com.evomotion.glrenderview.interfaces.IRenderInteractEvent;

import java.lang.ref.WeakReference;

/**
 * Created by zhuyuanxuan on 23/11/2016.
 * gl-render
 */

public class InteractController implements View.OnTouchListener {
    private static final String TAG = "InteractControl";

    public static final int NULL_MODE = -1;
    public static final int TOUCH_MODE = 1;
    public static final int SENSOR_MODE = 2;

    private static final float INERTIA_PARAM = 0.007f;

    private Context mContext;
    private GestureDetector.SimpleOnGestureListener mDragListener;
    private GestureDetector.SimpleOnGestureListener mSensorClickListener;
    private ScaleGestureDetector.SimpleOnScaleGestureListener mScaleListener;
    private GestureDetector mTouchGestureDetector;
    private GestureDetector mTapDetectorInSensorMode;
    private ScaleGestureDetector mScaleGestureDetector;
    private SensorManager mSensorManager;
    private Sensor mAcceleSensor;
    private GyroSenorEventListener mSensorListener;
    private IOnTapListener mTapListener;
    private IRenderInteractEvent mRenderInteractEvent;
    private int mControlMode;
    private boolean mIsScale;

    public InteractController(Context context, IRenderInteractEvent view) {
        this.mRenderInteractEvent = view;
        this.mContext = context;
        initSensor();
        mIsScale = false;
        mControlMode = -1;
    }

    public void setControlMode(final View view, int mode) {
        mControlMode = mode;
        if (mode == SENSOR_MODE) {
            Log.i(TAG, "sensor mode");
            mSensorManager.registerListener(mSensorListener, mAcceleSensor,
                    SensorManager.SENSOR_DELAY_GAME);
            final WeakReference<View> weakView = new WeakReference<>(view);
            view.postDelayed(new Runnable() {
                @Override
                public void run() {
                    if (weakView.get() != null) {
                        mSensorListener.resetOrientation(weakView.get().getRight() - weakView.get
                                ().getLeft(), weakView.get().getBottom() - weakView.get().getTop());
                    }
                }
            }, 50);
        } else {
            Log.i(TAG, "not in sensor mode");
            mSensorManager.unregisterListener(mSensorListener);
        }
    }

    public int getControlMode() {
        return mControlMode;
    }

    public void setTapListener(IOnTapListener mTapListener) {
        this.mTapListener = mTapListener;
    }

    public void initSensor() {
        initListener();
        initDetector();

        mSensorManager = (SensorManager) mContext.getSystemService(Context.SENSOR_SERVICE);
        mAcceleSensor = mSensorManager.getDefaultSensor(Sensor.TYPE_GYROSCOPE);
        mSensorListener = new GyroSenorEventListener();
    }

    //
    // sensor manage
    //

    /**
     * init listeners and implement callback method
     */
    private void initListener() {
        mDragListener = new GestureDetector.SimpleOnGestureListener() {
            @Override
            public boolean onDown(MotionEvent e) {
                return true;
            }

            @Override
            public boolean onScroll(MotionEvent e1, MotionEvent e2,
                                    float distanceX, float distanceY) {
                // if now in scale, disable the drag event
                if (!mIsScale) {
                    scrollAct(distanceX, distanceY);
                }
                return false;
            }

            @Override
            public boolean onFling(MotionEvent e1, MotionEvent e2,
                                   float velocityX, float velocityY) {
                // Log.i(TAG, "velocity x" + velocityX + " y " + velocityY);
                // mRenderInteractEvent.onInertia(velocityX * -1 * INERTIA_PARAM,
                //         velocityY * -1 * INERTIA_PARAM);

                inertiaAct(velocityX, velocityY);
                return false;
            }

            @Override
            public boolean onSingleTapConfirmed(MotionEvent e) {
                // deal the video control
                if (mTapListener != null) {
                    mTapListener.onSingleTap();
                }
                return false;
            }
        };

        // this listener is for mTapDetectorInSensorMode will return false for other
        // GestureDetector to handle, since we want to handle scale event in Gyroscope/Sensor mode.
        mSensorClickListener = new GestureDetector.SimpleOnGestureListener() {
            @Override
            public boolean onSingleTapConfirmed(MotionEvent e) {
                // deal the video control
                if (mTapListener != null) {
                    mTapListener.onSingleTap();
                }

                return false;
            }
        };
        mScaleListener = new MyScaleGestureListener();

    }

    ////////////////
    // Private
    ////////////////

    private void initDetector() {
        // init Detectors
        mTouchGestureDetector = new GestureDetector(mContext, mDragListener);
        // this detector is used in sensor mode
        mTapDetectorInSensorMode = new GestureDetector(mContext, mSensorClickListener);
        mScaleGestureDetector = new ScaleGestureDetector(mContext, mScaleListener);
    }

    /**
     * @param event motion event
     * @return True if the event was handled, false otherwise.
     * Use diff Detector to deal the touch action in diff mode(finger or sensor).
     */
    private boolean handleTouchEvent(MotionEvent event) {
        if (SENSOR_MODE == mControlMode) {
            // Here first detect single tap in mTapDetectorInSensorMode, if detected, will return
            // false then will use mScaleGestureDetector to detect the scale event and if
            // detected, will return true, means event is handled.
            return mTapDetectorInSensorMode.onTouchEvent(event) ||
                    mScaleGestureDetector.onTouchEvent(event);
        } else {
            return mTouchGestureDetector.onTouchEvent(event) ||
                    mScaleGestureDetector.onTouchEvent(event);
        }
    }

    //////////////
    // Lifecycle
    /////////////

    public void resume() {
        if (mControlMode == SENSOR_MODE) {
            Log.i(TAG, "Interactor resume, register listener");
            mSensorManager.registerListener(mSensorListener, mAcceleSensor, SensorManager
                    .SENSOR_DELAY_GAME);
        }
    }

    public void pause() {
        Log.i(TAG, "Interactor pause");
        mSensorManager.unregisterListener(mSensorListener);
    }

    //////
    // OnTouchListener
    /////

    /**
     * Called when a touch event is dispatched to a view. This allows listeners to
     * get a chance to respond before the target view.
     *
     * @param v     The view the touch event has been dispatched to.
     * @param event The MotionEvent object containing full information about
     *              the event.
     * @return True if the listener has consumed the event, false otherwise.
     */
    @Override
    public boolean onTouch(View v, MotionEvent event) {
        return handleTouchEvent(event);
    }

    //////////////////////
    // The gesture actions
    //////////////////////

    /**
     * Actions deal the scroll action, will call onSpan() method implemented by render view.
     *
     * @param dx delta x
     * @param dy delta y
     */
    private void scrollAct(float dx, float dy) {
        if (Math.abs(dx) < 3.0f) dx = 0f;
        if (Math.abs(dy) < 3.0f) dy = 0f;
        mRenderInteractEvent.onSpan(dx, dy);
    }

    private void sensorRotateAct(float deltaAxisX, float deltaAxisY) {
        mRenderInteractEvent.onSensorRotate(deltaAxisX, -deltaAxisY);
    }

    /**
     * @param factor the scale factor
     *               ZhuYuanxuan: todo: change the implement of every view.
     */
    private void scaleAct(float factor) {
        mRenderInteractEvent.onZoom(factor);
    }

    private void inertiaAct(float vX, float vY) {
        this.mRenderInteractEvent.onInertia(vX * -0.5f, vY * -0.5f);
    }

    /**
     * MyScaleGestureListener
     * This logic is for meizu M3 note, first scale will jitter.
     */
    private class MyScaleGestureListener extends ScaleGestureDetector.SimpleOnScaleGestureListener {
        float mLastScaleFactor = 1.0f;
        int mIsFirstZoomIn = 1;
        int mTimes = 0;
        int mSkipTimes = 0;

        @Override
        public boolean onScale(ScaleGestureDetector detector) {
            float currentFactor = detector.getScaleFactor();
            if (mIsFirstZoomIn == 1 && mTimes < 50) {
                // first time zoom in
                if (mLastScaleFactor < 1 && currentFactor > 1 && mSkipTimes < 4) {
                    // Skip this value
                    ++mSkipTimes;
                    return false;
                }
            }
            mSkipTimes = 0;
            scaleAct(detector.getScaleFactor());
            mLastScaleFactor = currentFactor;
            ++mTimes;
            return true;
        }

        @Override
        public boolean onScaleBegin(ScaleGestureDetector detector) {
            mIsScale = true;
            mLastScaleFactor = 1;
            return super.onScaleBegin(detector);
        }

        @Override
        public void onScaleEnd(ScaleGestureDetector detector) {
            mIsScale = false;
            super.onScaleEnd(detector);
            if (mIsFirstZoomIn == 1) {
                mIsFirstZoomIn = 0;
            }
        }
    }

    /**
     * Gyroscope listener
     */
    private class GyroSenorEventListener implements SensorEventListener {
        private static final float NS2S = 1.0f / 1000000000.0f;
        private long mGyroEventTimestamp = 0;

        int orientation;

        @Override
        public void onSensorChanged(SensorEvent event) {
            if (event.sensor.getType() == Sensor.TYPE_GYROSCOPE) {
                // This time step's delta rotation to be multiplied by the current rotation
                // after computing it from the gyro sample data.
                if (mGyroEventTimestamp != 0) {
                    final float dT = (event.timestamp - mGyroEventTimestamp) * NS2S;
                    float axisX = 0;
                    float axisY = 0;
                    if (orientation == Configuration.ORIENTATION_LANDSCAPE) {
                        axisX = -event.values[1];
                        axisY = event.values[0];
                    } else {
                        axisX = event.values[0];
                        axisY = event.values[1];
                    }
                    // Axis of the rotation sample, not normalized yet. else {
                    float deltaRotateRadX = axisX * dT;
                    float deltaRotateRadY = axisY * dT;

                    sensorRotateAct(deltaRotateRadX, deltaRotateRadY);
                }
                mGyroEventTimestamp = event.timestamp;
            }
        }

        @Override
        public void onAccuracyChanged(Sensor sensor, int accuracy) {

        }

        void resetOrientation(int width, int height) {
            if (width > height) {
                orientation = Configuration.ORIENTATION_LANDSCAPE;
            } else {
                orientation = Configuration.ORIENTATION_PORTRAIT;
            }
            // Log.e(TAG, "get orientation: " + orientation);
        }
    }
}
