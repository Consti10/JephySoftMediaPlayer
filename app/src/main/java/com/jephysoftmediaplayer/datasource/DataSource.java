package com.jephysoftmediaplayer.datasource;

/**
 * Created by jephy on 8/1/17.
 */

public interface DataSource {

    void start();

    void pause();

    void stop();

    void seekTo(int second);

    void setSource(String source);
}
