package com.evomotion.glrenderview;

import android.content.Context;
import android.os.Bundle;
import android.os.Parcelable;
import android.support.annotation.Nullable;
import android.util.AttributeSet;
import android.util.Log;
import android.widget.RelativeLayout;

import com.evomotion.glrenderview.interfaces.IGlRenderView;
import com.evomotion.glrenderview.interfaces.IOnTapListener;
import com.evomotion.glrenderview.interfaces.IRenderErrorCallback;
import com.evomotion.glrenderview.interfaces.IRenderInteractEvent;
import com.evomotion.glrenderview.interfaces.IShotCallback;

/**
 * Created by zhuyuanxuan on 12/12/2016.
 * gl-render
 */

public abstract class AbstractRenderLayout extends RelativeLayout {
    private static final String TAG = AbstractRenderLayout.class.getName();

    public
    @Nullable
    IGlRenderView getView() {
        return mView;
    }

    protected IGlRenderView mView;
    protected InteractController mInteract;
    protected IOnTapListener mTapListener;
    protected IRenderErrorCallback mRenderErrorCallback;
    protected int mMode;
    protected int mSensorMode;
    protected RenderStatus mRenderStatus;

    /**
     * @param context Sure to call {@link #setRenderType(int)} otherwise will not draw anything!
     */
    public AbstractRenderLayout(Context context) {
        super(context);
        initFields();
        Log.w(TAG, "Sure to call {@link #setRenderType(int)} otherwise will not draw anything!");
    }

    /**
     * @param attrs Sure to call {@link #setRenderType(int)} otherwise will not draw anything!
     */
    public AbstractRenderLayout(Context context, AttributeSet attrs) {
        super(context, attrs);
        initFields();
        Log.w(TAG, "Sure to call {@link #setRenderType(int)} otherwise will not draw anything!");
    }

    /**
     * Pause the GlSurfaceView and the interact controller.
     */
    public void onPause() {
        if (mView != null) {
            mView.onPause();
        }

        if (mInteract != null) {
            mInteract.pause();
        }
    }

    public void onResume() {
        if (mView != null) {
            mView.onResume();
        }
        if (mInteract != null) {
            mInteract.resume();
        }
    }

    public void releaseRenderView() {
        if (mView != null) {
            mView.stopRendering();
        }
    }

    @Override
    protected Parcelable onSaveInstanceState() {
        Parcelable superState = super.onSaveInstanceState();
        Bundle bundle = new Bundle();
        bundle.putParcelable("superState", superState);
        RenderStatus renderStatus = getRenderStatus();
        bundle.putParcelable("mRenderStatus", renderStatus);
        bundle.putInt("mMode", mMode);
        return bundle;
    }

    @Override
    protected void onRestoreInstanceState(Parcelable state) {
        if (state instanceof Bundle) {
            Bundle bundle = (Bundle) state;
            int mode = bundle.getInt("mMode");
            setRenderType(mode);
            RenderStatus renderStatus = bundle.getParcelable("mRenderStatus");
            setRenderStatus(renderStatus);
            state = bundle.getParcelable("superState");
        }
        super.onRestoreInstanceState(state);
    }

    /**
     * Call sensor and touch mode setter after call
     * play mode(eg. `setFishEye()`) method,
     * Or the view have not been instanced, set the mode is useless.<br/>
     * In sensor mode, force set screen portrait.
     */
    public void setSensorMode() {
        if (mMode == RenderType.VR) {
            return;
        }

        mSensorMode = InteractController.SENSOR_MODE;

        if (mInteract != null) {
            mInteract.setControlMode(this, InteractController.SENSOR_MODE);
            Log.i(TAG, "Set sensor mode success");
        }
    }

    public void setTouchMode() {
        if (mMode == RenderType.VR) {
            return;
        }

        mSensorMode = InteractController.TOUCH_MODE;

        if (mInteract != null) {
            mInteract.setControlMode(this, InteractController.TOUCH_MODE);
            Log.i(TAG, "Set touch mode success");
        }
    }

    public int getInteractMode() {
        return mSensorMode;
    }

    public void enableGyroscope(boolean enable) {
        if (enable) {
            setSensorMode();
        } else {
            setTouchMode();
        }
    }

    /**
     * Call this to reset camera view and view scale to default.<br/>
     * <b>Attention:</b> when in VR mode or sensor(gyroscope) is on: Don't call this.
     */
    public void reset() {
        if (mView != null) {
            mView.resetView();
        }
    }

    /**
     * @param shotCallback Use your interface implement class instance to handler the
     *                     onScreenshotClick result.
     *                     <p>
     *                     Take screen shot and get result from call back method.
     */
    public void takeScreenShot(IShotCallback shotCallback) {
        mView.takeScreenshot(shotCallback);
    }

    /**
     * @param tapListener implement onSingleTap()
     */
    public void setOnTapListener(IOnTapListener tapListener) {
        mTapListener = tapListener;
        if (mInteract != null) {
            mInteract.setTapListener(tapListener);
        }
    }

    /**
     * @param errorCallback implement errorOccurred(Throwable throwable)
     *                      call it when error occurs
     */
    public void setRenderErrorCallback(IRenderErrorCallback errorCallback) {
        mRenderErrorCallback = errorCallback;
        if (mView != null) {
            mView.setRenderErrorCallback(errorCallback);
        }
    }

    public RenderStatus getRenderStatus() {
        if (mRenderStatus == null) {
            mRenderStatus = new RenderStatus();
        }
        // get latest value
        if (mView != null) {
            float[] array = mView.getRotateZoomValues();
            mRenderStatus.setLatitude(array[0]);
            mRenderStatus.setLongitude(array[1]);
            mRenderStatus.setZoomFactor(array[2]);
        }
        return mRenderStatus;
    }

    public void setRenderStatus(RenderStatus renderStatus) {
        if (renderStatus == null) {
            return;
        }

        this.mRenderStatus = renderStatus;

        if (mView != null) {
            float[] array = new float[3];
            array[0] = renderStatus.getLatitude();
            array[1] = renderStatus.getLongitude();
            array[2] = renderStatus.getZoomFactor();
            mView.setRotateZoomValue(array);
        }
    }

    public int getRenderType() {
        return mMode;
    }

    /**
     * @return public static final int NULL_MODE = -1;
     * public static final int TOUCH_MODE = 1;
     * public static final int SENSOR_MODE = 2;
     */
    public int getSensorMode() {
        return mInteract != null ? mInteract.getControlMode() : InteractController.NULL_MODE;
    }

    public void requestRender() {
        mView.requestRender();
    }

    /////////////
    // protected
    /////////////

    /**
     * Initiate the InteractController instance depend on the mode, will initiate
     * all listeners in the InteractController.
     *
     * @param mode public static final int NULL_MODE = -1;
     *             public static final int TOUCH_MODE = 1;
     *             public static final int SENSOR_MODE = 2;
     *             use gyroscope or listen to all gesture event
     */
    protected void initInteract(int mode) {
        if (InteractController.TOUCH_MODE == mode || InteractController.SENSOR_MODE == mode) {
            mInteract = new InteractController(getContext(), (IRenderInteractEvent) mView);
            Log.i(TAG, "init interactor");
            mInteract.initSensor();
            mInteract.setControlMode(this, mode);
            if (mTapListener != null) {
                mInteract.setTapListener(mTapListener);
            }
        } else {
            Log.e(TAG, "Invalid sensor mode in initInteract(int mode)");
        }
    }

    //////////////
    // Abstract Methods
    /////////////

    /**
     * @param mode refer to class RenderType
     *             int NULL = -1;
     *             int SPHERE = 1;
     *             int LITTLE_PLANET = 2;
     *             int FISHEYE = 3;
     *             int VR = 4;
     */
    public abstract void setRenderType(int mode);

    /**
     * Force set screen portrait.
     */
    protected abstract void setLittlePlanet();

    protected abstract void setFishEye();

    protected abstract void setSphere();

    /**
     * Force set screen landscape.<br/>
     * If in sphere mode with sensor, then set same look at in VR mode.
     */
    protected abstract void setVr();

    ////////////
    // Private
    ////////////

    private void initFields() {
        mMode = RenderType.NULL;
        mSensorMode = InteractController.NULL_MODE;
    }

}
