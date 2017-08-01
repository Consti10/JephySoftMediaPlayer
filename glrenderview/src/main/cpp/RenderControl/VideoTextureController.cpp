//
// Created by zhuyuanxuan on 25/11/2016.
// Copyright (c) 2016 Evomotion. All rights reserved.
//

#include "VideoTextureController.h"

#include "ShaderCommon.h"
#include "LogUtils.h"

/**
 * Video Texture Controller
 */
const char TAG[] = {"Video Texture Controller"};


evo::VideoTextureController::VideoTextureController(BaseRenderController *renderer, int yuv_format)
        : BaseTextureController(renderer), m_yuv_format(yuv_format) {
    m_y_tex_name = 0;
    m_uv_tex_name = 0;
    m_logo_tex_data = nullptr;
}

evo::VideoTextureController::~VideoTextureController() {
    LOGI(TAG, "Destruct VideoTextureController");
    if (m_texture_num > 0) {
        glDeleteTextures(m_texture_num, m_texture_list);
        LOGI(TAG, "delete %d textures No. %d ~ %d",
             m_texture_num, m_texture_list[0], m_texture_list[m_texture_num - 1]);
    }
    if (m_logo_tex_data != nullptr) {
        glDeleteTextures(1, m_texture_list + 3);
        delete m_logo_tex_data;
        m_logo_tex_data = nullptr;
    }
}

void evo::VideoTextureController::Init() {
    GLuint program = m_renderer->get_program_id();
    if (program == 0) {
        LOGE(TAG, "Program error in video texture controller");
        return;
    }
    glUseProgram(program);

    VideoTextureController::InitSamplerLoc();

    GLint mat_index = glGetUniformLocation(program, kColorMat);
    glUniformMatrix3fv(mat_index, 1, GL_FALSE, yuv_mat_hdtv);

    LOGI(TAG, "Initiate video texture controller");
    evo::CheckGlError("upload color mat");
}

TextureType evo::VideoTextureController::get_texture_type() {
    return VIDEO;
}

bool evo::VideoTextureController::ReActiveTex() {
    Init();
    if (m_create_texture_myself) {
        // here bind texture again
        GLuint program = m_renderer->get_program_id();
        glUseProgram(program);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, m_texture_list[0]);
        glUniform1i(m_sampler_y_index, 0);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, m_texture_list[1]);
        glUniform1i(m_sampler_u_index, 1);

        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, m_texture_list[2]);
        glUniform1i(m_sampler_v_index, 2);

        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, m_texture_list[3]);
        glUniform1i(m_logo_sampler_index, 3);

        LOGI(TAG, "Reactive video texture controller");

        return CheckGlError("Reactive texture in VideoTextureController");
    }

    // use texture created outside, so should bind again.
    if (m_y_tex_name && m_uv_tex_name) {
        SetYuvTexName(m_y_tex_name, m_uv_tex_name);

        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, m_texture_list[3]);
        glUniform1i(m_logo_sampler_index, 3);
    }
    return false;
}

bool evo::VideoTextureController::SetRgbTextureName(GLuint name) {
    return true;
}

bool evo::VideoTextureController::SetYuvTexName(GLuint y_name, GLuint uv_name) {
    m_create_texture_myself = false;
    // y texture
    m_y_tex_name = y_name;
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, y_name);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glUniform1i(m_sampler_y_index, 0);

    // uv texture
    m_uv_tex_name = uv_name;
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, uv_name);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glUniform1i(m_sampler_uv_index, 1);

    return evo::CheckGlError("Set yuv texture name");
}

bool evo::VideoTextureController::SetBmpDataTexture(void *data, int width, int height,
                                                    int32_t format) {
    return true;
}

bool
evo::VideoTextureController::SetYuvTexData(GLubyte *y_data, GLubyte *u_data, GLubyte *v_data,
                                           int width, int height) {
    m_create_texture_myself = true;
    GLuint program = m_renderer->get_program_id();
    glUseProgram(program);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_texture_list[0]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, width, height, 0, GL_RED, GL_UNSIGNED_BYTE, y_data);
    glUniform1i(m_sampler_y_index, 0);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, m_texture_list[1]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, width / 2, height / 2, 0, GL_RED, GL_UNSIGNED_BYTE,
                 u_data);
    glUniform1i(m_sampler_u_index, 1);

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, m_texture_list[2]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, width / 2, height / 2, 0, GL_RED, GL_UNSIGNED_BYTE,
                 v_data);
    glUniform1i(m_sampler_v_index, 2);

    return CheckGlError("upload yuv texture");
}

bool evo::VideoTextureController::SetLogoBmpData(void *data, int width, int height) {
    if (m_logo_tex_data != nullptr) {
        delete m_logo_tex_data;
    }

    GLuint program = m_renderer->get_program_id();
    glUseProgram(program);

    size_t data_size = (size_t) (width * height * 4);
    m_logo_tex_data = new uint8_t[data_size];
    memcpy(m_logo_tex_data, data, data_size);

    glDeleteTextures(1, m_texture_list + 3);
    glGenTextures(1, m_texture_list + 3);

    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, m_texture_list[3]);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA,
                 GL_UNSIGNED_BYTE, m_logo_tex_data);
    glUniform1i(m_logo_sampler_index, 3);

    if (CheckGlError("Upload logo texture")) {
        return 1;
    }
    m_texture_num = 4;
    LOGI(TAG, "set logo/watermark texture %d upload to loc:%d.",
         m_texture_list[3], m_logo_sampler_index);

    return false;
}

bool evo::VideoTextureController::BindTexture() {
    if (m_y_tex_name && m_uv_tex_name) {
        // ios
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, m_y_tex_name);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, m_uv_tex_name);

        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, m_texture_list[3]);
    } else {
        // android
        for (int i = 0; i < m_texture_num; ++i) {
            glActiveTexture((GLenum) (GL_TEXTURE0 + i));
            glBindTexture(GL_TEXTURE_2D, m_texture_list[i]);
        }
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, m_texture_list[3]);
    }

    return false;
}

/**
 * Generate texture objects depend on the yuv format
 * @param type texture type
 * @return 0 if no error
 */
int evo::VideoTextureController::GenTextures(TextureType type) {
    m_create_texture_myself = true;
    switch (type) {
        case YUV420P:
            m_texture_num = 3;
            break;
        case NV12:
            m_texture_num = 2;
            break;
        default:
            LOGE(TAG, "Not supported video textures yet");
            return -1;
    }

    glGenTextures(m_texture_num, m_texture_list);
    for (int i = 0; i < m_texture_num; ++i) {
        glActiveTexture((GLenum) (GL_TEXTURE0 + i));
        glBindTexture(GL_TEXTURE_2D, m_texture_list[i]);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }
    LOGI(TAG, "Generate %d textures.", m_texture_num);
    return CheckGlError("Gen video textures");
}

bool evo::VideoTextureController::SetNV12TexData(GLubyte *y_data, GLubyte *uv_data,
                                                 int width, int height) {

    m_create_texture_myself = true;
    GLuint program = m_renderer->get_program_id();
    glUseProgram(program);
    if (CheckGlError("glUseProgram")) {
        return false;
    }
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_texture_list[0]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, width, height, 0, GL_RED, GL_UNSIGNED_BYTE, y_data);
    glUniform1i(m_sampler_y_index, 0);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, m_texture_list[1]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_LUMINANCE_ALPHA, width / 2, height / 2, 0,
                 GL_LUMINANCE_ALPHA, GL_UNSIGNED_BYTE, uv_data);
    glUniform1i(m_sampler_uv_index, 1);
//    LOGI(TAG, "Upload nv12 texture %d %d to %d %d", m_texture_list[0], m_texture_list[1], m_sampler_y_index, m_sampler_uv_index);
    return CheckGlError("upload yuv texture");
}

/*
 * Private
 */

void evo::VideoTextureController::InitSamplerLoc() {
    GLuint program = m_renderer->get_program_id();
    if (program == 0) {
        return;
    }
    m_sampler_y_index = glGetUniformLocation(program, kSamplerY);
    m_sampler_uv_index = glGetUniformLocation(program, kSamplerUV);

    m_sampler_u_index = glGetUniformLocation(program, kSamplerU);
    m_sampler_v_index = glGetUniformLocation(program, kSamplerV);
    m_logo_sampler_index = glGetUniformLocation(program, kSamplerLogo);
}
