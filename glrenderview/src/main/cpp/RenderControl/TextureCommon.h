//
// Created by zhuyuanxuan on 22/11/2016.
// Copyright (c) 2016 Evomotion. All rights reserved.
//

#ifndef GL_RENDER_TEXTURE_COMMON_H
#define GL_RENDER_TEXTURE_COMMON_H

#include <stdint.h>

enum TextureType {
    NULL_TYPE,
    IMAGE,
    VIDEO,
    YUV420P, /* YUV420P, [YY YY UU VV] */
    NV12,    /* YUV420SP, [YY YY UV] */
    NV21     /* YUV420SP, [YY YY VU] */
};

enum DisplayMode {
    NULL_MODE,
    SPHERE,
    LITTLE_PLANET,
    FISHEYE,
    VR
};

struct TextureInfo {
    TextureType type;
    int width;
    int height;
    uint8_t *data;
};
typedef struct TextureInfo TextureInfo;

const float yuv_mat_sdtv[] = {1.164383, 1.164383, 1.164383,
                              0.0, -0.391762f, 2.017232,
                              1.596027, -0.812968f, 0.0};

const float yuv_mat_hdtv[] = {1.164383, 1.164383, 1.164383,
                              0.0, -0.213f, 2.112,
                              1.793, -0.533f, 0.0};

const float yuv_mat_full_range[] = {1.0, 1.0, 1.0,
                                    0.0, -0.343f, 1.765,
                                    1.4, -0.711f, 0.0};

const float kYuvMat[] = {1.0, 1.0, 1.0,
                         0.0, -0.187f, 1.856,
                         1.575, -0.468f, 0.0};

#endif
// GL_RENDER_TEXTURE_COMMON_H
