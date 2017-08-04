package com.jephysoftmediaplayer.util;

import android.util.Log;

import java.util.Queue;
import java.util.concurrent.ArrayBlockingQueue;

/**
 * Created by jephy on 7/24/17.
 */

public class FrameGroupQueueRepository {
    public static final int DEFAULT_CAPACITY = 6;
    private static final String TAG = "FrameGroupRepository";
    private Queue<FrameGroup> frameGroups;

    public FrameGroupQueueRepository(int capacity){
        frameGroups = new ArrayBlockingQueue<FrameGroup>(capacity);
    }

    public FrameGroupQueueRepository(){
        this(DEFAULT_CAPACITY);
    }

    public FrameGroup put(FrameGroup frameGroup){
        try{
            frameGroups.add(frameGroup);
        }catch (IllegalStateException e){
            frameGroups.clear();
            Log.e(TAG, "解不过来，丢掉缓存帧组");
            frameGroups.add(frameGroup);
        }

        return frameGroup;
    }

    public FrameGroup get(){
        FrameGroup polledFrrameGroup = frameGroups.poll();
        return polledFrrameGroup;
    }

    public int size(){
        return frameGroups.size();
    }
}
