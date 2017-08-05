package com.jephysoftmediaplayer.util;

import android.util.Log;

import java.nio.ByteBuffer;
import java.util.Queue;
import java.util.concurrent.ArrayBlockingQueue;

/**
 * Created by jephy on 8/5/17.
 */

/**
 * 压缩数据帧组缓存器
 * 其数据来源是播放器的帧，其数据消费是解码控制器，解码控制器应该持有它
 * 播放器应该持有解码控制器
 */
public class CompressedFramePacketBuffer {
    public static final String TAG = "CompressedFramePacketBuffer";

    private Queue<FrameGroup> frameGroups;
    private int frameGroupSize;
    private int bufferSize;

    /**
     * 一个15帧的帧组空间大约700k
     * @param bufferSize 缓存的帧组数量
     * @param frameGroupSize 每个帧组大小
     */
    public CompressedFramePacketBuffer(int bufferSize, int frameGroupSize) {
        this.frameGroups = new ArrayBlockingQueue<FrameGroup>(bufferSize);
        this.bufferSize = bufferSize;
        this.frameGroupSize = frameGroupSize;
    }

    private FrameGroup tempFrameGroup;
    /**
     *
     * @param frame 帧数据
     * @return true if add success
     */
    public boolean addFrame(ByteBuffer frame) {
        if (frameGroups.size() == frameGroupSize) {//达到了帧组数量
            return false;
        }

        if (FrameUtils.isKeyFrame(frame)){
            if (tempFrameGroup != null) {
                Log.d(TAG, "添加帧组到缓存，帧组长度 = "+tempFrameGroup.size());
                frameGroups.add(tempFrameGroup);
            }
            tempFrameGroup = new FrameGroup(frameGroupSize);//创建并指向新帧组
            return tempFrameGroup.addFrame(frame);
        }else {
            if (tempFrameGroup == null) {
                Log.d(TAG, "排除开头传输非I帧");
                return false;//排除开头传输非I帧
            }
            Log.d(TAG, "添加p帧");
            return tempFrameGroup.addFrame(frame);
        }
    }

    public FrameGroup getFrameGroup(){
        return frameGroups.poll();
    }

//    public int remain(){
//        return frameGroups.size();
//    }

    public int size(){
        return frameGroups.size();
    }
}
