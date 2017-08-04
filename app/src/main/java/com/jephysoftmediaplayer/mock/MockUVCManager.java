package com.jephysoftmediaplayer.mock;

import android.util.Log;

import com.jephysoftmediaplayer.decode.OnFrameCallback;

import java.nio.ByteBuffer;
import java.util.ArrayList;
import java.util.List;

/**
 * Created by jephy on 7/31/17.
 */


public class MockUVCManager {
    private final String TAG = "MockUVCManager";

    public static MockUVCManager instance;
    private ByteBufferSendMocker byteBufferSendMocker;

    private MockUVCManager(){
        if (null == byteBufferSendMocker) {
            byteBufferSendMocker = new ByteBufferSendMocker();
            byteBufferSendMocker.register(iFrameCallback);
            new Thread(byteBufferSendMocker).start();
        }
    }

    private List<OnFrameCallback> mFrameCallbacks = new ArrayList<>();

    public void setFrameCallback(OnFrameCallback callback) {
        mFrameCallbacks.add(callback);
    }

    public static MockUVCManager getInstance(){
        if (instance == null) {
            instance = new MockUVCManager();
        }
        return instance;
    }

    /**
     * 模拟uvc开始
     * 从存储卡获取h264帧数据并发送
     */
    public void startOnDemand() {
        byteBufferSendMocker.open(null);
    }

    public void stopOnDemand(){
        byteBufferSendMocker.pause();
    }

    public void closeOnDemand(){
        byteBufferSendMocker.close();
    }

    private OnFrameCallback iFrameCallback = new OnFrameCallback() {
        @Override
        public void onFrame(ByteBuffer frame) {
            Log.d(TAG, "iFrameCallback: "+frame);
            for (OnFrameCallback cameraFrameCallback : mFrameCallbacks) {
                cameraFrameCallback.onFrame(frame);
                Log.d(TAG, "mock 读取的帧数据："+frame);
            }

        }
    };
}
