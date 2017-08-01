//
// Created by zhuyuanxuan on 01/12/2016.
// Copyright (c) 2016 Evomotion. All rights reserved.
//

#include "SphereRenderController.h"

#include "matrix_transform.hpp"
#include "type_ptr.hpp"
#include "LogUtils.h"
#include "VertexData.h"
#include "ShaderCommon.h"

const GLfloat kClearColor = 0;
const float kNear = 0.1f;
const float kFar = 100.0f;
const float kMaxZoom = 0.3f;
const float kMinZoom = -2.0f;
const float kMaxZoomFisheye = 0.5f;
const float kMinZoomFisheye = -0.86f;  // get value by test
const float kMaxRad = (const float) (M_PI_2 * 0.9);
const float kZero = 0.00001;
const float kRadius = kRadiusOfSphere;
const char TAG[] = {"Sphere RenderController"};

evo::SphereRenderController::SphereRenderController(TextureType type)
        : BaseRenderController(type) {
    m_mode = NULL_MODE;
    InitEveryThing();
}

evo::SphereRenderController::SphereRenderController(TextureType m_texture_type, DisplayMode m_mode)
        : BaseRenderController(m_texture_type, m_mode) {
    InitEveryThing();
}

evo::SphereRenderController::~SphereRenderController() {
    m_vao = 0;
    delete m_renderer;
    m_renderer = 0;

    if (m_rotate_mat4) {
        delete m_rotate_mat4;
        m_rotate_mat4 = nullptr;
    }
}

void evo::SphereRenderController::SetRotateZoomStatus(GLfloat lat, GLfloat lon, GLfloat factor) {
    BaseRenderController::SetRotateZoomStatus(lat, lon, factor);
    m_update_param = 1;
}

void evo::SphereRenderController::SetRotateStatus(GLfloat lat, GLfloat lon) {
    BaseRenderController::SetRotateStatus(lat, lon);
    if (m_lat > kMaxRad) {
        m_lat = kMaxRad;
    }
    if (m_lat + kMaxRad < kZero) {
        m_lat = kMaxRad * -1.0f;
    }

    m_update_param = 1;
}

void evo::SphereRenderController::set_lat(GLfloat m_lat) {
    BaseRenderController::set_lat(m_lat);
    m_update_param = 1;
}

bool evo::SphereRenderController::InitUniforms(int width, int height) {
    BaseRenderController::InitUniforms(width, height);

    glUseProgram(m_program_id);

    GLint yuv2rgb_mat_loc = glGetUniformLocation(m_program_id, kColorMat);

    glUniformMatrix3fv(yuv2rgb_mat_loc, 1, GL_FALSE, yuv_mat_hdtv);

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
    glm::mat4 lookat_mat = glm::lookAt(m_camera_pos,
                                       m_lookat_point,
                                       glm::vec3(0, 1.0f, 0));
    glUniformMatrix4fv(m_lookat_index, 1, GL_FALSE, glm::value_ptr(lookat_mat));

    // projection

    /* Add this code to fix
     * <a href="http://chandao.chinacloudapp.cn/zentao/bug-view-1458.html">bug #1458</a>
     * When little planet to sphere, at the same time, screen rotate, should prevent the fov keep
     * the old fov of little planet
     */
    if (m_animate_status == RENDER_TRANSFORM) {
        m_fov = kFov;
    }

    glm::mat4 projection_mat = glm::perspective(m_fov, m_aspect, kNear, kFar);
    glUniformMatrix4fv(m_projection_mat_index, 1, GL_FALSE, glm::value_ptr(projection_mat));

    m_update_param = 1;
    return evo::CheckGlError("init uniforms");
}


bool evo::SphereRenderController::InitGl() {
    BaseRenderController::InitGl();

    bool result;
    if (m_texture_type == NV12) {
        result = m_renderer->InitProgram(kSphereVerStr, kSphereFragNV12WithLogo);
        m_program_id = m_renderer->get_program_id();
        evo::CheckGlError("init video program");
    } else if (m_texture_type == YUV420P) {
        result = m_renderer->InitProgram(kSphereVerStr, kSphereYUV420PWithLogoFrag);
        m_program_id = m_renderer->get_program_id();
        evo::CheckGlError("init video program");
    } else {
        // init image program
        result = m_renderer->InitProgram(kSphereImgVerStr,
                                         kSphereImgFragStrWithLogo);
        evo::CheckGlError("init image program");
        m_program_id = m_renderer->get_program_id();
    }
    if (result) {
        LOGE(TAG, "Init program error!");
        return true;
    } else {
        LOGI(TAG, "init program %x", m_program_id);
    }

    // init VAO
    GLuint vao;
    // first: get all attribute handler
    GLint position = glGetAttribLocation(m_program_id, kPositionName);
    GLint texcoord = glGetAttribLocation(m_program_id, kTexCoordName);
    // use handler and vertex/texcoord values to init vao
    m_renderer->InitVao(&vao, (GLuint) position, (GLuint) texcoord,
                        kSphereVertexs, kSphereTexcoords, kVertexNums);

    m_vao = vao;

    if (vao == 0) {
        LOGE(TAG, "Init VAO error");
        return true;
    } else {
        LOGI(TAG, "vao %x", m_vao);
    }

    glClearColor(kClearColor, kClearColor, kClearColor, 1.0f);
    return false;
}

// calculate the look at point on span and reset the camera view matrix.
// first, reverse y value,
// the max value of delta y is fov;
// while max value of delta x is `2 * arctan(tan(fov / 2) / (height / width))`
// since `tan(90 / 2) / tan (x / 2) = height / width`
// and m_aspect = width / height.
void evo::SphereRenderController::OnSpanXY(float x, float y) {
    if (m_animate_status != RENDER_DRAWING) {
        return;
    }
    GLfloat delta_latitude = y * m_fov * -1.0f;
    GLfloat delta_longitude = (GLfloat) (x * 2.0 * atanf(tanf((float) (m_fov / 2.0)) * m_aspect));

    m_lat += delta_latitude;
    m_lon += delta_longitude;

    if (m_lat > kMaxRad) {
        m_lat = kMaxRad;
    }
    if (m_lat + kMaxRad < kZero) {
        m_lat = kMaxRad * -1.0f;
    }
    m_update_param = true;
}

// factor: get zoom parameter by `factor - 1.0f`
// here we think: zoom out -> factor > 1; zoom in -> factor < 1.
void evo::SphereRenderController::OnZoom(float factor) {
    if (m_animate_status != RENDER_DRAWING) {
        return;
    }
    if (m_mode == SPHERE) {
        m_zoom_factor += (factor - 1.0f);
        m_zoom_factor = (GLfloat) (m_zoom_factor > kMaxZoom ?
                                   kMaxZoom : m_zoom_factor < kMinZoom ?
                                              kMinZoom : m_zoom_factor);
    } else {
        m_fov_factor += (factor - 1.0f);
        m_fov_factor = (GLfloat) (m_fov_factor > kMaxZoomFisheye ?
                                  kMaxZoomFisheye : m_fov_factor < kMinZoomFisheye ?
                                                    kMinZoomFisheye : m_fov_factor);
    }
    m_update_param = true;
}

void evo::SphereRenderController::OnSensorRotateXY(float x_axis, float y_axis) {
    BaseRenderController::OnSensorRotateXY(x_axis, y_axis);

    m_lat += x_axis;
    m_lon += y_axis;

    if (m_lat > kMaxRad) {
        m_lat = kMaxRad;
    }
    if (m_lat + kMaxRad < kZero) {
        m_lat = kMaxRad * -1.0f;
    }
    m_update_param = true;
}

void evo::SphereRenderController::SetRotateMat(float *rotate_mat) {
    if (m_animate_status == RENDER_TRANSFORM) {
        // When turn the gyroscope on and click switch mode,
        // should reset the rotation matrix first
        // Don't touch the zoom factor because it diff in different display mode
//        m_zoom_factor = 0;
        if (nullptr == m_rotate_mat4) {
            m_rotate_mat4 = new float[16];
        }
        memcpy(m_rotate_mat4, glm::value_ptr(glm::mat4(1.0)), sizeof(float) * 16);
        m_update_param = true;
        return;
    }

    if (m_animate_status != RENDER_DRAWING) {
        return;
    }

    if (rotate_mat[0] == -1.0f && rotate_mat[1] == -1.0f) {
        m_zoom_factor = 0;
        if (nullptr == m_rotate_mat4) {
            m_rotate_mat4 = new float[16];
        }
        memcpy(m_rotate_mat4, glm::value_ptr(glm::mat4(1.0)), sizeof(float) * 16);
        m_update_param = true;
        return;
    }

    // TODO 20170214 yxzhu refactor, use a common status to judge
    // reset the parameters first
    m_zoom_factor = 0;
    m_lat = 0;
    m_lon = 0;
    if (nullptr == m_rotate_mat4) {
        m_rotate_mat4 = new float[16];
    }
    memcpy(m_rotate_mat4, rotate_mat, 16 * sizeof(float));

    m_update_param = true;
}

bool evo::SphereRenderController::Draw() {
    if (m_animate_status == RENDER_NULL_STATUS) {
        m_animate_status = RENDER_DRAWING;
    }
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // add externalOES texture
//    GLint sampler = glGetUniformLocation(m_program_id, "Etexture");
//    glUniform1i(sampler, 0);

    // first, make sure program and vao is valid
    if (m_renderer == nullptr || m_program_id == 0) {
        return true;
    }

    glUseProgram(m_program_id);

    if (0 == m_vao) {
        return true;
    }

    glBindVertexArray(m_vao);

    if (m_update_param) {
        if (UpdateParam()) {
            return true;
        }
        // reset m_update_param
        m_update_param = false;
    }

    // draw
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDrawArrays(GL_TRIANGLES, 0, kVertexNums);
    return evo::CheckGlError("Draw");
}

int evo::SphereRenderController::XY2LatLon(float x, float y, float *lat, float *lon) {
    if (abs(m_aspect) < kZero) {
        LOGE(TAG, "aspect null %s", __PRETTY_FUNCTION__);
        return 1;
    }
    *lat = y * kFov * -1.0f;
    *lon = x * 2.0f * atanf(tanf((float) (kFov / 2.0f)) * m_aspect);
    return 0;
}

float evo::SphereRenderController::get_fov() const {
    return m_fov;
}

void evo::SphereRenderController::set_fov(float m_fov) {
    SphereRenderController::m_fov = m_fov;
    m_update_param = 1;
}

void evo::SphereRenderController::set_zoom_factor(float zoom) {
    BaseRenderController::set_zoom_factor(zoom);
    m_update_param = 1;
}

void evo::SphereRenderController::SetFovZoom(float fov, float zoom) {
    BaseRenderController::SetFovZoom(fov, zoom);
    m_fov = fov;
    m_zoom_factor = zoom;
    m_update_param = 1;
}

void evo::SphereRenderController::SetLatFov(float lat, float fov) {
    m_lat = lat;
    m_fov = fov;
    m_update_param = 1;
}

void evo::SphereRenderController::SetLatFovZoom(float lat, float fov, float zoom) {
    m_lat = lat;
    m_fov = fov;
    m_zoom_factor = zoom;
    m_update_param = 1;
}

void evo::SphereRenderController::ResetParams(DisplayMode mode) {
    if (mode == SPHERE) {
        m_fov_factor = 0;
        return;
    }

    if (mode == LITTLE_PLANET || mode == FISHEYE) {
        m_zoom_factor = -1.0f;
        return;
    }
}

void evo::SphereRenderController::set_face_culling(int m_face_culling) {
    SphereRenderController::m_face_culling = m_face_culling;
}

/**
 * private functions
 */

// When zoom factor change, change the position of camera, and the camera position is
// to do with the look at point.
//
//  look at  *
//            \
//             \
//  ball center *
//               \
//                \
//   camera pos    *
// When rotate, look at point change, the camera pos change too.
bool evo::SphereRenderController::UpdateParam() {
    // calculate look at
    GLfloat center_y = sinf(m_lat) * kRadius;
    GLfloat center_x = cosf(m_lat) * sinf(m_lon) * kRadius;
    GLfloat center_z = cosf(m_lat) * cosf(m_lon) * kRadius;
    glm::vec3 look_at_point = glm::vec3(center_x, center_y, -1.0f * center_z);

    //update zoom factor
    glm::vec3 camera_point;
    if (m_animate_status == RENDER_TRANSFORM) {
        camera_point = look_at_point * m_zoom_factor;
        LOGI(TAG, "Mode transform: fov = %f, zoom_factor = %f.", m_fov, m_zoom_factor);
//        LOGI(TAG, "Camera %f %f %f.", camera_point.x, camera_point.y, camera_point.z);
        // upload projection matrix
        glm::mat4 projection_mat = glm::perspective(m_fov, m_aspect, kNear, kFar);
        glUniformMatrix4fv(m_projection_mat_index, 1, GL_FALSE, glm::value_ptr(projection_mat));
//        camera_point = m_camera_pos;
    }
        // Not in transform animation, the zoom performance is diff between sphere and fisheye
    else if (m_mode == SPHERE) {
        camera_point = look_at_point * m_zoom_factor;
//        LOGE(TAG, "Draw sphere zoom: %f.", m_zoom_factor);
    } else {
        camera_point = look_at_point * -1.0f;
        // upload new projection matrix
        // fov 80~140 so: factor * 60 / 180
        m_fov = (GLfloat) (kFishEyeFov - m_fov_factor * M_PI * 60 / 180);
//        LOGI(TAG, "Fish/Little planet: field of view = %f, zoom_factor: %f.", m_fov, m_zoom_factor);
        glm::mat4 projection_mat = glm::perspective(m_fov, m_aspect, kNear, kFar);
        glUniformMatrix4fv(m_projection_mat_index, 1, GL_FALSE, glm::value_ptr(projection_mat));
    }

    if (m_animate_status == RENDER_SWITCH_DIRECTLY) {
        glm::mat4 projection_mat = glm::perspective(m_fov, m_aspect, kNear, kFar);
        glUniformMatrix4fv(m_projection_mat_index, 1, GL_FALSE, glm::value_ptr(projection_mat));
        m_animate_status = RENDER_DRAWING;
    }

//    LOGI(TAG, "camera point x %f; y %f; z %f;\n",
//         camera_point.x, camera_point.y, camera_point.z);

    glm::mat4 lookat_mat = glm::lookAt(camera_point, look_at_point,
                                       glm::vec3(0, 1.0f, 0));
    glUniformMatrix4fv(m_lookat_index, 1, GL_FALSE,
                       glm::value_ptr(lookat_mat));

    // judge whether to upload the camera position for culling
    if (m_mode != SPHERE || m_face_culling == 0) {
        GLfloat zeroPoint[3] = {0, 0, 0};
        glUniform3fv(glGetUniformLocation(m_program_id, kCameraPos), 1,
                     zeroPoint);
    } else if (m_zoom_factor < kZero) {
        // when zoom in:
        // calculate the distance and upload the camera position
        glUniform3fv(glGetUniformLocation(m_program_id, kCameraPos), 1,
                     glm::value_ptr(camera_point));
        evo::CheckGlError("uniform distance");
//        LOGI(TAG, "Upload camera position");
//        LOGI(TAG, "camera x %f y %f  z %f.\n",
//             camera_point.x, camera_point.y, camera_point.z);
    } else {
        GLfloat zeroPoint[3] = {0, 0, 0};
        glUniform3fv(glGetUniformLocation(m_program_id, kCameraPos), 1,
                     zeroPoint);
    }

    // Upload the rotate matrix
    if (m_rotate_mat4 != nullptr) {
        glUniformMatrix4fv(m_model_index, 1, GL_FALSE, m_rotate_mat4);
    }

    return false;
}

void evo::SphereRenderController::InitEveryThing() {
    // common initiate events
    m_renderer = new BaseRender();
    m_update_param = 0;
    m_rotate_mat4 = nullptr;
    m_face_culling = 1;

    // initiate fov, camera position and look at position
    switch (m_mode) {
        case SPHERE:
            m_fov = kFov;
            m_camera_pos = glm::vec3(0, 0, 0);
            m_lookat_point = glm::vec3(0, 0, -1);
            m_zoom_factor = 0;
            break;
        case FISHEYE:
            m_fov = kFishEyeFov;
            m_camera_pos = glm::vec3(0, 0, 0.5f);
            m_lookat_point = glm::vec3(0, 0, -1);
            m_zoom_factor = -1.0f;
            break;
        case LITTLE_PLANET: {
            m_fov = kFishEyeFov;
            // Must set latitude to the south point
            m_lat = -kMaxRad;
            GLfloat center_y = sinf(m_lat) * kRadius;
            GLfloat center_x = cosf(m_lat) * sinf(m_lon) * kRadius;
            GLfloat center_z = cosf(m_lat) * cosf(m_lon) * kRadius;
            m_lookat_point = glm::vec3(center_x, center_y, -1.0f * center_z);
            m_camera_pos = m_lookat_point * -1.0f;
            m_zoom_factor = -1.0f;
            break;
        }
        default:
            LOGE(TAG, "ERROR: no mode set, init nothing!");
            break;
    }
}
