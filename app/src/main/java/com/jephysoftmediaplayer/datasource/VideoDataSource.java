package com.jephysoftmediaplayer.datasource;

/**
 * Created by jephy on 8/1/17.
 */

public abstract class VideoDataSource implements DataSource{

//    @Override
//    public abstract void start();
//
//    @Override
//    public abstract void pause();
//
//    @Override
//    public abstract void stop();
//
//    @Override
//    public abstract void seekTo(int second);




    protected OnFrameCallback onFrameCallback;

    public void setOnFrameCallback(OnFrameCallback onFrameCallback) {
        this.onFrameCallback = onFrameCallback;
    }

    protected OnVideoInfoCallback onVideoInfoCallback;

    public void setOnVideoInfoCallback(OnVideoInfoCallback onVideoInfoCallback) {
        this.onVideoInfoCallback = onVideoInfoCallback;
    }

    protected OnPlayStatus onPlayStatus;

    public void setOnPlayStatus(OnPlayStatus onPlayStatus) {
        this.onPlayStatus = onPlayStatus;
    }

    public interface OnVideoInfoCallback {
        void onVideoInfo(int frameRate, int durration);
    }

    public interface OnPlayStatus{
        enum Status{
             STATUS_IDLE,
             STATUS_WORK,
             STATUS_PAUSE
        }
        void onPlayStatus(Status status);
    }
}
