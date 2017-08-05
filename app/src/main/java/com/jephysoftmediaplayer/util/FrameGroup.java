package com.jephysoftmediaplayer.util;

import java.nio.ByteBuffer;
import java.util.Queue;
import java.util.concurrent.ArrayBlockingQueue;

/**
 * Created by jephy on 8/5/17.
 */

public class FrameGroup {
    private Queue<ByteBuffer> frameGroup;
    private int frameGroupMaxSize;
    public FrameGroup(int frameGroupMaxSize) {
        this.frameGroupMaxSize = frameGroupMaxSize;
        this.frameGroup = new ArrayBlockingQueue<ByteBuffer>(frameGroupMaxSize);
    }

    public boolean addFrame(ByteBuffer frame){
        if (frameGroup.size() < frameGroupMaxSize) {
            frameGroup.add(frame);
            return true;
        }
        return false;
    }

    public ByteBuffer getFrame(){
        return frameGroup.poll();
    }

//    public int remain(){
//        return frameGroup.size();
//    }

    public int size(){
        return frameGroup.size();
    }

    public  Queue<ByteBuffer> getFrameGroup(){
        return frameGroup;
    }
}
