package com.jephysoftmediaplayer.datasource;

import com.jephysoftmediaplayer.mock.MockUVCManager;

/**
 * Created by jephy on 8/4/17.
 */

public class H264FileVideoDataSource extends VideoDataSource {
    private MockUVCManager mockUVCManager;

    public H264FileVideoDataSource() {
        this.mockUVCManager = MockUVCManager.getInstance();
        mockUVCManager.setFrameCallback(onFrameCallback);
    }

    @Override
    public void start() {
        mockUVCManager.startOnDemand();
    }

    @Override
    public void pause() {
        mockUVCManager.stopOnDemand();
    }

    @Override
    public void stop() {
        mockUVCManager.closeOnDemand();
    }

    @Override
    public void seekTo(int second) {

    }

    @Override
    public void setSource(String source) {

    }

}
