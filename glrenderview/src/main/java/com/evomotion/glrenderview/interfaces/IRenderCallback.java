package com.evomotion.glrenderview.interfaces;

import com.evomotion.glrenderview.BinocularVrView;

/**
 * Created by zhuyuanxuan on 12/12/2016.
 * gl-render
 */

public interface IRenderCallback {
    void onSurfaceCreated();

    void onSurfaceChanged(int width, int height);

    void onSwitchToVRRender(BinocularVrView vrView);

    void onSurfaceFrameTimeout();
}
