//
// Created by zhuyuanxuan on 01/12/2016.
// Copyright (c) 2016 Evomotion. All rights reserved.
//

#include "VrRenderController.h"

#include "LogUtils.h"
#include "VertexData.h"
#include "ShaderCommon.h"
#include "matrix_transform.hpp"
#include "type_ptr.hpp"

const char TAG[] = {"VR Render Controller"};
const GLfloat kClearColor = 0;
const float kNear = 0.1f;
const float kFar = 100.0f;

evo::VrRenderController::VrRenderController(TextureType type) : BaseRenderController(type) {
    m_lookat_mat4 = new GLfloat[16];
    m_projection_mat4 = new GLfloat[16];
    m_renderer = new BaseRender();
    m_update_param = 0;
    m_texture_num = 0;
    m_mode = VR;
}

evo::VrRenderController::~VrRenderController() {
    delete m_lookat_mat4;
    delete m_projection_mat4;

    m_vao = 0;
    delete m_renderer;
    m_renderer = 0;

    if (m_texture_num) {
        glDeleteTextures(m_texture_num, m_textures);
        LOGI(TAG, "delete %d textures No. %d ~ %d",
             m_texture_num, m_textures[0], m_textures[m_texture_num - 1]);
        m_texture_num = 0;
    }
}

bool evo::VrRenderController::InitGl() {
    BaseRenderController::InitGl();

    bool result;
    if (m_texture_type == YUV420P) {
        result = m_renderer->InitProgram(kVrVerStr, kVrYuvWithLogoFrag);
        if (result) {
            LOGE(TAG, "Init video program fail");
            return 1;
        }

        m_program_id = m_renderer->get_program_id();
        evo::CheckGlError("init video program");
    } else if (m_texture_type == NV12) {
#ifdef __ANDROID_API__
        result = m_renderer->InitProgram(kVrVerStr, kVrFragNV12WithLogo);
#else
        result = m_renderer->InitProgram(kSphereVerStr, kSphereFragNV12WithLogo);
#endif
        if (result) {
            LOGE(TAG, "Init video program fail");
            return 1;
        }

        m_program_id = m_renderer->get_program_id();
        evo::CheckGlError("init video program");
    } else {
        // init image program
#ifdef __ANDROID__
        result = m_renderer->InitProgram(kVrVerStr,
                                         kVrImgFragwithLogo);
#else
        result = m_renderer->InitProgram(kSphereVerStr,
                kVrImgFragwithLogo);
#endif
        if (result) {
            LOGE(TAG, "Init image program fail");
            return 1;
        }

        m_program_id = m_renderer->get_program_id();
        evo::CheckGlError("init image program");
    }

    LOGI(TAG, "init program %x success.", m_program_id);

    // init VAO
    GLuint vao;
    // first: get all attribute handler
    GLint position = glGetAttribLocation(m_program_id, kPositionName);
    GLint tex_coord = glGetAttribLocation(m_program_id, kTexCoordName);
    // use handler and vertex/tex_coord values to init vao
    m_renderer->InitVao(&vao, (GLuint) position, (GLuint) tex_coord,
                        kSphereVertexs, kSphereTexcoords, kVertexNums);
    m_vao = vao;

    if (vao == 0) {
        return true;
    } else {
        LOGI(TAG, "Create vao: %x", m_vao);
    }

    return false;
}

bool evo::VrRenderController::Draw() {
    glClearColor(kClearColor, kClearColor, kClearColor, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // first, make sure program and vao is valid
    if (m_program_id && m_renderer) {
//        LOGI(TAG, "Program id %x", m_program_id);
        glUseProgram(m_program_id);
        if (m_vao) {
            //re upload vertex again, make sure vertex is OK.
            GLint position = glGetAttribLocation(m_program_id, kPositionName);
            GLint texcoord = glGetAttribLocation(m_program_id, kTexCoordName);

            // gen vbo
            GLuint vbo[] = {0, 0};
            glGenBuffers(2, vbo);

            glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
            glBufferData(GL_ARRAY_BUFFER, 3 * kVertexNums * sizeof(float), kSphereVertexs,
                         GL_STATIC_DRAW);
            glVertexAttribPointer((GLuint) position, 3, GL_FLOAT, GL_FALSE, 0, 0);

            glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
            glBufferData(GL_ARRAY_BUFFER, 2 * kVertexNums * sizeof(float), kSphereTexcoords,
                         GL_STATIC_DRAW);
            glVertexAttribPointer((GLuint) texcoord, 2, GL_FLOAT, GL_FALSE, 0, 0);
            glBindBuffer(GL_ARRAY_BUFFER, 0);

            glEnableVertexAttribArray((GLuint) position);
            glEnableVertexAttribArray((GLuint) texcoord);

            if (m_update_param) {
                if (UpdateParam()) {
                    return true;
                }
                // reset m_update_param
                m_update_param = false;
            }
            // draw
            glDrawArrays(GL_TRIANGLES, 0, kVertexNums);

            // release buffer
            glDeleteBuffers(2, vbo);
        }
    }
    return evo::CheckGlError("Draw");
}

bool evo::VrRenderController::InitUniforms(int width, int height) {
    BaseRenderController::InitUniforms(width, height);

    glUseProgram(m_program_id);
    // set uniform value
    // first get handler
    m_projection_mat_index = glGetUniformLocation(m_program_id, kProjectionName);
    m_model_index = glGetUniformLocation(m_program_id, kModelName);
    m_lookat_index = glGetUniformLocation(m_program_id, kLookatName);

    // calculate initial value
    // model
    glm::mat4 mat4;
    glm::vec3 ratio = glm::vec3(1.0f);
    glm::mat4 model_mat = glm::scale(mat4, ratio);
    glUniformMatrix4fv(m_model_index, 1, GL_FALSE, glm::value_ptr(model_mat));
    // view/camera/look at
    glm::mat4 lookat_mat = glm::lookAt(glm::vec3(0, 0, 0),
                                       glm::vec3(0, 0, -1.0f),  // look at -z direction
                                       glm::vec3(0, 1.0f, 0));
    glUniformMatrix4fv(m_lookat_index, 1, GL_FALSE, glm::value_ptr(lookat_mat));
    // projection
    glm::mat4 projection_mat = glm::perspective(kFov, m_aspect, kNear, kFar);
    glUniformMatrix4fv(m_projection_mat_index, 1, GL_FALSE, glm::value_ptr(projection_mat));

    return evo::CheckGlError("init uniforms");
}

void evo::VrRenderController::UpdateLookat(float *mat4) {
    for (int i = 0; i < 16; ++i) {
        m_lookat_mat4[i] = mat4[i];
    }
    m_update_param = true;
}

void evo::VrRenderController::UpdateProjection(float *mat4) {
    for (int i = 0; i < 16; ++i) {
        m_projection_mat4[i] = mat4[i];
    }
    m_update_param = true;
}

void evo::VrRenderController::OnSpanXY(float x, float y) {

}

void evo::VrRenderController::OnZoom(float factor) {

}

void evo::VrRenderController::SetFirstTextureName(GLuint texture_name) {
    m_textures[0] = texture_name;
}

/**
 * private functions
 */

// Update the look at matrix, projection matrix and model matrix(for rotate)
bool evo::VrRenderController::UpdateParam() {
    glUniformMatrix4fv(m_lookat_index, 1, GL_FALSE, m_lookat_mat4);
    glUniformMatrix4fv(m_projection_mat_index, 1, GL_FALSE,
                       m_projection_mat4);
    return false;
}
