package com.jephysoftmediaplayer.mock;

import com.jephysoftmediaplayer.decode.OnFrameCallback;

import java.nio.ByteBuffer;

/**
 * Created by jephy on 7/19/17.
 */

public interface Data {
    void send(ByteBuffer byteBuffer);
    void register(OnFrameCallback observer);
    void unRegister(OnFrameCallback observer);
}
