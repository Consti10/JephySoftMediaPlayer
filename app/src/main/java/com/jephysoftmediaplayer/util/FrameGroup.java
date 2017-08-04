package com.jephysoftmediaplayer.util;

import android.util.Log;


import com.jephysoftmediaplayer.decode.VideoStamp;

import java.util.Queue;
import java.util.concurrent.ArrayBlockingQueue;

/**
 * Created by jephy on 7/24/17.
 */

public class FrameGroup {
    private static final int DEFAULT_CAPACITY = 20;
    private static final String TAG = "FrameGroup";
    private int capacity;
    private Queue<byte[]> queue;

    public FrameGroup(int capacity) {
        queue = new ArrayBlockingQueue<byte[]>(capacity);
        this.capacity = capacity;
    }

    public FrameGroup(){
        this(DEFAULT_CAPACITY);
    }

    public void put(byte[] data){
        queue.add(data);
    }

    public byte[] get(){
        return queue.poll();
    }

    public void clear(){
        queue.clear();
    }

    public static boolean isIDRFrame(byte[] frame){

        boolean isIDRFrame = VideoStamp.isIDRFrame(frame);
        Log.d(TAG, "flag = " + isIDRFrame);
        if (isIDRFrame){
            return true;
        }
        return false;
    }

    public int size() {
        return queue.size();
    }

    public int getCapacity() {
        return capacity;
    }
}
