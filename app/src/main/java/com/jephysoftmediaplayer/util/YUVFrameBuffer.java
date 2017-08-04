package com.jephysoftmediaplayer.util;

import android.util.Log;


import java.util.Queue;
import java.util.concurrent.PriorityBlockingQueue;

/**
 * Created by jephy on 7/26/17.
 */

public class YUVFrameBuffer {
    private static final String TAG = "YUVFrameBuffer";
    private static final int DEFAULT_CAPACITY = 10;

    private int capacity;

    private Queue<YUVFrame> yuvFrameQueue;

    public YUVFrameBuffer(int capacity) {
        yuvFrameQueue = new PriorityBlockingQueue<>(capacity);
        this.capacity = capacity;
    }

    public YUVFrameBuffer() {
        this(DEFAULT_CAPACITY);
    }

    public void put(YUVFrame yuvFrame){
        yuvFrameQueue.offer(yuvFrame);
        Log.d(TAG, "添加数据");
        int i = 0;
        for (YUVFrame frame:yuvFrameQueue) {
            Log.d(TAG, i+", 队列中对象：timeStamp: "+frame.getTimestamp());
        }
        Log.d(TAG,yuvFrameQueue.toString());
    }

    public YUVFrame get(){
        return yuvFrameQueue.poll();
    }

    public YUVFrame element(){
        return yuvFrameQueue.element();
    }

    public int size(){
        return yuvFrameQueue.size();
    }

    public int capacity(){
        return capacity;
    }
}
