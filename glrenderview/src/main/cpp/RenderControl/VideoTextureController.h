//
// Created by zhuyuanxuan on 25/11/2016.
// Copyright (c) 2016 Evomotion. All rights reserved.
//

#ifndef GL_RENDER_VIDEOTEXTURECONTROLLER_H
#define GL_RENDER_VIDEOTEXTURECONTROLLER_H

#include "BaseTextureController.h"

namespace evo {
    class VideoTextureController : public BaseTextureController {
    public:

        VideoTextureController(BaseRenderController *renderer, int yuv_format);

        VideoTextureController(const VideoTextureController &obj) = delete;

        ~VideoTextureController();

        // Make sure this will only be called in yuv texture!
        // Don't call if use android external texture.
        // Get all samplers' location and upload the yuv transform mat
        void Init() override;

        bool ReActiveTex() override;

        bool BindTexture() override;

        // return true, don't call it.
        bool SetBmpDataTexture(void *data, int width, int height, int32_t format) override;

        bool SetLogoBmpData(void *data, int width, int height) override;

        bool SetRgbTextureName(GLuint name) override;

        bool SetNV12TexData(GLubyte *y_data, GLubyte *uv_data, int width, int height) override;

        bool SetYuvTexData(GLubyte *y_data, GLubyte *u_data, GLubyte *v_data, int width, int height);

        bool SetYuvTexName(GLuint y_name, GLuint uv_name) override;

        TextureType get_texture_type() override;

        int GenTextures(TextureType type) override;

    private:
        GLint m_sampler_y_index;
        GLint m_sampler_uv_index;
        GLint m_sampler_u_index;
        GLint m_sampler_v_index;
        GLint m_logo_sampler_index;
        GLuint m_y_tex_name;
        GLuint m_uv_tex_name;
        GLint m_sampler_android_index;
        uint8_t *m_logo_tex_data;
        int m_yuv_format;

        void InitSamplerLoc();
    };
}

#endif
// GL_RENDER_VIDEOTEXTURECONTROLLER_H
