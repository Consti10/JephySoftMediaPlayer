//
// Created by zhuyuanxuan on 18/11/2016.
// Copyright (c) 2016 Evomotion. All rights reserved.
//

#include "BaseRender.h"

void evo::BaseRender::InitVao(GLuint *vao,
                              GLuint vertex_index,
                              GLuint texcoord_index,
                              float *vertex, float *tex_coords,
                              GLint numbers) {
    GLuint vbo[] = {0, 0, 0};

    glGenVertexArrays(1, vao);
    glBindVertexArray(*vao);
    glGenBuffers(2, vbo);

    glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
    glBufferData(GL_ARRAY_BUFFER, 3 * numbers * sizeof(float), vertex, GL_STATIC_DRAW);
    glVertexAttribPointer(vertex_index, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
    glBufferData(GL_ARRAY_BUFFER, 2 * numbers * sizeof(float), tex_coords, GL_STATIC_DRAW);
    glVertexAttribPointer(texcoord_index, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glEnableVertexAttribArray(vertex_index);
    glEnableVertexAttribArray(texcoord_index);
    glBindVertexArray(0);

    glDeleteBuffers(2, vbo);
}

bool evo::BaseRender::InitProgram(const char *vertex_shader, const char *frag_shader) {
    m_program = new evo::GLProgram();
    if (m_program->buildProgram(vertex_shader, frag_shader)) {
        return true;
    }
    return m_program->link();
}

GLuint evo::BaseRender::get_program_id() {
    if (m_program) {
        return m_program->getProgram();
    } else {
        return 0;
    }
}

evo::BaseRender::~BaseRender() {
    delete m_program;
    m_program = 0;
}