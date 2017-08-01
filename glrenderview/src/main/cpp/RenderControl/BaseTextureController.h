//
// Created by zhuyuanxuan on 22/11/2016.
// Copyright (c) 2016 Evomotion. All rights reserved.
//

#ifndef GL_RENDER_TEXTURE_CONTROLLER_H
#define GL_RENDER_TEXTURE_CONTROLLER_H

#include "TextureCommon.h"
#include "BaseRenderController.h"

namespace evo {
    class BaseTextureController {
    public:

        BaseTextureController(BaseRenderController *renderer);

        BaseTextureController(const BaseTextureController &obj) = delete;

        // get texture sampler location, create texture obj
        // don't call it when use android external texture
        virtual void Init() = 0;

        virtual int GenTextures(TextureType type) = 0;

        void SetRender(BaseRenderController *render);

        // Call this when change to another displaySelectorView mode
        virtual bool ReActiveTex() = 0;

        virtual bool BindTexture() = 0;

        virtual bool SetBmpDataTexture(void *data, int width, int height,
                                       int32_t format) = 0;
        virtual bool
        SetNV12TexData(GLubyte *y_data, GLubyte *uv_data,
                      int width, int height) = 0;

        virtual bool SetLogoBmpData(void *data, int width, int height) = 0;

        virtual bool SetRgbTextureName(GLuint name) = 0;

        virtual bool SetYuvTexName(GLuint y_name, GLuint uv_name) = 0;

        virtual TextureType get_texture_type() = 0;

        virtual ~BaseTextureController();

    protected:
        BaseRenderController *m_renderer;
        GLsizei m_width;
        GLsizei m_height;
        GLuint m_texture_list[10];
        int m_texture_num;
        bool m_texture_ok;
        bool m_create_texture_myself;
    };

}
#endif
// GL_RENDER_TEXTURE_CONTROLLER_H
