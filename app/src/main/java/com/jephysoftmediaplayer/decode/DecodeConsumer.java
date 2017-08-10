package com.jephysoftmediaplayer.decode;

import android.util.Log;

import com.jephysoftmediaplayer.util.CompressedFramePacketBuffer;

import java.nio.ByteBuffer;
import java.util.Queue;

/**
 * Created by jephy on 8/5/17.
 */

public class DecodeConsumer implements Runnable {
    private final String TAG = "DecodeConsumer";
    private UVCSoftDecoder decoder;
    private CompressedFramePacketBuffer compressedFramePacketBuffer;
    private volatile  boolean stop = false;
    private volatile  boolean  pause = false;
//    private Thread thread;

    public DecodeConsumer(OnDecodeYUVCompeleted yuvConsumer,CompressedFramePacketBuffer compressedFramePacketBuffer) {
        this.decoder = new UVCSoftDecoder(yuvConsumer);
        this.compressedFramePacketBuffer = compressedFramePacketBuffer;
//        thread = new Thread(this);
    }

    public void start(){
        this.pause = false;
        this.stop = false;
        new Thread(this).start();
//        if (!thread.isAlive()) {
//            thread.start();
//        }

    }

    public void pause(){
        this.pause = true;
    }

    public void stop(){
        this.stop = true;
    }

    @Override
    public void run() {
        while (true){
            if (pause) {
                break;
            }
            if (stop){
                compressedFramePacketBuffer.removeAll();
                break;
            }
            if (compressedFramePacketBuffer.size() > 0) {
                Queue<ByteBuffer> frameGroup = compressedFramePacketBuffer.getFrameGroup().getFrameGroup();
                while (!frameGroup.isEmpty()){
                    ByteBuffer frame = frameGroup.poll();
                    frame.flip();
                    byte[] byteFrame = new byte[frame.remaining()];
                    frame.get(byteFrame);
                    Log.d(TAG, "消费解码"+byteFrame.length);
                    decoder.decode(byteFrame);
                }
            }

        }
    }

}
