package com.evomotion.glrenderview;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;

import java.lang.ref.SoftReference;

public class WaterMarkerProducer {

    private static SoftReference<Bitmap> bitmap;

    public static Bitmap get(Context context) {
        return BitmapFactory.decodeResource(context.getResources(), R.mipmap.watermarker);
        /*if (bitmap == null || bitmap.get() == null) {
            synchronized (WaterMarkerProducer.class) {
                if (bitmap == null || bitmap.get() == null) {
                    bitmap = new SoftReference<Bitmap>(BitmapFactory.decodeResource(context.getResources(), R.mipmap.watermarker));
                }
            }
        }
        return bitmap.get();*/
    }
}
