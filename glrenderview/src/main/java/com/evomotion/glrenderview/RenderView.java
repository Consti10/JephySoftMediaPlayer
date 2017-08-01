package com.evomotion.glrenderview;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.opengl.GLES30;
import android.opengl.GLSurfaceView;
import android.os.Message;
import android.util.AttributeSet;
import android.util.Log;

import com.evomotion.glrenderview.interfaces.IDisplayCallback;
import com.evomotion.glrenderview.interfaces.IGlRenderView;
import com.evomotion.glrenderview.interfaces.IRenderCallback;
import com.evomotion.glrenderview.interfaces.IRenderErrorCallback;
import com.evomotion.glrenderview.interfaces.IRenderInteractEvent;
import com.evomotion.glrenderview.interfaces.IShotCallback;

import java.lang.ref.WeakReference;
import java.nio.ByteBuffer;
import java.util.Timer;
import java.util.TimerTask;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

/**
 * Created by zhuyuanxuan on 16/11/2016.
 * gl-render
 */

public class RenderView extends GLSurfaceView implements IRenderInteractEvent, IGlRenderView {
    // load jni library
    static {
        System.loadLibrary("glrender");
    }

    private static final String TAG = "RenderView.java";
    private InnerRenderer mRender;
    private byte[] mByteArray;
    private IShotCallback mShotCallback;
    private GlRenderHandler mHandler;

    private int mMode;
    private int mTextureType;
    private Timer timer = new Timer();

    private IRenderCallback mRenderCallback;
    private GlSurfaceViewEventSignal mGlViewEventSignal;

    public RenderView(Context context) {
        super(context);
        initParam();
        initGl();
        Log.i(TAG, "init render view finish");
    }

    public RenderView(Context context, AttributeSet attrs) {
        super(context, attrs);
        initParam();
        initGl();
    }

    /////////////////
    // Lifecycle
    ////////////////

    @Override
    public void onPause() {
        super.onPause();
        Log.i(TAG, "On Pause");
    }

    @Override
    public void onResume() {
        super.onResume();
    }

    @Override
    public void stopRendering() {
        destroy(mRender.mRenderControllerPtr, mRender.mTextureControllerPtr, mRender.mAnimationPtr);
    }

    ///////////////////////
    // Implement GlRenderView methods
    //////////////////////

    @Override
    public void setRenderCallback(IRenderCallback renderCallback) {
        this.mRenderCallback = renderCallback;
    }

    @Override
    public void setRenderErrorCallback(IRenderErrorCallback errorCallback) {
        mRender.setRendererErrorCallback(errorCallback);
    }


    @Override
    public void setTouchListener(OnTouchListener listener) {
        this.setOnTouchListener(listener);
    }

    /**
     * set picture path and read picture,
     * call native function to set texture
     *
     * @param path the path of picture
     */
    @Override
    public void setPicPath(String path) {
        mRender.setPicPath(path);
    }

    @Override
    public void setPicMode() {
        mTextureType = VideoFormat.IMAGE;
    }

    @Override
    public void setVideoMode(int yuvFormat) {
        mTextureType = yuvFormat;
    }

    @Override
    public void initDisplayMode(int mode) {
        mMode = mode;
    }

    @Override
    public void changeDisplayMode(int mode) {
        mRender.setChangeMode(true, mode);
        requestRender();
    }

    @Override
    public int displayPixelsRenderView(byte[] y, byte[] u, byte[] v, int width, int height) {
        if (mTextureType != VideoFormat.YUV420P) {
            Log.e(TAG, "Error video mode error your mode is not YUV420P is: " + mTextureType);
            return -1;
        }

        mGlViewEventSignal.setYuvTexture(y, u, v, width, height);
        requestRender();
        return 0;
    }

    @Override
    public int displayPixelsRenderView(byte[] y, byte[] uv, int width, int height) {
        if (mTextureType != VideoFormat.NV12) {
            Log.e(TAG, "Error video mode error your mode is not NV12 is: " + mTextureType);
            return -1;
        }

        mGlViewEventSignal.setNV12Texture(y, uv, width, height);
        requestRender();
        return 0;
    }

    @Override
    public int displayPixelsRenderView(long y, long u, long v, int width, int height) {
        if (mTextureType != VideoFormat.YUV420P) {
            Log.e(TAG, "Error video mode error your mode is not YUV420P is: " + mTextureType);
            return -1;
        }

        mGlViewEventSignal.setYuvTexture(y, u, v, width, height);
        requestRender();
        return 0;
    }

    @Override
    public int displayPixelsRenderView(long y, long uv, int width, int height) {
        if (mTextureType != VideoFormat.NV12) {
            Log.e(TAG, "Error video mode error your mode is not NV12 is: " + mTextureType);
            return -1;
        }

        mGlViewEventSignal.setNV12Texture(y, uv, width, height);
        requestRender();
        return 0;
    }

    @Override
    public void resetView() {
        resetViewNative(mRender.mRenderControllerPtr, mRender.mAnimationPtr);
        setRenderMode(RENDERMODE_CONTINUOUSLY);
    }

    /**
     * @return a float array [m_latitude, m_longitude, m_zoom_factor]
     */
    @Override
    public float[] getRotateZoomValues() {
        return getRenderStatusNative(mRender.mRenderControllerPtr);
    }

    /**
     * @param angles float array [m_latitude, m_longitude, m_zoom_factor]
     */
    @Override
    public void setRotateZoomValue(float[] angles) {
        mRender.setRotateZoomValue(angles);
        requestRender();
    }

    public void takeScreenshot(IShotCallback shot) {
        int width = getWidth();
        int height = getHeight();
        int size = width * height * 4;
        mByteArray = new byte[size];
        mRender.startReadPixel(mByteArray, mHandler);

        mShotCallback = shot;
    }

    ////////////
    // Implement interface IRenderInteractEvent
    ////////////

    /**
     * calculate delta_x / width then call native function
     */
    @Override
    public void onSpan(float x, float y) {
        synchronized (this) {
            onSpanXYNative(x / this.getWidth(), y / this.getHeight(), mRender.mRenderControllerPtr);
        }
        requestRender();
    }

    @Override
    public void onSensorRotate(float axisX, float axisY) {
        synchronized (this) {
            onSensorRotateXYNative(axisX, axisY, mRender.mRenderControllerPtr);
            requestRender();
        }
    }

    @Override
    public synchronized void onZoom(float factor) {
        onZoomNative(factor, mRender.mRenderControllerPtr);
        requestRender();
    }

    public static void onZoom(float factor, long renderControllerPtr) {
        onZoomNative(factor, renderControllerPtr);
    }

    @Override
    public void onInertia(float x, float y) {
        synchronized (this) {
            onInertiaXYNative(x / this.getWidth(), y / this.getHeight(),
                    mRender.mRenderControllerPtr, mRender.mAnimationPtr);
        }
        setRenderMode(RENDERMODE_CONTINUOUSLY);
    }

    public void finishShot() {
        Bitmap bmp = Bitmap.createBitmap(getWidth(), getHeight(), Bitmap.Config.ARGB_8888);
        bmp.copyPixelsFromBuffer(ByteBuffer.wrap(mByteArray));
        mShotCallback.onScreenShotSuccess(bmp);
    }

    ///////////////
    // private methods
    ///////////////

    private void initParam() {
        mTextureType = RenderType.NULL;
        mMode = RenderType.NULL;
        mHandler = new GlRenderHandler(this);
        mGlViewEventSignal = new GlSurfaceViewEventSignal();
    }

    private void initGl() {
        //set EGLConfig
        setEGLContextClientVersion(3);
        setEGLConfigChooser(8, 8, 8, 8, 16, 0);
        mRender = new InnerRenderer();
        setRenderer(mRender);
        // Render the view only when there is a change in the drawing data
        setRenderMode(GLSurfaceView.RENDERMODE_WHEN_DIRTY);
    }

    public void setDisplayCallback(IDisplayCallback displayCallback) {
        if (mRender != null) {
            mRender.setDisplayCallback(displayCallback);
        }
    }

    private WeakReference<RenderView> weakSelf() {
        return new WeakReference<>(this);
    }

    /**
     * InnerRenderer
     */
    private class InnerRenderer implements Renderer {
        // Should be same as defined in the `BaseRenderController.h`
        // enum RenderAnimationDrawingStatus {
        //   RENDER_NULL_STATUS,
        //   RENDER_DRAWING,
        //   RENDER_INERTIA,
        //   RENDER_RESET,
        //   RENDER_TRANSFORM
        // };
        private static final int RENDER_DRAWING = 1;

        private IRenderErrorCallback mRendererErrorCallback;
        private String mPicPath;
        private boolean mUpdatePath = false;
        private boolean mIfChangeMode = false;
        private boolean mIfChangeRotateAngle = false;
        private boolean mScreenshot = false;
        private int mNextMode = -1;
        private float[] mRotateZoomFactors;
        private byte[] mByteArray;
        private GlRenderHandler mHandler;
        private int mRenderStatus;
        // pointers in c++ code
        private long mRenderControllerPtr;
        private long mTextureControllerPtr;
        private long mAnimationPtr;
        private IDisplayCallback mDisplayCallback;

        @Override
        public void onSurfaceCreated(GL10 gl10, EGLConfig eglConfig) {
            if (mTextureType != RenderType.NULL && mMode != RenderType.NULL) {
                long[] pointers = surfaceCreated(mTextureType, mMode);
                if (pointers != null) {
                    mRenderControllerPtr = pointers[0];
                    mTextureControllerPtr = pointers[1];
                    mAnimationPtr = pointers[2];
                    // Log.i(TAG, "Get method pointer" + pointers[0] + "::" + pointers[1] + " 3rd: "
                    //         + pointers[2]);
                } else {
                    Log.e(TAG, "Can't get pointers");
                }
            } else {
                Log.e(TAG, "Error! texture type OR mode unknown!");
            }

            if (mTextureType > VideoFormat.IMAGE) {
                // set logo texture
                BitmapFactory.Options options = new BitmapFactory.Options();
                options.inScaled = false;
                Bitmap logo = WaterMarkerProducer.get(getContext());

                if (logo == null) {
                    Log.e(TAG, "Logo null");
                    return;
                }
                setLogoTexture(logo, mTextureControllerPtr);

                if (mRenderCallback != null) {
                    mRenderCallback.onSurfaceCreated();
                }
            }
        }

        @Override
        public void onSurfaceChanged(GL10 gl10, int width, int height) {
            surfaceChanged(width, height, mRenderControllerPtr, mTextureControllerPtr);
            if (mPicPath != null) {
                setPicTexture(mPicPath);
                mUpdatePath = false;
                if (mDisplayCallback != null) {
                    mDisplayCallback.onDisplayed();
                }
            }

            if (mIfChangeRotateAngle) {
                setRenderStatusNative(mRotateZoomFactors, mRenderControllerPtr);
                mIfChangeRotateAngle = false;
            }
        }

        /**
         * @param gl10 GL10
         *             Here judge if need change mode, if true and mode value valid
         *             then call changeMode method.
         *             And judge if need update file path, if true, update.
         */
        @Override
        public void onDrawFrame(GL10 gl10) {
            // Log.e(TAG, "DRAW");
            if (mIfChangeMode && (mNextMode > -1)) {
                synchronized (RenderView.this) {
                    long[] pointers = {mRenderControllerPtr, mTextureControllerPtr, mAnimationPtr};
                    changeMode(mNextMode, pointers);
                    setRenderMode(RENDERMODE_CONTINUOUSLY);
                }  // synchronized
                mMode = mNextMode;
                mIfChangeMode = false;
                mNextMode = -1;
            }

            if (mUpdatePath) {
                Log.i(TAG, "Update picture path");
                if (mPicPath != null) {
                    setPicTexture(mPicPath);
                } else {
                    Log.e(TAG, "picture path null! ");
                }
                mUpdatePath = false;
                if (mDisplayCallback != null) {
                    mDisplayCallback.onDisplayed();
                }
            }

            if (mScreenshot) {
                // If take a screen shot, then ignore this frame
                mScreenshot = false;
                int result = readPixelsNative(mByteArray, mRenderControllerPtr);
                if (result == 0) {
                    Message msg = Message.obtain();
                    msg.what = GlRenderHandler.FINISH;
                    mHandler.sendMessage(msg);
                    Log.i(TAG, "screen shot finish, flag is: " + mScreenshot);
                } else {
                    if (mRendererErrorCallback != null) {
                        mRendererErrorCallback.errorOccurred(new Throwable("Error in NDK" +
                                "read pixels"));
                    }
                    Log.e(TAG, "Error in NDK read pixels");
                }
            }

            // Here do draw.
            if (mTextureType > VideoFormat.IMAGE) {
                gl10.glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
                gl10.glClear(GLES30.GL_COLOR_BUFFER_BIT);

                if (mGlViewEventSignal.needUpdateYuvTexture()) {
                    if (mTextureControllerPtr == 0) {
                        Log.e(TAG, "Should not render yuv pixels when texture controller not " +
                                "ready ");
                        return;
                    }

                    setTimeoutDetect(5000);

                    mGlViewEventSignal.useTextureData(mTextureControllerPtr);
                }
            }

            mRenderStatus = drawFrame(mRenderControllerPtr, mTextureControllerPtr);
            // get render status every draw event
            if (mRenderStatus == RENDER_DRAWING) {
                // change mode finished now drawing
                // draw again when switch to another mode
                mRenderStatus = drawFrame(mRenderControllerPtr, mTextureControllerPtr);
                // after switched set back
                setRenderMode(RENDERMODE_WHEN_DIRTY);
            }  // mRenderStatus ok
        }

        void setRendererErrorCallback(IRenderErrorCallback errorCallback) {
            if (errorCallback == null) {
                Log.e(TAG, "errorCallback == null");
                return;
            }
            mRendererErrorCallback = errorCallback;
        }

        void setDisplayCallback(IDisplayCallback displayCallback) {
            mDisplayCallback = displayCallback;
        }

        void setPicPath(String path) {
            mPicPath = path;
            mUpdatePath = true;
        }

        void setChangeMode(boolean mIfChangeMode, int mode) {
            this.mIfChangeMode = mIfChangeMode;
            this.mNextMode = mode;
        }

        void setRotateZoomValue(float[] array) {
            mRotateZoomFactors = array;
            mIfChangeRotateAngle = true;
        }

        void startReadPixel(byte[] bytes, GlRenderHandler handler) {
            mScreenshot = true;
            mByteArray = bytes;
            mHandler = handler;
            requestRender();
        }

        /////////////////
        // private method
        /////////////////

        private void setPicTexture(String path) {
            BitmapFactory.Options options = new BitmapFactory.Options();
            options.inScaled = false;

            Log.i(TAG, "Photo path: " + path);
            Bitmap bitmap = BitmapFactory.decodeFile(path, options);
            if (bitmap == null) {
                Log.e(TAG, "Bitmap NULL!");
                if (mRendererErrorCallback != null) {
                    Throwable throwable = new Throwable("Bitmap null");
                    mRendererErrorCallback.errorOccurred(throwable);
                }
                return;
            }
            setBitmap(bitmap, mTextureControllerPtr);

            // logo
            Bitmap logo = WaterMarkerProducer.get(getContext());

            if (logo == null) {
                Log.e(TAG, "Logo null");
                return;
            }

            setLogoTexture(logo, mTextureControllerPtr);

            bitmap.recycle();
            logo.recycle();
        }

        private void setTimeoutDetect(long timeDelay) {
            if (timer != null) {
                timer.cancel();
            }

            timer = new Timer();
            final WeakReference<RenderView> weakSelf = weakSelf();
            timer.schedule(new TimerTask() {
                @Override
                public void run() {
                    if (weakSelf.get() != null && weakSelf.get().mRenderCallback != null) {
                        weakSelf.get().mRenderCallback.onSurfaceFrameTimeout();
                    }
                }
            }, timeDelay);
        }

    }

    ///////
    // Native JNI method
    ///////

    /**
     * @param textureType enum TextureType {
     *                    NULL_TYPE,
     *                    IMAGE,
     *                    VIDEO
     *                    YUV420P,
     *                    NV12,
     *                    NV21
     *                    };
     * @param mode        enum DisplayMode {NULL_MODE,
     *                    SPHERE,
     *                    LITTLE_PLANET,
     *                    FISHEYE,
     *                    VR};
     * @return pointer of BaseRenderController and BaseTextureController
     * [BaseRenderController *, BaseTextureController *]
     */
    public static native long[] surfaceCreated(int textureType, int mode);

    public static native void surfaceChanged(int width, int height, long renderPtr, long
            textureControllerPtr);

    public static native int drawFrame(long renderPtr, long texPtr);

    public static native void destroy(long renderPtr, long textureControllerPtr, long animatePtr);

    // Gesture functions
    private static native void onSpanXYNative(float x, float y, long renderPtr);

    private static native void onSensorRotateXYNative(float x, float y, long renderPtr);

    private static native void onInertiaXYNative(float x, float y, long renderPtr, long
            animatePtr);

    private static native void onZoomNative(float factor, long renderPtr);

    /**
     * @param params    float array [m_latitude, m_longitude, m_zoom_factor]
     * @param renderPtr pointer
     */
    public static native void setRenderStatusNative(float[] params, long renderPtr);

    /**
     * @param renderPtr pointer
     * @return a float array [m_latitude, m_longitude, m_zoom_factor]
     */
    public static native float[] getRenderStatusNative(long renderPtr);

    private static native void resetViewNative(long renderPtr, long animatePtr);

    // Bitmap transfer
    public static native void setBitmap(Bitmap bmp, long texPtr);

    public static native void setLogoTexture(Bitmap logo, long texPtr);

    private static native void setBitmapAndLogo(Bitmap bmp, Bitmap logo, long texPtr);

    /**
     * @param mode     the same as surfaceCreated
     * @param pointers [BaseRenderController *, BaseTextureController *]
     */
    public static native void changeMode(int mode, long[] pointers);

    // screen shot
    public static native int readPixelsNative(byte[] bytes, long renderPtr);

    public static native int nativeDisplayYuv420pPixelBytes(byte[] y, byte[] u, byte[] v, int
            width, int
                                                                    height, long texPtr);

    public static native int nativeDisplayNV12PixelBytes(byte[] y, byte[] uv, int width, int
            height, long texPtr);

    public static native int nativeDisplayYuv420pPixel(long y, long u, long v, int width, int
            height, long texPtr);

    public static native int nativeDisplayNV12Pixel(long y, long uv, int width, int height, long
            texturePtr);
}
