package com.evomotion.glrenderview;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.opengl.Matrix;
import android.util.AttributeSet;
import android.util.Log;
import android.view.MotionEvent;
import android.view.View;

import com.evomotion.glrenderview.interfaces.IDisplayCallback;
import com.evomotion.glrenderview.interfaces.IGlRenderView;
import com.evomotion.glrenderview.interfaces.IRenderCallback;
import com.evomotion.glrenderview.interfaces.IRenderErrorCallback;
import com.evomotion.glrenderview.interfaces.IRenderInteractEvent;
import com.evomotion.glrenderview.interfaces.IShotCallback;
import com.google.vr.sdk.base.Eye;
import com.google.vr.sdk.base.GvrView;
import com.google.vr.sdk.base.HeadTransform;
import com.google.vr.sdk.base.Viewport;

import java.lang.ref.WeakReference;
import java.util.Timer;
import java.util.TimerTask;

import javax.microedition.khronos.egl.EGLConfig;

/**
 * Created by zhuyuanxuan on 29/11/2016.
 * gl-render
 */

public class BinocularVrView extends GvrView implements GvrView.StereoRenderer,
        IGlRenderView, IRenderInteractEvent {
    static {
        System.loadLibrary("glrender");
    }

    private static final float CAMERA_Z = 0.01f;
    private static final float Z_NEAR = 0.1f;
    private static final float Z_FAR = 100.0f;
    private static final String TAG = BinocularVrView.class.getName();

    private float[] mCameraView;
    private float[] mEulerAngles;

    private int projection_flag;
    private int mTextureType;
    private String mPicPath;
    private IRenderCallback mRenderCallback;
    private IRenderErrorCallback mRenderErrorCallback;
    private GlSurfaceViewEventSignal mGlSurfaceViewEventSignal;
    // pointers in c++ code
    private long mRenderControllerPtr;
    private long mTextureControllerPtr;
    private Timer timer = new Timer();

    public BinocularVrView(Context context) {
        super(context);
        Log.i(TAG, "init Binocular Vr view");
        initView();
    }

    public BinocularVrView(Context context, AttributeSet attrs) {
        super(context, attrs);
        Log.i(TAG, "init Binocular Vr view with attrs");
        initView();
    }

    //////////////////////
    //GvrView.StereoRenderer
    //////////////////////

    @Override
    public void onNewFrame(HeadTransform headTransform) {
        headTransform.getEulerAngles(mEulerAngles, 0);
        if (mGlSurfaceViewEventSignal.needUpdateYuvTexture()) {
            if (mTextureControllerPtr != 0) {
                if (timer != null) {
                    timer.cancel();
                }

                timer = new Timer();
                final WeakReference<BinocularVrView> weakSelf = weakSelf();
                timer.schedule(new TimerTask() {
                    @Override
                    public void run() {
                        if (weakSelf.get() != null && weakSelf.get().mRenderCallback != null) {
                            weakSelf.get().mRenderCallback.onSurfaceFrameTimeout();
                        }
                    }
                }, 3000);

                mGlSurfaceViewEventSignal.useTextureData(mTextureControllerPtr);
            }
        }
    }

    @Override
    public void onDrawEye(Eye eye) {
        if (mGlSurfaceViewEventSignal.isFirstFrame() || mTextureType == VideoFormat.IMAGE) {
            float[] cameraViewMat = new float[16];
            Matrix.multiplyMM(cameraViewMat, 0, eye.getEyeView(), 0, mCameraView, 0);
            updateLookAt(cameraViewMat, mRenderControllerPtr);

            if (projection_flag > 0) {
                float[] perspective = eye.getPerspective(Z_NEAR, Z_FAR);
                updateProjection(perspective, mRenderControllerPtr);
                projection_flag = 0;
                // before draw frame reactive texture
                reactiveTex(mTextureControllerPtr);
            }
            RenderView.drawFrame(mRenderControllerPtr, mTextureControllerPtr);
        }
    }

    @Override
    public void onFinishFrame(Viewport viewport) {
    }

    @Override
    public void onSurfaceCreated(EGLConfig eglConfig) {
        if (mTextureType != RenderType.NULL) {
            long[] longs = RenderView.surfaceCreated(mTextureType, RenderType.VR);
            if (longs != null) {
                mRenderControllerPtr = longs[0];
                mTextureControllerPtr = longs[1];
            } else {
                Log.e(TAG, "return invalid pointers!");
            }
        } else {
            Log.w(TAG, "Error texture type unknown!");
        }

        if (mTextureType > VideoFormat.IMAGE) {
            if (mRenderCallback != null) {
                mRenderCallback.onSurfaceCreated();
            }

            Bitmap logo = WaterMarkerProducer.get(getContext());
            RenderView.setLogoTexture(logo, mTextureControllerPtr);

        }
    }

    @Override
    public void onSurfaceChanged(int width, int height) {
        RenderView.surfaceChanged(width, height, mRenderControllerPtr,
                mTextureControllerPtr);
        if (mPicPath != null) {
            setPicTexture(mPicPath);
        }

        if (mRenderCallback != null) {
            mRenderCallback.onSurfaceChanged(width, height);
        }
    }

    @Override
    public void onRendererShutdown() {
        if (mRenderControllerPtr == 0 || mTextureControllerPtr == 0) {
            return;
        }
        Log.e(TAG, "Vr view shutdown");
        RenderView.destroy(mRenderControllerPtr, mTextureControllerPtr, 0);
        mRenderControllerPtr = 0;
        mTextureControllerPtr = 0;
    }

    private WeakReference<BinocularVrView> weakSelf() {
        return new WeakReference<>(this);
    }

    ///////////////
    // LifeCycle
    //////////////

    @Override
    protected void onDetachedFromWindow() {
        super.onDetachedFromWindow();
        Log.e(TAG, "vr view detached from window");
    }

    @Override
    public void onPause() {
        super.onPause();
        Log.e(TAG, "Vr view pause");
    }

    @Override
    public void onResume() {
        super.onResume();
    }

    //////
    // IGlRenderView implement
    /////

    @Override
    public void setRenderCallback(IRenderCallback renderCallback) {
        this.mRenderCallback = renderCallback;
    }

    @Override
    public void setRenderErrorCallback(IRenderErrorCallback errorCallback) {
        if (errorCallback == null) {
            Log.e(TAG, "errorCallback == null");
            return;
        }
        mRenderErrorCallback = errorCallback;
    }

    /**
     * Don't response to any touch event.
     *
     * @param e event
     * @return true.
     */
    @Override
    public boolean onTouchEvent(MotionEvent e) {
        return true;
    }

    @Override
    public void setTouchListener(View.OnTouchListener listener) {
        this.setOnTouchListener(listener);
    }

    @Override
    public void setPicPath(String path) {
        mPicPath = path;
    }

    @Override
    public void setPicMode() {
        mTextureType = VideoFormat.IMAGE;
    }

    @Override
    public void setVideoMode(int yuvFormat) {
        mTextureType = yuvFormat;
    }

    /**
     * @param mode mode
     *             Don't need to call this when in VR mode.
     */
    @Override
    public void initDisplayMode(int mode) {
    }

    @Override
    public void changeDisplayMode(int mode) {

    }

    @Override
    public int displayPixelsRenderView(final byte[] y, final byte[] u, final byte[] v, final int
            width, final int height) {
        if (mTextureControllerPtr != 0 && mTextureType == VideoFormat.YUV420P) {

            mGlSurfaceViewEventSignal.setYuvTexture(y, u, v, width, height);
            return 0;
        }
        Log.e(TAG, "Should not call it when texture controller not ready or in picture mode");
        return 1;
    }

    @Override
    public int displayPixelsRenderView(byte[] y, byte[] uv, int width, int height) {
        if (mTextureControllerPtr != 0 && mTextureType == VideoFormat.NV12) {
            mGlSurfaceViewEventSignal.setNV12Texture(y, uv, width, height);
            return 0;
        }
        Log.e(TAG, "Should not call it when texture controller not ready or in picture mode");
        return 1;
    }

    @Override
    public int displayPixelsRenderView(long y, long u, long v, int width, int height) {
        if (mTextureControllerPtr != 0 && mTextureType == VideoFormat.YUV420P) {

            mGlSurfaceViewEventSignal.setYuvTexture(y, u, v, width, height);

            return 0;
        }
        Log.e(TAG, "Should not call it when texture controller not ready or in picture mode");
        return 1;
    }

    @Override
    public int displayPixelsRenderView(long y, long uv, int width, int height) {
        if (mTextureControllerPtr != 0 && mTextureType == VideoFormat.NV12) {

            mGlSurfaceViewEventSignal.setNV12Texture(y, uv, width, height);

            return 0;
        }
        Log.e(TAG, "Should not call it when texture controller not ready or in picture mode");
        return 1;
    }

    /**
     * Log error and do nothing.
     */
    @Override
    public void resetView() {
        Log.e(TAG, "Error call onResetClick view in Binocular vr view!");
    }


    /**
     * @return float[2] angles. angles[0] = Pitch (X axis): [-pi/2, pi/2]
     * angles[1] = Yaw (Y axis): [-pi, pi]
     */
    @Override
    public float[] getRotateZoomValues() {
        return mEulerAngles;
    }

    @Override
    public void setRotateZoomValue(float[] angles) {
        // dangerous don't use it
        // updateRotateAngle(angles);
    }

    @Override
    public void takeScreenshot(IShotCallback shot) {
    }

    @Override
    public void stopRendering() {
        onRendererShutdown();
    }

    @Override
    public void requestRender() {

    }

    @Override
    public void setDisplayCallback(IDisplayCallback displayCallback) {

    }

    /////////////
    // IRenderInteractEvent
    /////////////

    /**
     * @param x the delta x value in screen
     * @param y the delta y value in screen
     */
    @Override
    public void onSpan(float x, float y) {

    }

    @Override
    public void onSensorRotate(float axisX, float axisY) {
    }

    @Override
    public void onZoom(float factor) {

    }

    @Override
    public void onInertia(float x, float y) {

    }

    //////
    // private methods
    /////

    private void initView() {
        mCameraView = new float[16];
        mEulerAngles = new float[3];

        mTextureType = -1;
        projection_flag = 1;

        setEGLConfigChooser(8, 8, 8, 0, 16, 0);
        setEGLContextClientVersion(3);
        setRenderer(this);

        // Hide the back and settings buttons
        findViewById(R.id.ui_back_button).setVisibility(View.GONE);
        findViewById(R.id.ui_settings_button).setVisibility(View.GONE);

        initLookAtMat();

        mGlSurfaceViewEventSignal = new GlSurfaceViewEventSignal();
    }

    private void initLookAtMat() {
        Matrix.setLookAtM(mCameraView, 0, 0.0f, 0.0f, CAMERA_Z,
                0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f);
    }

    private void setPicTexture(String path) {
        BitmapFactory.Options options = new BitmapFactory.Options();
        options.inScaled = false;

        Bitmap bitmap = BitmapFactory.decodeFile(path, options);
        if (bitmap == null) {
            Log.e(TAG, "Bitmap NULL!");
            if (mRenderErrorCallback != null) {
                Throwable throwable = new Throwable("Bitmap null");
                mRenderErrorCallback.errorOccurred(throwable);
            }
            return;
        }

        RenderView.setBitmap(bitmap, mTextureControllerPtr);

        Bitmap logo = WaterMarkerProducer.get(getContext());
        RenderView.setLogoTexture(logo, mTextureControllerPtr);

        bitmap.recycle();
    }

    /**
     * @param angles the eular angles
     *               calculate look ar matrix by rotate angles
     *               and ues it in `onDrawEye()`.
     */
    private void updateRotateAngle(float[] angles) {
        float xRotate = (float) (angles[0] * Math.PI);
        // here 0.4 get from experiment.
        float yRotate = (float) (-1.0f * angles[1] * Math.PI * 0.4f);
        float center_y = (float) Math.sin(xRotate);
        float center_x = (float) (Math.cos(xRotate) * Math.sin(yRotate));
        float center_z = (float) (Math.cos(xRotate) * Math.cos(yRotate));
        // Z axis need reverse, look at -Z.
        Matrix.setLookAtM(mCameraView, 0, 0.0f, 0.0f, 0.0f,
                center_x, center_y, -1.0f * center_z, 0.0f, -1.0f, 0.0f);
    }

    ////////////////
    // Vr functions
    ////////////////

    public static native void updateLookAt(float[] mat, long renderPtr);

    public static native void updateProjection(float[] mat, long renderPtr);

    /**
     * Call textureController->ReActiveTexture() to bind texture again.
     *
     * @param texturePtr pointer of texture controller
     */
    private static native void reactiveTex(long texturePtr);
}
