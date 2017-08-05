package com.jephysoftmediaplayer.decode;

import com.jephysoftmediaplayer.util.CompressedFramePacketBuffer;

import java.nio.ByteBuffer;
import java.util.Queue;

/**
 * Created by jephy on 8/5/17.
 */

public class DecodeConsumer implements Runnable {

    private UVCSoftDecoder decoder;
    private CompressedFramePacketBuffer compressedFramePacketBuffer;

    public DecodeConsumer(OnDecodeYUVCompeleted yuvConsumer,CompressedFramePacketBuffer compressedFramePacketBuffer) {
        this.decoder = new UVCSoftDecoder(yuvConsumer);
        this.compressedFramePacketBuffer = compressedFramePacketBuffer;
    }

    @Override
    public void run() {
        while (true){
            if (compressedFramePacketBuffer.size() > 0) {
                Queue<ByteBuffer> frameGroup = compressedFramePacketBuffer.getFrameGroup().getFrameGroup();
                while (!frameGroup.isEmpty()){
                    ByteBuffer frame = frameGroup.poll();
                    byte[] byteFrame = new byte[frame.remaining()];
                    frame.get(byteFrame);
                    decoder.decode(byteFrame);
                }
            }

        }
    }

}
