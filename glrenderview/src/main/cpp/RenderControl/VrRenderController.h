//
// Created by zhuyuanxuan on 01/12/2016.
// Copyright (c) 2016 Evomotion. All rights reserved.
//

#ifndef GL_RENDER_VRRENDERCONTROLLER_H
#define GL_RENDER_VRRENDERCONTROLLER_H

#include "BaseRenderController.h"

namespace evo {

    class VrRenderController : public BaseRenderController {

    public:

        VrRenderController(TextureType type);

        VrRenderController(const VrRenderController &obj) = delete;

        ~VrRenderController() override;

        bool InitGl() override;

        bool InitUniforms(int width, int height) override;

        bool Draw() override;

        void UpdateLookat(float *mat4) override;

        void UpdateProjection(float *mat4) override;

        void OnSpanXY(float x, float y) override;

        void OnZoom(float factor) override;

        // add iOS VR mode
        void SetFirstTextureName(GLuint texture_name);

    private:

        GLfloat *m_lookat_mat4;
        GLfloat *m_projection_mat4;
        GLuint m_textures[10];
        int m_texture_num;
        GLint m_model_index;
        GLint m_lookat_index;
        GLint m_projection_mat_index;
        int m_update_param;

        bool UpdateParam();
    };
}

#endif
//  GL_RENDER_VRRENDERCONTROLLER_H
