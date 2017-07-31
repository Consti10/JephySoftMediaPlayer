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
        int ret = init();
        Log.d(TAG, "UVCSoftDecoder........................." + ret);
    }

    public int decode(byte[] data){

        return decode(data, data.length);
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

    @Keep
    private void YUVPacket(int width, int height, long timestamp, byte[] y, byte[] u,byte[] v) {
        Log.d(TAG, "当前线程" + Thread.currentThread());
        Log.d(TAG, "YUVPacket.........................y.length = "+y.length+" u.length = "+u.length+" v.leangth = "+v.length);
        Log.d(TAG, "YUVPacket timeStamp = " + timestamp);
        onDecodeYUVCompeleted.onDecodeYUVCompeleted(y,u,v,width,height,timestamp);
    }

    static {
        System.loadLibrary("media_controller-lib");
    }

    public native void construct();
    public native void destruct();

    native int init();
    native int decode(byte[] data,int size);

    public ByteBuffer getPicture()
    {
        return null;
    }

}
