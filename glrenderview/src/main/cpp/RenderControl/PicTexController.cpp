//
// Created by zhuyuanxuan on 02/12/2016.
// Copyright (c) 2016 Evomotion. All rights reserved.
//

#include "PicTexController.h"

#include "LogUtils.h"
#include "ShaderCommon.h"
#include "VertexData.h"

#include <math.h>

namespace evo {
    enum AndroidBitmapFormat {
        ANDROID_BITMAP_FORMAT_NONE = 0,
        ANDROID_BITMAP_FORMAT_RGBA_8888 = 1,
        ANDROID_BITMAP_FORMAT_RGB_565 = 4,
        ANDROID_BITMAP_FORMAT_RGBA_4444 = 7,
        ANDROID_BITMAP_FORMAT_A_8 = 8,
    };

    const char TAG[] = {"PicTexControllerAndr"};

    PicTexController::PicTexController(BaseRenderController *renderer)
            : BaseTextureController(renderer) {
        m_tex_data_ok = false;
        m_tex_data = nullptr;
        m_logo_tex_data = nullptr;
    }

    PicTexController::~PicTexController() {
        if (m_tex_data) {
            delete m_tex_data;
            m_tex_data = nullptr;
        }

        if (m_logo_tex_data) {
            delete m_logo_tex_data;
            m_logo_tex_data = nullptr;
        }

        glDeleteTextures(m_texture_num, m_texture_list);
        LOGI(TAG, "release texture data!");
    }

    void PicTexController::Init() {
        PicTexController::InitSamplerLoc();
        // if textures name didn't correspond to existing textures, will ignore it.
        glDeleteTextures(1, m_texture_list);

        glGenTextures(1, m_texture_list);
        glBindTexture(GL_TEXTURE_2D, m_texture_list[0]);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        glBindTexture(GL_TEXTURE_2D, 0);
        LOGI(TAG, "Init texture: gen and set parameter of texture %u", m_texture_list[0]);

        if (evo::CheckGlError("Init Texture")) {
            return;
        }
        m_texture_ok = true;
        m_texture_num = 1;
    }


    int PicTexController::GenTextures(TextureType type) {
        return 0;
    }

    bool PicTexController::SetRgbTextureName(GLuint name) {
        m_create_texture_myself = false;
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, name);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glUniform1i(m_rgb_sampler_index, 0);

        m_texture_list[0] = name;
        return evo::CheckGlError("upload rgb texture");
    }

    bool PicTexController::SetYuvTexName(GLuint y_name, GLuint uv_name) {
        return true;
    }

    bool PicTexController::SetBmpDataTexture(void *data, int width, int height,
                                             int32_t format) {
        if (!m_texture_ok) {
            return 1;
        }
        m_create_texture_myself = true;

        if (format == ANDROID_BITMAP_FORMAT_RGBA_8888) {
            GLint size = width * height * 4;
            m_width = width;
            m_height = height;

            m_tex_data = new uint8_t[size];
            memcpy(m_tex_data, data, (size_t) size);

            // set texture
            GLuint texture = m_texture_list[0];
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, texture);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_width, m_height,
                         0, GL_RGBA, GL_UNSIGNED_BYTE, m_tex_data);
            glUniform1i(m_rgb_sampler_index, 0);
            LOGI(TAG, "set RGB data of texture %d upload to loc:%d.", texture, m_rgb_sampler_index);
            m_tex_data_ok = true;
            return evo::CheckGlError("upload texture");
        } else if (format == ANDROID_BITMAP_FORMAT_RGB_565) {
            LOGI(TAG, "Set texture with rgb565");
            GLint size = width * height * 2;
            m_width = width;
            m_height = height;

            m_tex_data = new uint8_t[size];
            memcpy(m_tex_data, data, (size_t) size);

            // set texture
            GLuint texture = m_texture_list[0];
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, texture);

            // should use GL_UNSIGNED_SHORT_5_6_5 if use GL_UNSIGNED_BYTE, error occurs!
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB565, m_width, m_height,
                         0, GL_RGB, GL_UNSIGNED_SHORT_5_6_5, m_tex_data);
            glUniform1i(m_rgb_sampler_index, 0);
            LOGI(TAG, "set RGB data of texture %d upload to loc:%d.", texture, m_rgb_sampler_index);
            m_tex_data_ok = true;
            return evo::CheckGlError("upload texture");
        }
        return 1;
    }

    bool PicTexController::SetLogoBmpData(void *data, int width, int height) {
        // prepare data
        if (m_logo_tex_data != nullptr) {
            delete m_logo_tex_data;
        }

        size_t size = (size_t) (width * height * 4);
        m_logo_tex_data = new uint8_t[size];
        memcpy(m_logo_tex_data, data, size);

        // create a new texture object
        glDeleteTextures(1, m_texture_list + 1);
        glGenTextures(1, m_texture_list + 1);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, m_texture_list[1]);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA,
                     GL_UNSIGNED_BYTE, m_logo_tex_data);

        glUniform1i(m_logo_sampler_index, 1);

        if (CheckGlError("Upload logo texture")) {
            return 1;
        }
        LOGI(TAG, "set logo/watermark texture %d upload to loc:%d.",
             m_texture_list[1], m_logo_sampler_index);

        m_texture_num = 2;

        // active the last texture object
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, m_texture_list[0]);
        return false;
    }

    // get texture sampler location index AGAIN!
// then active and bind texture again.
    bool PicTexController::ReActiveTex() {
        PicTexController::InitSamplerLoc();
        if (m_create_texture_myself) {
            if (m_tex_data_ok) {
                // set texture
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, m_texture_list[0]);
                glUniform1i(m_rgb_sampler_index, 0);

                glActiveTexture(GL_TEXTURE1);
                glBindTexture(GL_TEXTURE_2D, m_texture_list[1]);
                glUniform1i(m_logo_sampler_index, 1);

                LOGI(TAG, "Reactive: set RGB and logo tex in %u and %u.", m_texture_list[0],
                        m_texture_list[1]);
            }
        } else {
            // ios: use texture outside
            // set rgb texture name again
            PicTexController::SetRgbTextureName(m_texture_list[0]);
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D, m_texture_list[1]);
            glUniform1i(m_logo_sampler_index, 1);
            LOGI(TAG, "Reset logo texture %u to location %d.",
                    m_texture_list[1], m_logo_sampler_index);
        }

        return CheckGlError("Reactive texture");
    }

    bool PicTexController::BindTexture() {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, m_texture_list[0]);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, m_texture_list[1]);
        return false;
    }

    TextureType PicTexController::get_texture_type() {
        return IMAGE;
    }

    bool PicTexController::SetNV12TexData(GLubyte *y_data, GLubyte *uv_data, int width, int height) {
        return 1;
    }

/**
 * private functions
 */

    void PicTexController::InitSamplerLoc() {
        GLuint program = m_renderer->get_program_id();
        if (program == 0) {
            LOGE(TAG, "Program error");
            return;
        }
        glUseProgram(program);
        m_rgb_sampler_index = glGetUniformLocation(program, kSamplerRgb);
        m_logo_sampler_index = glGetUniformLocation(program, kSamplerLogo);
        CheckGlError("get rgb sampler index");
    }

    // we assume texture are 32-bit(RGBA8888) per pixel
    bool PicTexController::SetBmpAndLogoData(void *tex, void *logo, int tex_width, int tex_height,
                                             int logo_width, int logo_height) {
        if (!m_texture_ok) {
            return 1;
        }

        size_t tex_size = (size_t) (tex_width * tex_height * 4);
        size_t logo_size = (size_t) (logo_width * logo_height * 4);

        m_tex_data = new uint8_t[tex_size];
        uint8_t *logo_data = new uint8_t[logo_size];

        memcpy(m_tex_data, tex, tex_size);
        memcpy(logo_data, logo, logo_size);

        double biggest_z = kRadiusOfSphere * sin(0.1f * M_PI);
        for (int row = (int) (tex_height * 0.9); row < tex_height; ++row) {
            for (int column = 0; column < tex_width; ++column) {
                int texture_index = (row * tex_width + column) * 4;
                double theta = (1.0f - (row * 1.0f / tex_height)) * M_PI;
                double varphi = column * 1.0f / tex_width * M_PI * 2;

                double x = kRadiusOfSphere * sin(theta) * cos(varphi);
                double y = kRadiusOfSphere * sin(theta) * sin(varphi);
                // something interesting!
                double x_div_z = x / biggest_z;
                double y_div_z = y / biggest_z;

                int logo_row = (int) ((y_div_z + 1.0f) * 0.5f * logo_height);
                int logo_column = (int) ((x_div_z + 1.0f) * 0.5f * logo_width);
                int logo_index = logo_row * logo_width + logo_column;

                uint8_t *src_logo_data = logo_data + logo_index * 4;
                uint8_t *dest_tex_data = m_tex_data + texture_index;

                // blend color
                float alpha = src_logo_data[3] / 255.0f;
                dest_tex_data[0] = (uint8_t) (alpha * src_logo_data[0] +
                                              (1.0f - alpha) * dest_tex_data[0]);
                dest_tex_data[1] = (uint8_t) (alpha * src_logo_data[1] +
                                              (1.0f - alpha) * dest_tex_data[1]);
                dest_tex_data[2] = (uint8_t) (alpha * src_logo_data[2] +
                                              (1.0f - alpha) * dest_tex_data[2]);
                dest_tex_data[3] = 255;
            }
        }

        delete[] logo_data;

        // set texture
        GLuint texture = m_texture_list[0];
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, tex_width, tex_height,
                     0, GL_RGBA, GL_UNSIGNED_BYTE, m_tex_data);
        glUniform1i(m_rgb_sampler_index, 0);
        LOGI(TAG, "set RGB data of texture %d upload to loc:%d.", texture, m_rgb_sampler_index);
        m_tex_data_ok = true;
        return CheckGlError("upload texture");
    }
}
