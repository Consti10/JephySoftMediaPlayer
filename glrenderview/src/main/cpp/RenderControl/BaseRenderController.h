//
// Created by zhuyuanxuan on 18/11/2016.
// Copyright (c) 2016 Evomotion. All rights reserved.
//

#ifndef GL_RENDER_BASE_RENDER_CONTROLLER_H
#define GL_RENDER_BASE_RENDER_CONTROLLER_H

#include "EnumsStatus.h"
#include "TextureCommon.h"
#include "BaseRender.h"

namespace evo {
    class BaseRenderController {
    public:
        BaseRenderController(TextureType type);

        BaseRenderController(TextureType m_texture_type, DisplayMode m_mode);

        BaseRenderController(const BaseRenderController &obj) = delete;

        virtual ~BaseRenderController();

        virtual bool InitGl();

        virtual bool InitUniforms(int width, int height);

        virtual bool Draw();

        virtual void UpdateLookat(float *mat4);

        virtual void UpdateProjection(float *mat4);

        // pure virtual functions
        virtual void OnSpanXY(float x, float y) = 0;

        virtual void OnSensorRotateXY(float x_axis, float y_axis);

        virtual void OnZoom(float factor) = 0;

        GLuint get_program_id() const;

        /**
         * @return longitude, the rotate around y axis (span in x of screen)
         */
        virtual GLfloat get_lon() const;

        /**
         * @return latitude, the rotate around x axis (span in y of screen)
         */
        virtual GLfloat get_lat() const;

        virtual GLfloat get_zoom_factor() const;

        int get_view_height() const;

        int get_view_width() const;

        /**
         * Set 3 params if render controller
         * @param lat latitude, the rotate around x axis (span in y of screen)
         * lat == x_rotate
         * @param lon longitude, the rotate around y axis (span in x of screen)
         * lon == y_rotate
         * @param factor zoom factor
         */
        virtual void SetRotateZoomStatus(GLfloat lat, GLfloat lon, GLfloat factor);

        virtual void SetRotateStatus(GLfloat lat, GLfloat lon);

        virtual void set_lat(GLfloat m_lat);

        virtual void set_zoom_factor(float zoom);

        void set_fov_factor(GLfloat m_fov_factor);

        virtual void SetFovZoom(float fov, float zoom);

        virtual void SetRotateMat(float *rotate_mat);

        /**
         * Transform from [x, y] which in view coordinate to
         * latitude and longitutde in a sphere
         * @param x the ratio of distance and screen width
         * @param y the ratios of distance and screen height
         * @param lat latitude in sphere
         * @param lon longitude in sphere
         * @return 0 if success
         */
        virtual int XY2LatLon(float x, float y, float *lat, float *lon);

        RenderAnimationDrawingStatus get_animate_status() const;

        void set_animate_status(RenderAnimationDrawingStatus m_animate_status);

        DisplayMode get_mode() const;

        void set_mode(DisplayMode m_mode);

    protected:
        BaseRender *m_renderer;
        TextureType m_texture_type;
        int m_view_width;
        int m_view_height;
        GLuint m_program_id;
        GLuint m_vao;
        GLfloat m_aspect;
        // lat and lon means the latitude and longitude of the look at point, in radians
        GLfloat m_lat;
        GLfloat m_lon;
        GLfloat m_zoom_factor;
        GLfloat m_fov_factor;
        RenderAnimationDrawingStatus m_animate_status;
        int m_animation_num;
        DisplayMode m_mode;

    private:
        void InitParams();

        void PrintGlInfo();

        void PrintGlString(const char *name, GLenum s);
    };


}
#endif //GL_RENDER_BASE_RENDER_CONTROLLER_H
