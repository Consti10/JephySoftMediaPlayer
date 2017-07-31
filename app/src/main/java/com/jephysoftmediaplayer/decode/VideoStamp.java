package com.jephysoftmediaplayer.decode;

import android.util.Log;

/**
 * Created by jfyang on 7/24/17.
 */

public class VideoStamp {
    private static final String TAG = "VideoStamp";
    public long pts;
    public long dts;
    public long timestamp;
    public int flag;//1, 关键帧; 0,非关键帧
    public int timebase_num;
    public int  timebase_den;

    public static boolean isIDRFrame(byte[] frame){

        VideoStamp videoStamp = Analysis(frame, frame.length);
        if (videoStamp == null) {
            Log.d(TAG, "videoStamp为空");
            return false;
        }
        Log.d(TAG, "videoStamp.flag = "+ videoStamp.flag+",  timestamp+ "+videoStamp.timestamp);

        if (videoStamp.flag == 1) {
            return true;
        }

        return false;
    }

    public static native VideoStamp Analysis(byte[] data,int size);
}
