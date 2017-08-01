//
// Created by zhuyuanxuan on 18/11/2016.
// Copyright (c) 2016 Evomotion. All rights reserved.
//

#include "BaseRenderController.h"
#include "LogUtils.h"

const char TAG[] = "BaseRenderController";

evo::BaseRenderController::BaseRenderController(TextureType type) : m_texture_type{type} {
    InitParams();
    m_mode = NULL_MODE;
}

evo::BaseRenderController::BaseRenderController(TextureType m_texture_type, DisplayMode m_mode)
        : m_texture_type(m_texture_type), m_mode(m_mode) {
    InitParams();
}

evo::BaseRenderController::~BaseRenderController() {
    m_program_id = 0;
}

bool evo::BaseRenderController::InitGl() {
    PrintGlInfo();
    return false;
}

// return false if OK.
bool evo::BaseRenderController::Draw() {
    return false;
}

bool evo::BaseRenderController::InitUniforms(int width, int height) {
    m_aspect = width * 1.0f / height;
    m_view_width = width;
    m_view_height = height;
    return false;
}

GLuint evo::BaseRenderController::get_program_id() const {
    return m_program_id;
}

void evo::BaseRenderController::SetRotateZoomStatus(GLfloat lat, GLfloat lon, GLfloat factor) {
    m_lon = lon;
    m_lat = lat;
    if (m_mode == SPHERE) {
        m_zoom_factor = factor;
    } else {
        m_fov_factor = factor;
    }
}

void evo::BaseRenderController::SetRotateStatus(GLfloat lat, GLfloat lon) {
    m_lat = lat;
    m_lon = lon;
}

void evo::BaseRenderController::set_lat(GLfloat m_lat) {
    BaseRenderController::m_lat = m_lat;
}

void evo::BaseRenderController::UpdateLookat(float *mat4) {
}

void evo::BaseRenderController::UpdateProjection(float *mat4) {
}

GLfloat evo::BaseRenderController::get_lon() const {
    return m_lon;
}

GLfloat evo::BaseRenderController::get_lat() const {
    return m_lat;
}

GLfloat evo::BaseRenderController::get_zoom_factor() const {
    if (m_mode == SPHERE) {
        return m_zoom_factor;
    } else {
        return m_fov_factor;
    }
}

int evo::BaseRenderController::get_view_height() const {
    return m_view_height;
}

int evo::BaseRenderController::get_view_width() const {
    return m_view_width;
}

void evo::BaseRenderController::SetRotateMat(float *rotate_mat) {
}

void evo::BaseRenderController::OnSensorRotateXY(float x_axis, float y_axis) {
}

int evo::BaseRenderController::XY2LatLon(float x, float y, float *lat, float *lon) {
    return 1;
}

RenderAnimationDrawingStatus evo::BaseRenderController::get_animate_status() const {
    return m_animate_status;
}

void evo::BaseRenderController::set_animate_status(RenderAnimationDrawingStatus m_animate_status) {
    BaseRenderController::m_animate_status = m_animate_status;
}

void evo::BaseRenderController::set_zoom_factor(float zoom) {
    if (m_mode == SPHERE) {
        m_zoom_factor = zoom;
    } else {
        m_fov_factor = zoom;
    }
}

void evo::BaseRenderController::SetFovZoom(float fov, float zoom) {}

void evo::BaseRenderController::set_fov_factor(GLfloat m_fov_factor) {
    BaseRenderController::m_fov_factor = m_fov_factor;
}

DisplayMode evo::BaseRenderController::get_mode() const {
    return m_mode;
}

void evo::BaseRenderController::set_mode(DisplayMode m_mode) {
    BaseRenderController::m_mode = m_mode;
}

/*
 * Private
 */

void evo::BaseRenderController::PrintGlInfo() {
    LOGI("OpenGl Info:", "======================== GL Info ==========================");
    PrintGlString("Version", GL_VERSION);
    PrintGlString("Shading language version", GL_SHADING_LANGUAGE_VERSION);
    PrintGlString("Vendor", GL_VENDOR);
    PrintGlString("Renderer", GL_RENDERER);
    PrintGlString("Extensions", GL_EXTENSIONS);
    LOGI("OpenGl Info:", "======================== End ==============================");
}

void evo::BaseRenderController::PrintGlString(const char *name, GLenum s) {
    const char *v = (const char *) glGetString(s);
    LOGI("OpenGl Info:", "GL %s = %s\n", name, v);
}

void evo::BaseRenderController::InitParams() {
    m_view_height = 0;
    m_view_width = 0;
    m_zoom_factor = 0;
    m_fov_factor = 0;
    m_lat = 0;
    m_lon = 0;
    m_animate_status = RENDER_NULL_STATUS;
    m_animation_num = 0;
}
