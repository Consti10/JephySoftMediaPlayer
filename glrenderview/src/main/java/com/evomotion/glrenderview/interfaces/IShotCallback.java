package com.evomotion.glrenderview.interfaces;

import android.graphics.Bitmap;

/**
 * Created by zhuyuanxuan on 09/12/2016.
 * gl-render
 */

public interface IShotCallback {
    /**
     * @param bmp on screenshot success, will return the bitmap,
     *            should implement this method to save the screen shot result.
     */
    void onScreenShotSuccess(Bitmap bmp);
}
