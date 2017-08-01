package com.jephysoftmediaplayer.player;

import android.os.Handler;
import android.os.Message;
import android.util.Log;

import com.jephysoftmediaplayer.decode.OnDecodeYUVCompeleted;
import com.jephysoftmediaplayer.decode.UVCSoftDecoder;
import com.jephysoftmediaplayer.mock.MockUVCManager;

import java.nio.ByteBuffer;

/**
 * Created by jephy on 7/31/17.
 */

public class JephyPlayer implements OnDecodeYUVCompeleted {
    private static final String TAG = "JephyPlayer";
    private final static int DECODED_SUCCESS = 0;

    private MockUVCManager mockUVCManager;
    private UVCSoftDecoder uvcSoftDecoder;

    public void prepare(){
        mockUVCManager = MockUVCManager.getInstance();
        uvcSoftDecoder = new UVCSoftDecoder(this);
        mockUVCManager.setFrameCallback(cameraFrameCallback);
    }

    public void start(){
        mockUVCManager.startOnDemand();
    }

    public void pause(){
        //TODO mockUVCManager.pause();
        mockUVCManager.stopOnDemand();
    }

    public void stop(){
        mockUVCManager.closeOnDemand();
    }

    public void seekTo(int position){

    }

    private MockUVCManager.UVCCameraFrameCallback cameraFrameCallback = new MockUVCManager.UVCCameraFrameCallback() {
        @Override
        public void onFrameCallback(ByteBuffer frame) {
//            Log.d(TAG, "JephyPlayer onFrameCallbck: "+ frame);
            byte[] frameBytes = new byte[frame.remaining()];
            frame.get(frameBytes);
            Log.d(TAG, "JephyPlayer onFrameCallbck: "+ frameBytes.length);

            uvcSoftDecoder.decode(frameBytes);
        }
    };

    long decodeCount = 0;
    long startTime = 0;
    boolean initStartTime = true;
    @Override
    public void onDecodeYUVCompeleted(byte[] yData,byte[] uData,byte[] vData, int width, int height,long timeStamp) {
        Log.d(TAG, "解完一帧，所在线程 " + Thread.currentThread());
        if (initStartTime) {
            startTime = System.currentTimeMillis();
        }
        initStartTime = false;
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
                    break;
            }
        }
    };
}
