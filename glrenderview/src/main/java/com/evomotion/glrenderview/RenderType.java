package com.evomotion.glrenderview;

import android.support.annotation.IntDef;

/**
 * Created by zhuyuanxuan on 06/12/2016.
 * gl-render
 */

public class RenderType {


    @IntDef({NULL, SPHERE, LITTLE_PLANET, FISHEYE, VR})
    public @interface RenderTypeMode {
    }

    public static final int NULL = -1;
    public static final int SPHERE = 1;
    public static final int LITTLE_PLANET = 2;
    public static final int FISHEYE = 3;
    public static final int VR = 4;
}
