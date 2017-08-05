package com.jephysoftmediaplayer.util;

import org.junit.Test;

import java.nio.ByteBuffer;

import static org.junit.Assert.*;

/**
 * Created by jephy on 8/5/17.
 */
public class FrameUtilsTest {
    @Test
    public void should_be_able_to_tell_key_frame(){
        ByteBuffer keyFrame = ByteBuffer.wrap(new byte[]{0, 0, 0, 1});
        ByteBuffer pFrame = ByteBuffer.wrap(new byte[]{0, 0, 0, 0});
//        boolean expectedIsKeyFrame  = FrameUtils.isKeyFrame(keyFrame);
//        boolean expectedNOTKeyFrame = FrameUtils.isKeyFrame(pFrame);
//        assertTrue(expectedIsKeyFrame);
//        assertFalse(expectedNOTKeyFrame);
    }

}