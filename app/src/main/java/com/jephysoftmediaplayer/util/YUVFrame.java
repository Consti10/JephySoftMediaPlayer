package com.jephysoftmediaplayer.util;

import android.support.annotation.NonNull;

/**
 * Created by jephy on 7/26/17.
 */

public class YUVFrame implements Comparable<YUVFrame>{

    private byte[] y;

    private byte[] u;

    private byte[] v;

    private int width;

    private int height;

    private long timestamp;

    public YUVFrame() {
    }

    public YUVFrame(byte[] y, byte[] u, byte[] v, int width, int height, long timestamp) {
        this.y = y;
        this.u = u;
        this.v = v;
        this.width = width;
        this.height = height;
        this.timestamp = timestamp;
    }

    public int getWidth() {
        return width;
    }

    public void setWidth(int width) {
        this.width = width;
    }

    public int getHeight() {
        return height;
    }

    public void setHeight(int height) {
        this.height = height;
    }

    public byte[] getY() {
        return y;
    }

    public void setY(byte[] y) {
        this.y = y;
    }

    public byte[] getU() {
        return u;
    }

    public void setU(byte[] u) {
        this.u = u;
    }

    public byte[] getV() {
        return v;
    }

    public void setV(byte[] v) {
        this.v = v;
    }

    public long getTimestamp() {
        return timestamp;
    }

    public void setTimestamp(long timestamp) {
        this.timestamp = timestamp;
    }

    @Override
    public int compareTo(@NonNull YUVFrame o) {
        return (int) (this.getTimestamp() - o.getTimestamp());
    }
}
