package com.jephysoftmediaplayer.util;

import android.util.Log;

import com.jephysoftmediaplayer.decode.VideoStamp;

import java.nio.ByteBuffer;

/**
 * Created by jephy on 8/5/17.
 */

public class FrameUtils {

    public static boolean isKeyFrame(ByteBuffer keyFrame) {
        byte[] byteFrame = new byte[keyFrame.remaining()];
        keyFrame.get(byteFrame);

        return VideoStamp.isIDRFrame(byteFrame);
    }
}
