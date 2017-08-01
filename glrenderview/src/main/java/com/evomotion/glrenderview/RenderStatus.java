package com.evomotion.glrenderview;

import android.os.Parcel;
import android.os.Parcelable;


public class RenderStatus implements Parcelable {
    private float mLatitude;
    private float mLongitude;
    private float mZoomFactor;

    public float getLatitude() {
        return mLatitude;
    }

    public void setLatitude(float mLatitude) {
        this.mLatitude = mLatitude;
    }

    public float getLongitude() {
        return mLongitude;
    }

    public void setLongitude(float mLongitude) {
        this.mLongitude = mLongitude;
    }

    public float getZoomFactor() {
        return mZoomFactor;
    }

    public void setZoomFactor(float mZoomFactor) {
        this.mZoomFactor = mZoomFactor;
    }

    @Override
    public int describeContents() {
        return 0;
    }

    @Override
    public void writeToParcel(Parcel dest, int flags) {
        dest.writeFloat(this.mLatitude);
        dest.writeFloat(this.mLongitude);
        dest.writeFloat(this.mZoomFactor);
    }

    public RenderStatus() {
    }

    protected RenderStatus(Parcel in) {
        this.mLatitude = in.readFloat();
        this.mLongitude = in.readFloat();
        this.mZoomFactor = in.readFloat();
    }

    public static final Parcelable.Creator<RenderStatus> CREATOR = new Parcelable.Creator<RenderStatus>() {
        @Override
        public RenderStatus createFromParcel(Parcel source) {
            return new RenderStatus(source);
        }

        @Override
        public RenderStatus[] newArray(int size) {
            return new RenderStatus[size];
        }
    };
}
