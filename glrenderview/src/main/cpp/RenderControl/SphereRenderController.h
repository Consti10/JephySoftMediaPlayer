//
// Created by zhuyuanxuan on 01/12/2016.
// Copyright (c) 2016 Evomotion. All rights reserved.
//

#ifndef GL_RENDER_SPHERERENDERCONTROLLER_H
#define GL_RENDER_SPHERERENDERCONTROLLER_H

#include "BaseRenderController.h"

#include "vec3.hpp"

#include <math.h>

namespace evo {

    class SphereRenderController : public BaseRenderController {

    public:
        SphereRenderController(TextureType type);

        SphereRenderController(TextureType m_texture_type, DisplayMode m_mode);

        SphereRenderController(const SphereRenderController &render) = delete;

        ~SphereRenderController() override;

        void SetRotateZoomStatus(GLfloat lat, GLfloat lon, GLfloat factor) override;

        void SetRotateStatus(GLfloat lat, GLfloat lon) override;

        void set_lat(GLfloat m_lat) override;

        bool InitGl() override;

        bool InitUniforms(int width, int height) override;

        bool Draw() override;

        // x,y should be a ratio\n
        // e.g. x = deltaX / width; y = deltaY / height;
        void OnSpanXY(float x, float y) override;

        // Handle rotate of gyroscope, the params are absolute angles in rads
        void OnSensorRotateXY(float x_axis, float y_axis) override;

        void OnZoom(float factor) override;

        void SetRotateMat(float *rotate_mat) override;

        int XY2LatLon(float x, float y, float *lat, float *lon) override;

        void set_zoom_factor(float zoom) override;

        void SetFovZoom(float fov, float zoom) override;

        void SetLatFov(float lat, float fov);

        void SetLatFovZoom(float lat, float fov, float zoom);

        float get_fov() const;

        void set_fov(float m_fov);

        void ResetParams(DisplayMode mode);

        /**
         * Cautious, you don't need to call it, if everything works good
         * @param m_face_culling whether to do culling
         */
        void set_face_culling(int m_face_culling);

    private:
        GLint m_lookat_index;
        GLint m_model_index;
        GLint m_projection_mat_index;
        int m_update_param;
        float m_fov;
        float *m_rotate_mat4;
        glm::vec3 m_camera_pos;
        glm::vec3 m_lookat_point;
        int m_face_culling;

        // when change uniform parameters, call it.
        bool UpdateParam();

        void InitEveryThing();
    };
}

#endif
// GL_RENDER_SPHERERENDERCONTROLLER_H
