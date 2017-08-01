//
// Created by zhuyuanxuan on 16/02/2017.
// Copyright (c) 2017 Evomotion. All rights reserved.
//

#ifndef EVOCLIENTFORIOS_RECTANGLERENDERER_H
#define EVOCLIENTFORIOS_RECTANGLERENDERER_H

#include "BaseRender.h"

namespace evo {
    class RectangleRenderer {
    public:
        GLuint get_program();

        int InitGl();

        int InitUniforms(int width, int height);

        int SetYuvTextureName(unsigned int y, unsigned int uv);

        int draw();

    private:
        BaseRender *m_renderer;
        GLuint m_program;
        GLuint m_vao;
    };

}

#endif
// EVOCLIENTFORIOS_RECTANGLERENDERER_H
