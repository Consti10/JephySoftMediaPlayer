package com.evomotion.glrenderview;

import android.content.Context;
import android.util.AttributeSet;
import android.util.Log;
import android.view.View;

import com.evomotion.glrenderview.interfaces.IRenderCallback;

import junit.framework.Assert;

/**
 * Created by zhuyuanxuan on 25/11/2016.
 * gl-render
 */

public class GlVideoRenderLayout extends AbstractRenderLayout {
    private static final String TAG = GlVideoRenderLayout.class.getName();
    private IRenderCallback mRenderCallback;
    private GlVideoRenderLayoutCallback callback;

    public void setCallback(GlVideoRenderLayoutCallback callback) {
        this.callback = callback;
    }

    public GlVideoRenderLayout(Context context) {
        super(context);
    }

    public GlVideoRenderLayout(Context context, AttributeSet attrs) {
        super(context, attrs);
    }

    /////////
    // implement abstract methods
    ////////

    /**
     * @param mode refer to class RenderType
     *             int NULL = -1;
     *             int SPHERE = 1;
     *             int LITTLE_PLANET = 2;
     *             int FISHEYE = 3;
     *             int VR = 4;
     */
    @Override
    public void setRenderType(int mode) {
        Assert.assertNotSame(RenderType.NULL, mode);
        switch (mode) {
            case RenderType.SPHERE:
                setSphere();
                break;
            case RenderType.LITTLE_PLANET:
                setLittlePlanet();
                break;
            case RenderType.FISHEYE:
                setFishEye();
                break;
            case RenderType.VR:
                setVr();
                break;
            default:
                break;
        }
    }

    @Override
    protected void setLittlePlanet() {
        if (mMode == RenderType.LITTLE_PLANET) {
            return;
        }
        if (mMode == RenderType.NULL) {
            Log.i(TAG, "init new little planet view");
            mMode = RenderType.LITTLE_PLANET;
            initNormalView(mMode);
        } else if (mMode == RenderType.VR) {
            Log.i(TAG, "Change to little pla from VR");
            removeAllViews();

            mMode = RenderType.LITTLE_PLANET;
            initNormalView(mMode);
        } else {
            Log.i(TAG, "Change to little pla (non VR)");
            mMode = RenderType.LITTLE_PLANET;
            mView.changeDisplayMode(mMode);
        }
    }

    @Override
    protected void setFishEye() {
        if (mMode == RenderType.FISHEYE) {
            return;
        }

        if (mMode == RenderType.NULL) {
            Log.i(TAG, "init new fisheye view");
            mMode = RenderType.FISHEYE;
            initNormalView(mMode);
        } else if (mMode == RenderType.VR) {
            removeAllViews();

            mMode = RenderType.FISHEYE;
            initNormalView(mMode);
        } else {
            mMode = RenderType.FISHEYE;
            mView.changeDisplayMode(mMode);
        }
    }

    @Override
    protected void setSphere() {
        if (mMode == RenderType.SPHERE) {
            return;
        }

        if (mMode == RenderType.NULL) {
            Log.i(TAG, "Init new Sphere view");
            mMode = RenderType.SPHERE;
            initNormalView(mMode);
        } else if (mMode == RenderType.VR) {
            removeAllViews();
            mMode = RenderType.SPHERE;
            initNormalView(mMode);
        } else {
            mMode = RenderType.SPHERE;
            mView.changeDisplayMode(mMode);
        }
    }

    @Override
    protected void setVr() {
        Log.i(TAG, "Set VR mode sensor mode is " + mSensorMode);
        mMode = RenderType.VR;
        mSensorMode = InteractController.TOUCH_MODE;
        removeAllViews();
        mInteract.pause();

        BinocularVrView vrView = new BinocularVrView(getContext());


        if (this.callback != null) {
            vrView.setVideoMode(callback.videoFrameFormat());
        } else { //default
            vrView.setVideoMode(VideoFormat.NV12);
        }

        vrView.setRenderCallback(mRenderCallback);

        vrView.setRenderErrorCallback(mRenderErrorCallback);

        // don't response to touch event, don't need to set listener
        if (mRenderCallback != null) {
            // add the view in the callback
            mRenderCallback.onSwitchToVRRender(vrView);
        }
        mView = vrView;
    }

    public void setRenderCallback(IRenderCallback renderCallback) {
        this.mRenderCallback = renderCallback;
        if (mView != null) {
            mView.setRenderCallback(renderCallback);
        }
    }

    /**
     * Must call it in VR mode!
     *
     * @param enable if true, will set Interactor attached to vr view
     * @return 0 if success.
     */
    public int setVrInteractor(boolean enable) {
        if (mMode != RenderType.VR) {
            return 1;
        }

        if (enable) {
            initInteract(InteractController.TOUCH_MODE);
            mView.setTouchListener(mInteract);
            return 0;
        }
        return 1;
    }

    public int displayPixels(byte[] y, byte[] u, byte[] v, int width, int height) {
        if (mView != null) {
            return mView.displayPixelsRenderView(y, u, v, width, height);
        }
        return -1;
    }

    public int displayPixels(byte[] y, byte[] uv, int width, int height) {
        if (mView != null) {
            return mView.displayPixelsRenderView(y, uv, width, height);
        }
        return -1;
    }

    public int displayPixels(long y, long u, long v, int width, int height) {
        if (mView != null) {
            return mView.displayPixelsRenderView(y, u, v, width, height);
        }
        return -1;
    }

    public int displayNV12Pixels(long y, long uv, int width, int height) {
        if (mView != null) {
            return mView.displayPixelsRenderView(y, uv, width, height);
        }

        return -1;
    }

    //////////
    // Private
    //////////

    private void initNormalView(int mode) {
        Log.i(TAG, "Init normal view with mode: " + mode);
        mView = new RenderView(getContext());

        if (this.callback != null) {
            mView.setVideoMode(callback.videoFrameFormat());
        } else { //default
            mView.setVideoMode(VideoFormat.NV12);
        }
        /*// decide the yuv format
        if (AppInfo.mediacodecSupport()) {
            mView.setVideoMode(RenderType.NV12);
        } else {
            mView.setVideoMode(RenderType.YUV420P);
        }*/

        mView.initDisplayMode(mode);
        if (mRenderCallback != null) {
            mView.setRenderCallback(mRenderCallback);
        }
        if (mRenderErrorCallback != null) {
            mView.setRenderErrorCallback(mRenderErrorCallback);
        }

        if (mSensorMode != InteractController.NULL_MODE) {
            initInteract(mSensorMode);
        } else {
            initInteract(InteractController.TOUCH_MODE);
            // set last status value
            if (mRenderStatus != null) {
                float[] array = new float[3];
                array[0] = mRenderStatus.getLatitude();
                array[1] = mRenderStatus.getLongitude();
                mView.setRotateZoomValue(array);
            }
        }

        mView.setTouchListener(mInteract);

        addView((View) mView);
    }

    public interface GlVideoRenderLayoutCallback {
        int videoFrameFormat();
    }
}
