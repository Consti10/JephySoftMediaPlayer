package com.jephysoftmediaplayer.mock;

import android.os.Environment;
import android.util.Log;

import com.jephysoftmediaplayer.decode.OnFrameCallBack;

import java.io.BufferedInputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.nio.ByteBuffer;
import java.util.ArrayList;
import java.util.List;

public class ByteBufferSendMocker implements Data{
    private final String TAG = "ByteBufferSendMocker";
    private List<OnFrameCallBack> observers = new ArrayList<>();

    private File dir = null;

    public void open(File dir){
        if (null ==dir ) {
            dir = new File(Environment.getExternalStorageDirectory().getAbsolutePath() + "/temp_frame_0724_1549");
//            dir = new File(Environment.getExternalStorageDirectory().getAbsolutePath() + "/temp_frame_0722");
        }

        int frameNumber = dir.list().length;
        Log.d(TAG, "文件夹中文件数量" + frameNumber);
        for (int fileNum=0;fileNum<frameNumber;fileNum++) {
            File frame = new File(dir,"" + fileNum + "_frame.txt");
            FileInputStream inputStream =null;
            BufferedInputStream bis = null;
            try {

                inputStream = new FileInputStream(frame);
                bis = new BufferedInputStream(inputStream);
                int fileLength = bis.available();
                byte[] bytes = new byte[fileLength];
                bis.read(bytes);
                Log.d(TAG, "bytes[0] = "+bytes[0]+"，bytes[1] = "+bytes[1]+"bytes[2] = "+bytes[2]);

                ByteBuffer byteBuffer = ByteBuffer.wrap(bytes);
                Log.d(TAG, "Mock读取帧长度： " + byteBuffer.remaining());
                try {
                    Thread.currentThread().sleep(20);
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }
                send(byteBuffer);

            } catch (FileNotFoundException e) {
                e.printStackTrace();
            } catch (IOException e) {
                e.printStackTrace();
            }finally {
                if (null != inputStream) {
                    try {
                        inputStream.close();
                    } catch (IOException e) {
                        e.printStackTrace();
                    }
                }

                if (null != bis) {
                    try {
                        bis.close();
                    } catch (IOException e) {
                        e.printStackTrace();
                    }
                }
            }

        }

    }

    @Override
    public void send(ByteBuffer byteBuffer) {
        for (OnFrameCallBack observer :
                observers) {
                observer.onFrame(byteBuffer);
        }
    }

    @Override
    public void register(OnFrameCallBack observer) {
        observers.add(observer);
    }

    @Override
    public void unRegister(OnFrameCallBack observer) {
        observers.remove(observer);
    }
}
