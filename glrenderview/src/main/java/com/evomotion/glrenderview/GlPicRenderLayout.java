package com.evomotion.glrenderview;

import android.content.Context;
import android.util.AttributeSet;
import android.util.Log;
import android.view.View;

import com.evomotion.glrenderview.interfaces.IDisplayCallback;
import com.evomotion.glrenderview.interfaces.IShotCallback;

import junit.framework.Assert;

public class GlPicRenderLayout extends AbstractRenderLayout {

    private static final String TAG = "Photo Layout";
    private Context mContext;
    private String mPath;

    public GlPicRenderLayout(Context context) {
        super(context);
        mContext = context;
    }

    public GlPicRenderLayout(Context context, AttributeSet attrs) {
        super(context, attrs);
        mContext = context;
    }


    /**
     * Set the path of picture file, use it when view is created.
     *
     * @param path path of file
     */
    public void setPicPath(String path) {
        mPath = path;
    }

    public void updatePicPath(String path) {
        this.setPicPath(path);
        if (mView != null) {
            mView.setPicPath(path);
        } else {
            Log.e(TAG, "mView is null!");
        }
    }

    //////
    // Callback
    /////

    public void setDisplayCallback(IDisplayCallback displayCallback) {
        if (mView != null) {
            mView.setDisplayCallback(displayCallback);
        }
    }

    ///////////////
    // Mode Switch
    ///////////////

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
        if (mMode == RenderType.VR) {
            Log.i(TAG, "Change to little pla from VR");
            removeAllViews();
            mMode = RenderType.LITTLE_PLANET;
            initNormalView(mMode);
        } else if (mMode == RenderType.NULL) {
            Log.i(TAG, "init new little planet view");
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
        if (mMode == RenderType.VR) {
            removeAllViews();
            mMode = RenderType.FISHEYE;
            initNormalView(mMode);
        } else if (mMode == RenderType.NULL) {
            Log.i(TAG, "init new fisheye view");
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

        if (mMode == RenderType.VR) {
            // get rotate angles from vr view
            // float[] angles;
            // angles = mView.getRotateZoomValues();

            removeAllViews();

            mMode = RenderType.SPHERE;
            initNormalView(mMode);
            // mView.setRotateZoomValue(angles);
        } else if (mMode == RenderType.NULL) {
            Log.i(TAG, "Init new Sphere view");
            mMode = RenderType.SPHERE;
            initNormalView(mMode);
        } else {
            mMode = RenderType.SPHERE;
            mView.changeDisplayMode(mMode);
        }
    }

    /**
     * If in sphere mode with sensor, then set same look at in VR mode.
     */
    @Override
    protected void setVr() {
        Log.i(TAG, "Set VR mode");

        mMode = RenderType.VR;

        removeAllViews();

        mView = new BinocularVrView(mContext);
        mView.setPicMode();
        if (mPath != null) {
            mView.setPicPath(mPath);
        } else {
            Log.e(TAG, "Error no picture path");
        }

        initInteract(InteractController.TOUCH_MODE);
        mView.setTouchListener(mInteract);
        mView.setRenderErrorCallback(mRenderErrorCallback);

        addView((View) mView);
    }

    @Override
    public void takeScreenShot(IShotCallback shotCallback) {
        if (mView != null) {
            mView.takeScreenshot(shotCallback);
        } else {
            Log.e(TAG, "No view found!");
        }
    }

    ////////
    // private method
    ///////

    private void initNormalView(int mode) {
        Log.i(TAG, "Init normal view || with mode: " + mode);
        mView = new RenderView(mContext);
        mView.initDisplayMode(mode);
        mView.setPicMode();

        if (mRenderErrorCallback != null) {
            mView.setRenderErrorCallback(mRenderErrorCallback);
        }

        // set picture path.
        if (mPath != null) {
            mView.setPicPath(mPath);
        } else {
            Log.e(TAG, "Error no picture path");
        }

        if (mSensorMode != InteractController.NULL_MODE) {
            initInteract(mSensorMode);
        } else {
            initInteract(InteractController.TOUCH_MODE);
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
}
