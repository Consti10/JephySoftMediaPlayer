package com.jephysoftmediaplayer.util;

import junit.framework.Assert;

import org.junit.Test;

import java.nio.ByteBuffer;

/**
 * Created by jephy on 8/5/17.
 */
public class CompressedFrameGroupBufferTest {
    @Test
    public void should_be_to_add_frame_to_frameGroup(){
        CompressedFramePacketBuffer framePacketBuffer = new CompressedFramePacketBuffer(100, 15);
        ByteBuffer byteBuffer = ByteBuffer.wrap(new byte[]{0,1,2,3,4});
        boolean addFrame = framePacketBuffer.addFrame(byteBuffer);
        Assert.assertTrue(addFrame);
    }

    @Test
    public void should_be_able_to_get_frameGroup_as_byte_array_format(){

    }

}