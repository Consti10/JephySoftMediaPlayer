//
// Created by zhuyuanxuan on 18/11/2016.
// Copyright (c) 2016 Evomotion. All rights reserved.
//

#ifndef GL_RENDER_BASE_RENDER_H
#define GL_RENDER_BASE_RENDER_H

#include "../GLProgram.h"

namespace evo {
    class BaseRender {
    public:
        // return true if init fail.\nFalse if OK.
        bool InitProgram(const char *vertex_shader, const char *frag_shader);

        void InitVao(GLuint *vao, GLuint vertex_index, GLuint texcoord_index,
                     float *vertex, float *tex_coords, GLint numbers);

        GLuint get_program_id();

        virtual ~BaseRender();

    private:
        evo::GLProgram *m_program;
    };
}
#endif
// GL_RENDER_BASE_RENDER_H
