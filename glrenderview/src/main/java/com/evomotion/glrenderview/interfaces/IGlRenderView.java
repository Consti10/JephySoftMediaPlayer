package com.evomotion.glrenderview.interfaces;

import android.view.View;

/**
 * Created by zhuyuanxuan on 02/12/2016.
 * gl-render
 */

public interface IGlRenderView {
    void setRenderCallback(IRenderCallback renderCallback);

    void setDisplayCallback(IDisplayCallback displayCallback);

    void setRenderErrorCallback(IRenderErrorCallback errorCallback);

    void onPause();

    void onResume();

    void setTouchListener(View.OnTouchListener listener);

    void setPicPath(String path);

    void setPicMode();

    /**
     * Tell render view render video
     *
     * @param yuvFormat public static final int IMAGE = 1;
     *                  public static final int VIDEO = 2;
     *                  public static final int YUV420P = 3;
     *                  public static final int NV12 = 4;
     */
    void setVideoMode(int yuvFormat);

    void initDisplayMode(int mode);

    void changeDisplayMode(int mode);

    int displayPixelsRenderView(byte[] y, byte[] u, byte[] v, int width, int height);

    int displayPixelsRenderView(byte[] y, byte[] uv, int width, int height);

    int displayPixelsRenderView(long y, long u, long v, int width, int height);

    int displayPixelsRenderView(long y, long uv, int width, int height);

    /**
     * Don't call this when in VR mode.
     */
    void resetView();

    float[] getRotateZoomValues();

    void setRotateZoomValue(float[] angles);

    void takeScreenshot(IShotCallback shot);

    void stopRendering();

    void requestRender();
}
