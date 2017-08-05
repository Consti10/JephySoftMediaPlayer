package com.jephysoftmediaplayer.player;

import android.os.Handler;
import android.os.Looper;
import android.os.Message;
import android.util.Log;

import com.jephysoftmediaplayer.decode.DecodeConsumer;
import com.jephysoftmediaplayer.decode.OnDecodeYUVCompeleted;
import com.jephysoftmediaplayer.decode.OnFrameCallback;
import com.jephysoftmediaplayer.decode.UVCSoftDecoder;
import com.jephysoftmediaplayer.util.CompressedFramePacketBuffer;

import org.greenrobot.eventbus.EventBus;
import org.greenrobot.eventbus.Subscribe;
import org.greenrobot.eventbus.ThreadMode;

import java.nio.ByteBuffer;

/**
 * 解码控制器
 * Created by jephy on 8/2/17.
 * 从播放器回调获取到数据源，分配给解码器进行解码
 * 在这里同时也要创建解码缓存器
 * 解码控制器后面要考虑加入硬解
 */

public class DecodeController implements OnFrameCallback{
    public static final String TAG = "DecodeController";
    private final int RECALL_FRAME_SUCCESS = 0;
    private CompressedFramePacketBuffer compressedFramePacketBuffer;
    private Handler mHandler;

    private UVCSoftDecoder decoder;

    public DecodeController(OnDecodeYUVCompeleted displayer) {
        decoder = new UVCSoftDecoder(displayer);
        EventBus.getDefault().register(this);
        this.compressedFramePacketBuffer = new CompressedFramePacketBuffer(100,20);
        new Thread(frameProducer).start();//开启帧数据缓存线程
        new Thread(new DecodeConsumer(displayer,this.compressedFramePacketBuffer)).start();//开启一个解码器进行解码
        new Thread(new DecodeConsumer(displayer,this.compressedFramePacketBuffer)).start();//再开启一个解码器进行解码
        new Thread(new DecodeConsumer(displayer,this.compressedFramePacketBuffer)).start();//再开启一个解码器进行解码
    }

    @Subscribe(threadMode = ThreadMode.POSTING)
    public void onFrameReceived(ByteBuffer frame){
//        Log.d(TAG, "线程：" + Thread.currentThread());
//        byte[] data = new byte[frame.remaining()];
//        frame.get(data);
//        decoder.decode(data);
        mHandler.obtainMessage(RECALL_FRAME_SUCCESS,frame).sendToTarget();

    }

    @Override
    public void onFrame(ByteBuffer frame) {
        byte[] data = new byte[frame.remaining()];
        frame.get(data);
        decoder.decode(data);
        mHandler.obtainMessage(RECALL_FRAME_SUCCESS,frame).sendToTarget();

    }

//    class DecodeConsumer implements Runnable{
//
//        private UVCSoftDecoder decoder;
//        private CompressedFramePacketBuffer compressedFramePacketBuffer;
//
//        public DecodeConsumer(OnDecodeYUVCompeleted yuvConsumer,CompressedFramePacketBuffer compressedFramePacketBuffer) {
//            this.decoder = new UVCSoftDecoder(yuvConsumer);
//            this.compressedFramePacketBuffer = compressedFramePacketBuffer;
//        }
//
//        @Override
//        public void run() {
//            while (true){
//
//            }
//        }
//    }

    private Runnable frameConsumer = new Runnable() {
        @Override
        public void run() {
            while (true){

            }
        }
    };

    private Runnable frameProducer = new Runnable() {
        @Override
        public void run() {
            Looper.prepare();
            mHandler = new Handler(){
                @Override
                public void handleMessage(Message msg) {
                    switch (msg.what) {
                        case RECALL_FRAME_SUCCESS:
                            ByteBuffer frame = (ByteBuffer) msg.obj;
//                        compressedFramePacketBuffer.addFrame(frame);
//                        Log.d(TAG, "帧组缓存长度："+compressedFramePacketBuffer.remain());
                            byte[] data = new byte[frame.remaining()];
                            frame.get(data);
                            decoder.decode(data);
                            break;
                    }
                }
            };
            Looper.loop();
        }
    };

//    @Override
//    public void run() {//生产者线程
//
//
//    }
}
