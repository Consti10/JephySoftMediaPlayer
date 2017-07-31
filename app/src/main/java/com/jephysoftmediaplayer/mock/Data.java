package com.jephysoftmediaplayer.mock;

import com.jephysoftmediaplayer.decode.OnFrameCallBack;

import java.nio.ByteBuffer;

/**
 * Created by jephy on 7/19/17.
 */

public interface Data {
    void send(ByteBuffer byteBuffer);
    void register(OnFrameCallBack observer);
    void unRegister(OnFrameCallBack observer);
}
