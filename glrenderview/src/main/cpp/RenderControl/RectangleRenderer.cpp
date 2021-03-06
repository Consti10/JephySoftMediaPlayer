//
// Created by zhuyuanxuan on 16/02/2017.
// Copyright (c) 2017 Evomotion. All rights reserved.
//

#include "RectangleRenderer.h"
#include "TextureCommon.h"

using namespace evo;

const char rectangleVertexShader[] =
        "#version 300 es\n"
                "in vec4 vPosition;\n"
                "in vec4 vTexCoord;\n"
                "out vec2 v_TexCoord;\n"
                "void main() { \n"
                "    gl_Position = vPosition;\n"
                "    v_TexCoord.x = vTexCoord.x;\n"
                "    v_TexCoord.y = vTexCoord.y;\n"
                "}\n";

const char rectangleFragmentShader[] =
        "#version 300 es\n"
                "precision mediump float;\n"
                "uniform sampler2D yTexture;\n"
                "uniform sampler2D uvTexture;\n"
                "in vec2 v_TexCoord;\n"
                "out vec4 color_out;\n"
                "uniform mat3 yuv2rgb;\n"
                "void main() {\n"
                "    vec3 color;\n"
                "    color.r = texture(yTexture, v_TexCoord).r - 16.0 / 255.0;\n"
                "    color.gb = texture(uvTexture, v_TexCoord).ra - 128.0 / 255.0;\n"
                "    color_out = vec4(yuv2rgb * color, 1.0f);\n"
                "}\n";

const char *position_str = "vPosition";
const char *tex_coord_str = "vTexCoord";
const char *transform_mat_str = "yuv2rgb";
const char *y_tex_str = "yTexture";
const char *uv_tex_str = "uvTexture";

float vertex_val[] = {
        -1, -1, 0,
        -1, 1, 0,
        1, -1, 0,
        1, 1, 0};
float tex_coord_val[] = {
        0, 1,
        0, 0,
        1, 1,
        1, 0};

GLuint RectangleRenderer::get_program() {
    if (m_program) {
        return m_program;
    }
    return 0;
}

int RectangleRenderer::InitGl() {
    m_renderer = new BaseRender();
    m_renderer->InitProgram(rectangleVertexShader, rectangleFragmentShader);
    m_program = m_renderer->get_program_id();

    GLint position_index = glGetAttribLocation(m_program, position_str);
    GLint tex_coord_index = glGetAttribLocation(m_program, tex_coord_str);

    GLuint vao;
    m_renderer->InitVao(&vao, (GLuint) position_index, (GLuint) tex_coord_index,
            &vertex_val[0], &tex_coord_val[0], 4);
    if (CheckGlError("init vao in rectangle renderer")) {
        return 1;
    } else {
        m_vao = vao;
        return 0;
    }
}

int RectangleRenderer::InitUniforms(int width, int height) {
    glUseProgram(m_program);

    GLint yuv2rgb_mat_loc = glGetUniformLocation(m_program, transform_mat_str);

    glUniformMatrix3fv(yuv2rgb_mat_loc, 1, GL_FALSE, yuv_mat_hdtv);

    glViewport(0, 0, width, height);

    return 0;
}

int RectangleRenderer::SetYuvTextureName(unsigned int y, unsigned int uv) {
    glUseProgram(m_program);

    GLint y_sampler_loc = glGetUniformLocation(m_program, y_tex_str);
    GLint uv_sampler_loc = glGetUniformLocation(m_program, uv_tex_str);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, y);

    // no need to do glTexParameteri() because the texture is generated by
    // CVOpenGLESTextureCacheCreateTextureFromImage

    glUniform1i(y_sampler_loc, 0);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, uv);
    glUniform1i(uv_sampler_loc, 1);

    return CheckGlError("set texture");
}

int RectangleRenderer::draw() {
    glClearColor(0, 0, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(m_program);
    glBindVertexArray(m_vao);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    return CheckGlError("draw");
}
