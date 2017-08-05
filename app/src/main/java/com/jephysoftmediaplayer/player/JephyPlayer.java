package com.jephysoftmediaplayer.player;

import android.os.Handler;
import android.os.Message;
import android.util.Log;

import com.evomotion.glrenderview.GlVideoRenderLayout;
import com.evomotion.glrenderview.RenderType;
import com.evomotion.glrenderview.VideoFormat;
import com.jephysoftmediaplayer.datasource.H264FileVideoDataSource;
import com.jephysoftmediaplayer.datasource.VideoDataSource;
import com.jephysoftmediaplayer.decode.OnDecodeYUVCompeleted;
import com.jephysoftmediaplayer.decode.OnFrameCallback;
import com.jephysoftmediaplayer.decode.UVCSoftDecoder;


import org.greenrobot.eventbus.EventBus;

import java.nio.ByteBuffer;

/**
 * Created by jephy on 7/31/17.
 */

public class JephyPlayer implements OnDecodeYUVCompeleted {
    private static final String TAG = "JephyPlayer";
    private final static int DECODED_SUCCESS = 0;

    private VideoDataSource videoDataSource;
    private UVCSoftDecoder uvcSoftDecoder;
    private DecodeController decodeController;
    private GlVideoRenderLayout mGlVideoRenderLayout;

    public void setDisplay(GlVideoRenderLayout glVideoRenderLayout){
        this.mGlVideoRenderLayout = glVideoRenderLayout;
    }

    public void prepare(){
        uvcSoftDecoder = new UVCSoftDecoder(this);
        decodeController = new DecodeController(this);

        videoDataSource = new H264FileVideoDataSource();
        videoDataSource.setOnFrameCallback(onFrameCallback);
        initRenderView();
    }

    private OnFrameCallback onFrameCallback = new OnFrameCallback() {
        @Override
        public void onFrame(ByteBuffer frame) {
//            byte[] frameBytes = new byte[frame.remaining()];
//            frame.get(frameBytes);
//            Log.d(TAG, "JephyPlayer onFrameCallbck: "+ frameBytes.length);

//            uvcSoftDecoder.decode(frameBytes);
//            decodeController.onFrame(frame);
            EventBus.getDefault().post(frame);
        }
    };

    private void initRenderView(){
        mGlVideoRenderLayout.setTouchMode();
        mGlVideoRenderLayout.setCallback(new GlVideoRenderLayout.GlVideoRenderLayoutCallback() {
            @Override
            public int videoFrameFormat() {
                return VideoFormat.YUV420P;
            }
        });

        mGlVideoRenderLayout.setRenderType(RenderType.SPHERE);
    }

    public void start(){
        startTime = System.currentTimeMillis();
        decodeCount = 0;
        videoDataSource.start();
    }

    public void pause(){
        videoDataSource.pause();

    }

    public void stop(){
        videoDataSource.stop();
    }

    public void seekTo(int position){

    }

    long decodeCount = 0;
    long startTime = 0;
    @Override
    public void onDecodeYUVCompeleted(byte[] yData,byte[] uData,byte[] vData, int width, int height,long timeStamp) {
        Log.d(TAG, "解完一帧，所在线程 " + Thread.currentThread());
        decodeCount ++;
        long averageDecodeTime = (System.currentTimeMillis() - startTime)/decodeCount;
        Log.d(TAG, "平均解码时间" + averageDecodeTime);
        byte[][] yuvBytes = new byte[][]{yData,uData,vData};
        Message msg = mHandler.obtainMessage(DECODED_SUCCESS);
        msg.obj = yuvBytes;
        msg.arg1 = width;
        msg.arg2 = height;
        mHandler.sendMessage(msg);
    }

    private Handler mHandler = new Handler() {
        @Override
        public void handleMessage(Message msg) {
            switch (msg.what) {
                case DECODED_SUCCESS:
                    byte[][] data = (byte[][]) msg.obj;
                    int width = msg.arg1;
                    int height = msg.arg2;
                    Log.d(TAG, "DECODED_SUCCESS: " + data[0].length);
                    mGlVideoRenderLayout.displayPixels(data[0], data[1], data[2], width, height);
                    break;
            }
        }
    };
}
