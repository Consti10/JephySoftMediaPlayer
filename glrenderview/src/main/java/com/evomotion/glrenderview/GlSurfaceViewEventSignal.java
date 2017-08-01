package com.evomotion.glrenderview;

import java.util.concurrent.locks.ReentrantLock;

public class GlSurfaceViewEventSignal {
    private static final int BYTE_ARRAY = 0;
    private static final int POINTER = 1;

    private byte[] mYBuffer;
    private byte[] mUBuffer;
    private byte[] mVBuffer;
    private byte[] mUVBuffer;
    private long mYPtr;
    private long mUPtr;
    private long mVPtr;
    private long mUVPtr;
    private int mWidth;
    private int mHeight;
    private boolean mUpdateTex;
    private ReentrantLock mLock;
    private boolean isFirstFrame;
    private int mYUVType;
    private int mTextureDataSource;

    public GlSurfaceViewEventSignal() {
        this.mLock = new ReentrantLock();
        mUpdateTex = false;
        mYUVType = -1;
        mTextureDataSource = -1;
    }

    public boolean needUpdateYuvTexture() {
        if (mLock.tryLock()) {
            boolean ret = mUpdateTex;
            mLock.unlock();
            return ret;
        } else {
            return false;
        }
    }

    public boolean isFirstFrame() {
        try {
            mLock.lock();
            return this.isFirstFrame;
        } finally {
            mLock.unlock();
        }

    }

    public void setYuvTexture(byte[] y, byte[] u, byte[] v, int width, int height) {
        mLock.lock();
        try {
            mUpdateTex = false;
            mYBuffer = y;
            mUBuffer = u;
            mVBuffer = v;
            this.mWidth = width;
            this.mHeight = height;
            mUpdateTex = true;
            isFirstFrame = true;

            mYUVType = VideoFormat.YUV420P;
            mTextureDataSource = BYTE_ARRAY;
        } finally {
            mLock.unlock();
        }
    }

    public void setNV12Texture(byte[] y, byte[] uv, int width, int height) {
        mLock.lock();
        try {
            mUpdateTex = false;
            mYBuffer = y;
            mUVBuffer = uv;
            this.mWidth = width;
            this.mHeight = height;
            mUpdateTex = true;
            isFirstFrame = true;

            mYUVType = VideoFormat.NV12;
            mTextureDataSource = BYTE_ARRAY;
        } finally {
            mLock.unlock();
        }
    }


    public void setYuvTexture(long y, long u, long v, int width, int height) {
        mLock.lock();
        try {
            mUpdateTex = false;
            mYPtr = y;
            mUPtr = u;
            mVPtr = v;
            this.mWidth = width;
            this.mHeight = height;
            mUpdateTex = true;
            isFirstFrame = true;
            mYUVType = VideoFormat.YUV420P;
            mTextureDataSource = POINTER;
        } finally {
            mLock.unlock();
        }
    }

    public void setNV12Texture(long y, long uv, int width, int height) {

        //        mLock.lock();
        if (mLock.tryLock()) {
            try {
                mUpdateTex = false;
                mYPtr = y;
                mUVPtr = uv;
                this.mWidth = width;
                this.mHeight = height;
                mUpdateTex = true;
                isFirstFrame = true;
                mYUVType = VideoFormat.NV12;
                mTextureDataSource = POINTER;
            } finally {
                mLock.unlock();
            }
        }
    }

    /**
     * Use the pointer of pixel data instead of the byte array.
     * Will Choose correct method depend on the format(YUV420P, NV12)
     *
     * @param texturePtr the pointer of native texture controller
     */
    public void useTextureData(long texturePtr) {
        switch (mTextureDataSource) {
            case BYTE_ARRAY:
                switch (mYUVType) {
                    case VideoFormat.YUV420P:
                        RenderView.nativeDisplayYuv420pPixelBytes(mYBuffer, mUBuffer, mVBuffer,
                                mWidth,
                                mHeight, texturePtr);
                        break;
                    case VideoFormat.NV12:
                        RenderView.nativeDisplayNV12PixelBytes(mYBuffer, mUVBuffer, mWidth,
                                mHeight, texturePtr);
                        break;
                }
                break;
            case POINTER:
                switch (mYUVType) {
                    case VideoFormat.YUV420P:
                        RenderView.nativeDisplayYuv420pPixel(mYPtr, mUPtr, mVPtr, mWidth, mHeight,
                                texturePtr);
                        break;
                    case VideoFormat.NV12:
                        RenderView.nativeDisplayNV12Pixel(mYPtr, mUVPtr, mWidth, mHeight,
                                texturePtr);
                        break;
                    default:
                        return;
                }
                break;
            default:
                break;
        }
        mLock.lock();
        mUpdateTex = false;
        mLock.unlock();
    }
}
