//
// Created by zhuyuanxuan on 02/12/2016.
// Copyright (c) 2016 Evomotion. All rights reserved.
//

#ifndef GL_RENDER_PICTEXTURECONTROLLER_H
#define GL_RENDER_PICTEXTURECONTROLLER_H

#include "BaseTextureController.h"

namespace evo {

    class PicTexController : public BaseTextureController {

    public:
        PicTexController(BaseRenderController *renderer);

        PicTexController(const BaseTextureController &obj) = delete;

        virtual ~PicTexController();

        void Init() override;

        int GenTextures(TextureType type) override;

        // This two functions are called in ios, only set texture name.
        bool SetRgbTextureName(GLuint name) override;

        bool SetYuvTexName(GLuint y_name, GLuint uv_name) override;

        // @param format now support RGBA8888 and RGB565, others not supported!
        // enum AndroidBitmapFormat {
        //     ANDROID_BITMAP_FORMAT_NONE      = 0,
        //     ANDROID_BITMAP_FORMAT_RGBA_8888 = 1,
        //     ANDROID_BITMAP_FORMAT_RGB_565   = 4,
        //     ANDROID_BITMAP_FORMAT_RGBA_4444 = 7,
        //     ANDROID_BITMAP_FORMAT_A_8       = 8,
        // };
        bool SetBmpDataTexture(void *data, int width, int height,
                               int32_t format) override;

        // Set texture with logo, will add the logo to the bottom of the source bitmap
        bool SetBmpAndLogoData(void *tex, void *logo, int tex_width, int tex_height, int logo_width,
                               int logo_height);

        bool SetLogoBmpData(void *data, int width, int height) override;

        bool SetNV12TexData(GLubyte *y_data, GLubyte *uv_data, int width, int height) override;

        // Call this when change to another displaySelectorView mode
        // Active the texture again
        bool ReActiveTex() override;

        bool BindTexture() override;

        TextureType get_texture_type() override;

    private:
        GLint m_rgb_sampler_index;
        GLint m_logo_sampler_index;
        uint8_t *m_tex_data;
        uint8_t *m_logo_tex_data;
        int m_tex_data_ok;

        void InitSamplerLoc();
    };

}

#endif //GL_RENDER_PICTEXTURECONTROLLER_H
