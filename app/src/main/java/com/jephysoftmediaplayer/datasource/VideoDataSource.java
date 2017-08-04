package com.jephysoftmediaplayer.datasource;

import com.jephysoftmediaplayer.decode.OnFrameCallback;

/**
 * Created by jephy on 8/1/17.
 */

public abstract class VideoDataSource implements DataSource{

    public void setOnFrameCallback(OnFrameCallback onFrameCallback) {
        setOnSourceFrameCallback(onFrameCallback);
    }

    protected abstract void setOnSourceFrameCallback(OnFrameCallback onFrameCallback);

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
