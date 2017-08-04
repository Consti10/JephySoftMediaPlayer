package com.jephysoftmediaplayer.datasource;

import java.nio.ByteBuffer;

/**
 * Created by jephy on 8/1/17.
 */

public interface OnFrameCallback {
    void onFrameCallBack(ByteBuffer frame);
}
