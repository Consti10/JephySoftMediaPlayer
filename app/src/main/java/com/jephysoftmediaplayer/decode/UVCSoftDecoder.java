package com.jephysoftmediaplayer.decode;

import android.support.annotation.Keep;
import android.util.Log;

import java.nio.ByteBuffer;

/**
 * Created by jephy on 7/17/17.
 */
public class UVCSoftDecoder extends JNIObject{
    private final String TAG = "UVCSoftDecoder";
    private OnDecodeYUVCompeleted onDecodeYUVCompeleted;

    public UVCSoftDecoder(OnDecodeYUVCompeleted onDecodeYUVCompeleted) {
        this.onDecodeYUVCompeleted = onDecodeYUVCompeleted;
        construct();
        int ret = init(5);
        Log.d(TAG, "UVCSoftDecoder........................." + ret);
    }

    long nativeStart = 0;
    int nativeframeCount = 0;
    boolean nativeIsFirstDecodeCompleted = true;
    long nativeCurrentTime = 0;
    public int decode(byte[] data){
        nativeframeCount++;
        if (nativeIsFirstDecodeCompleted) {
            nativeStart = System.currentTimeMillis();
            nativeIsFirstDecodeCompleted = false;
        }
        int ret = decode(data, data.length);
        nativeCurrentTime = System.currentTimeMillis();
        long nativeAverageTime = (nativeCurrentTime -nativeStart)/nativeframeCount;
        Log.d(TAG, "底层解码时间：" + nativeAverageTime);

        return ret;
    }

    public void stopDecoder(){
        destruct();
    }

    @Override
    protected void finalize() throws Throwable {
        destruct();
        super.finalize();
    }

    //回调,解码后数据
    @Keep
    private void RawPacket(int width, int height, long timestamp, byte[] data, int size) {
        Log.d(TAG, "RawPacket.........................data.length = "+data.length);
        Log.d(TAG, "当前线程" + Thread.currentThread());
//        onDecodeYUVCompeleted.onDecodeYUVCompeleted(data);
        Log.d(TAG, "RawPacket timestamp = " + timestamp);
    }

    long start = 0;
    int frameCount = 0;
    boolean isFirstDecodeCompleted = true;
    long currentTime = 0;
    @Keep
    private void YUVPacket(int width, int height, long timestamp, byte[] y, byte[] u,byte[] v) {
//        Log.d(TAG, "当前线程" + Thread.currentThread());
//        Log.d(TAG, "YUVPacket.........................y.length = "+y.length+" u.length = "+u.length+" v.leangth = "+v.length);
//        Log.d(TAG, "YUVPacket timeStamp = " + timestamp);
        frameCount++;
        if (isFirstDecodeCompleted) {
            start = System.currentTimeMillis();
            isFirstDecodeCompleted = false;
        }
        onDecodeYUVCompeleted.onDecodeYUVCompeleted(y,u,v,width,height,timestamp);
        currentTime = System.currentTimeMillis();
        long averateTime = (currentTime - start)/frameCount;
        Log.d(TAG, "averatetime = "+ averateTime);
    }

    void YUVPacketNoCopy(int width, int height, long timestamp, byte[] data)
    {
        frameCount++;
        if (isFirstDecodeCompleted) {
            start = System.currentTimeMillis();
            isFirstDecodeCompleted = false;
        }
//        Log.d(TAG, "YUVPacketNoCopy : "+data.length);
        ByteBuffer y = ByteBuffer.wrap(data,0,width*height);
        ByteBuffer u = ByteBuffer.wrap(data,width*height,width*height/4);
        ByteBuffer v = ByteBuffer.wrap(data,width*height + width*height/4,width*height/4);

        byte[] yData = new byte[y.remaining()];
        y.get(yData);

        byte[] uData = new byte[u.remaining()];
        u.get(uData);

        byte[] vData = new byte[v.remaining()];
        v.get(vData);

        onDecodeYUVCompeleted.onDecodeYUVCompeleted(yData,uData,vData,width,height,timestamp);

        currentTime = System.currentTimeMillis();
        long averateTime = (currentTime - start)/frameCount;
        Log.d(TAG, "averatetime = "+ averateTime+"timeStamp = "+ timestamp);
    }

    static {
        System.loadLibrary("media_controller-lib");
    }

    public native void construct();
    public native void destruct();

    native int init(int thread_count);
    native int decode(byte[] data,int size);

    public ByteBuffer getPicture()
    {
        return null;
    }

}
