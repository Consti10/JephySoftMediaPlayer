package com.jephysoftmediaplayer.mock;

import android.os.Environment;
import android.os.Handler;
import android.os.Looper;
import android.os.Message;
import android.util.Log;

import com.jephysoftmediaplayer.decode.OnFrameCallback;

import java.io.BufferedInputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.nio.ByteBuffer;
import java.util.ArrayList;
import java.util.List;

public class ByteBufferSendMocker implements Data,Runnable{
    private final String TAG = "ByteBufferSendMocker";
    private final static int START = 0;
    private final static int PAUSE = 1;
    private final static int STOP = 2;


    private List<OnFrameCallback> observers = new ArrayList<>();

    private int frameRate = 30;

    public void setFrameRate(int frameRate){
        this.frameRate = frameRate;
    }

    private volatile boolean isStop = false;
    private volatile boolean isPause = false;
    private volatile boolean isPlay = false;

    //暂停流
    public void pause(){
        Log.d(TAG, "叫了暂停");
        isPause = true;
    }

    //停止流
    public void close(){
        isStop = true;
        currentPosition = 0;
    }

    public void open(File fileDir){
        if (fileDir == null) {
            fileDir = new File(Environment.getExternalStorageDirectory().getAbsolutePath() + "/temp_frame_0724_1549");
        }
        Message message = mHandler.obtainMessage(START);
        message.obj = fileDir;
        mHandler.sendMessage(message);
    }

    private void openFileStream(File fileDir){
        final File dir = fileDir;
        mockSendFrame(dir);
        isPlay = true;
    }

//    long start = 0;
//    int frameCount = 0;
//    boolean isFirstDecodeCompleted = true;
//    long currentTime = 0;
    private volatile int currentPosition = 0;//记录状态
    private void mockSendFrame(File fileDir) {
        final File dir = fileDir;

        int frameNumber = dir.list().length;
        Log.d(TAG, "文件夹中文件数量" + frameNumber);
        for (int fileNum = currentPosition; fileNum < frameNumber; fileNum++) {
            Log.d(TAG, "当前线程："+Thread.currentThread()+", 读取位置: "+ fileNum);
            if (isPause){
                currentPosition = fileNum;
                break;
            }
            if (isStop) {
                currentPosition = 0;
                break;
            }
            File frame = new File(dir, "" + fileNum + "_frame.txt");
//            frameCount++;
//            if (isFirstDecodeCompleted) {
//                start = System.currentTimeMillis();
//                isFirstDecodeCompleted = false;
//            }
            readFrame(frame);
//            currentTime = System.currentTimeMillis();
//            long averateTime = (currentTime - start)/frameCount;
//            Log.d(TAG, "io averatetime = "+ averateTime);
        }
    }

    private void readFrame(File frame) {
        FileInputStream inputStream = null;
        BufferedInputStream bis = null;
        try {
            inputStream = new FileInputStream(frame);
            bis = new BufferedInputStream(inputStream);
            int fileLength = bis.available();
            byte[] bytes = new byte[fileLength];
            bis.read(bytes);
//            Log.d(TAG, "bytes[0] = " + bytes[0] + "，bytes[1] = " + bytes[1] + "bytes[2] = " + bytes[2]);

            ByteBuffer byteBuffer = ByteBuffer.wrap(bytes);
            Log.d(TAG, "Mock读取帧长度： " + byteBuffer.remaining());
            try {
                int shouldSleep = 1000 / frameRate;
                Thread.currentThread().sleep(33);
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
            send(byteBuffer);

        } catch (FileNotFoundException e) {
            e.printStackTrace();
        } catch (IOException e) {
            e.printStackTrace();
        } finally {
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

    @Override
    public void send(ByteBuffer byteBuffer) {
        for (OnFrameCallback observer :
                observers) {
                observer.onFrame(byteBuffer);
        }
    }

    @Override
    public void register(OnFrameCallback observer) {
        observers.add(observer);
    }

    @Override
    public void unRegister(OnFrameCallback observer) {
        observers.remove(observer);
    }

    private Handler mHandler = null;
    @Override
    public void run() {
        Looper.prepare();
        mHandler = new Handler(){
            @Override
            public void handleMessage(Message msg) {
                switch (msg.what){

                    case START:
                        Log.d(TAG, "START：" + Thread.currentThread());
                        if (isPause == true) {
                            isPause=false;
                        }

                        if (isStop = true) {
                            isStop = false;
                        }
                        File file = (File) msg.obj;
                        openFileStream(file);
                        break;
                    case PAUSE:
                        Log.d(TAG, "PAUSE：" + Thread.currentThread()+"isPause = "+isPause);
                        isPause = true;
                        break;
                    case STOP:
                        Log.d(TAG, "STOP：" + Thread.currentThread()+"isStop = "+isStop);
                        isStop = true;
                        break;
                }
            }
        };
        Looper.loop();
    }
}
