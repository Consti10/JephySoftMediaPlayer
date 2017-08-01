package com.evomotion.glrenderview.interfaces;

/**
 * Created by zhuyuanxuan on 23/11/2016.
 * gl-render
 */

public interface IRenderInteractEvent {

    /**
     * @param x the delta x value in screen
     * @param y the delta y value in screen
     *          Don't need to do `x / width`
     */
    void onSpan(float x, float y);

    void onSensorRotate(float axisX, float axisY);

    void onZoom(float factor);

    void onInertia(float x, float y);
}
