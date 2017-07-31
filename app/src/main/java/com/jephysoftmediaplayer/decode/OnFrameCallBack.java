package com.jephysoftmediaplayer.decode;

import java.nio.ByteBuffer;

/**
 * Created by jephy on 7/31/17.
 */

public interface OnFrameCallBack {
    void onFrame(ByteBuffer frame);
}
