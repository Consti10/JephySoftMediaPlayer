package com.evomotion.glrenderview;

import android.os.Handler;
import android.os.Message;

import java.lang.ref.WeakReference;

/**
 * Created by zhuyuanxuan on 09/12/2016.
 * gl-render
 */

public class GlRenderHandler extends Handler {
    public static final int FINISH = 1;

    private final WeakReference<RenderView> mRender;

    public GlRenderHandler(RenderView render) {
        mRender = new WeakReference<>(render);
    }

    /**
     * Subclasses must implement this to receive messages.
     *
     * @param msg message
     */
    @Override
    public void handleMessage(Message msg) {
        RenderView samplerRender = mRender.get();

        if (samplerRender != null) {
            switch (msg.what) {
                case FINISH:
                    samplerRender.finishShot();
                    break;
                default:
                    break;
            }
        }
    }
}
