package com.jephysoftmediaplayer.decode;

/**
 * Created by jephy on 7/20/17.
 */

public interface OnDecodeYUVCompeleted {
    void onDecodeYUVCompeleted(byte[] yData, byte[] uData, byte[] vData, int width, int height, long timeStamp);
}
