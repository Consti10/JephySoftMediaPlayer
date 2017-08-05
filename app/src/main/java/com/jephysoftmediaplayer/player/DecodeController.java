package com.jephysoftmediaplayer.player;

import android.os.Handler;
import android.os.Looper;
import android.os.Message;
import android.util.Log;

import com.jephysoftmediaplayer.decode.OnFrameCallback;
import com.jephysoftmediaplayer.util.CompressedFramePacketBuffer;

import java.nio.ByteBuffer;

/**
 * 解码控制器
 * Created by jephy on 8/2/17.
 * 从播放器回调获取到数据源，分配给解码器进行解码
 * 在这里同时也要创建解码缓存器
 * 解码控制器后面要考虑加入硬解
 */

public class DecodeController implements OnFrameCallback,Runnable{
    public static final String TAG = "DecodeController";
    private final int RECALL_FRAME_SUCCESS = 0;
    private CompressedFramePacketBuffer compressedFramePacketBuffer;
    private Handler mHandler;

    public DecodeController() {
        this.compressedFramePacketBuffer = new CompressedFramePacketBuffer(100,20);
        new Thread(this).start();
    }

    @Override
    public void onFrame(ByteBuffer frame) {
        mHandler.obtainMessage(RECALL_FRAME_SUCCESS,frame).sendToTarget();
    }

    @Override
    public void run() {
        Looper.prepare();
        mHandler = new Handler(){
            @Override
            public void handleMessage(Message msg) {

                switch (msg.what) {
                    case RECALL_FRAME_SUCCESS:
                        compressedFramePacketBuffer.addFrame((ByteBuffer) msg.obj);
                        Log.d(TAG, "帧组缓存长度："+compressedFramePacketBuffer.remain());
                        break;
                }
            }
        };
        Looper.loop();
    }
}
