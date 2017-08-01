//
// Created by zhuyuanxuan on 22/11/2016.
// Copyright (c) 2016 Evomotion. All rights reserved.
//

#ifndef GL_RENDER_SHADER_COMMON_H
#define GL_RENDER_SHADER_COMMON_H

#include <math.h>

const float kLittlePlaVerts[] = {
        0, 0, 0,
        1, 1, 0,
        1, 0, 0,
        0, 0, 0,
        0, 1, 0,
        1, 1, 0,
};

const float kLittlePlaTexCoords[] = {
        1, 0,
        0, 1,
        0, 0,
        1, 0,
        1, 1,
        0, 1,
};

const int kLittlePlaVerNum = 6;

/////////////////
// Sphere
/////////////////

// Video shader
// y axis tex coordinate do reverse
const char kSphereVerStr[] =
        "#version 300 es\n"
                "in vec4 a_position;\n"
                "in vec4 a_tex_coord;\n"
                "out vec2 v_TexCoord;\n"
                "out vec4 vary_position;\n"
                "uniform mat4 vModel;\n"
                "uniform mat4 vProjection;\n"
                "uniform mat4 vLookat;\n"
                "uniform mat4 uSTMatrix;\n"
                "void main() { \n"
                "    gl_Position = vProjection * vLookat * vModel * a_position;\n"
                "    vary_position = a_position;\n"
                "    v_TexCoord.x = a_tex_coord.x;\n"
                "    v_TexCoord.y = 1.0 - a_tex_coord.y;\n"
                "}\n";

const char kSphereFragNV12[] =
        "#version 300 es\n"
                "precision mediump float;\n"
                "uniform sampler2D yTexture;\n"
                "uniform sampler2D uvTexture;\n"
                "uniform vec3 cameraPos;\n"
                "in vec2 v_TexCoord;\n"
                "in vec4 vary_position;\n"
                "out vec4 color_out;\n"
                "uniform mat3 yuv2rgb;\n"
                "void main() {\n"
                "    float alpha = 1.0f;\n"
                "    if (dot((vec3(0.0f, 0.0f, 0.0f) - cameraPos), (vary_position.xyz - cameraPos)) < dot(cameraPos, cameraPos)) {\n"
                "        alpha = 0.0f;\n"
                "    }\n"
                "    vec3 color;\n"
                "    color.r = texture(yTexture, v_TexCoord).r - 16.0 / 255.0;\n"
                "    color.gb = texture(uvTexture, v_TexCoord).ra - 128.0 / 255.0;\n"
                "    color_out = vec4(yuv2rgb * color, alpha);\n"
                "}\n";

const char kSphereFragNV12WithLogo[] =
        "#version 300 es\n"
                "precision mediump float;\n"
                "uniform sampler2D yTexture;\n"
                "uniform sampler2D uvTexture;\n"
                "uniform sampler2D logo_texture;\n"
                "uniform vec3 cameraPos;\n"
                "in vec2 v_TexCoord;\n"
                "in vec4 vary_position;\n"
                "out vec4 color_out;\n"
                "uniform mat3 yuv2rgb;\n"
                "void main() {\n"
                "    vec3 yuv_color;\n"
                "    yuv_color.r = texture(yTexture, v_TexCoord).r - 16.0 / 255.0;\n"
                "    yuv_color.gb = texture(uvTexture, v_TexCoord).ra - 128.0 / 255.0;\n"
                "    vec4 main_color = vec4(yuv2rgb * yuv_color, 1.0f);\n"
                "    vec4 logo_color = texture(logo_texture, v_TexCoord);\n"
                "    color_out = mix(main_color, logo_color, logo_color.a);\n"
                "    color_out.a = 1.0f;\n"
                "    if (dot((vec3(0.0f, 0.0f, 0.0f) - cameraPos), (vary_position.xyz - cameraPos))"
                "< dot(cameraPos, cameraPos)) {\n"
                "        color_out.a = 0.0f;\n"
                "    }\n"
                "}\n";

const char kSphereVideoYUV420PFrag[] =
        "#version 300 es\n"
                "precision mediump float;\n"
                "uniform sampler2D yTexture;\n"
                "uniform sampler2D uTexture;\n"
                "uniform sampler2D vTexture;\n"
                "uniform vec3 cameraPos;\n"
                "in vec2 v_TexCoord;\n"
                "in vec4 vary_position;\n"
                "out vec4 color_out;\n"
                "uniform mat3 yuv2rgb;\n"
                "void main() {\n"
                "    float alpha = 1.0f;\n"
                "    if (dot((vec3(0.0f, 0.0f, 0.0f) - cameraPos), (vary_position.xyz - cameraPos)) <"
                "dot(cameraPos, cameraPos)) {\n"
                "        alpha = 0.0f;\n"
                "    }\n"
                "    vec3 yuv_color;\n"
                "    yuv_color.r = texture(yTexture, v_TexCoord).r - 16.0 / 255.0;\n"
                "    yuv_color.g = texture(uTexture, v_TexCoord).r - 128.0 / 255.0;\n"
                "    yuv_color.b = texture(vTexture, v_TexCoord).r - 128.0 / 255.0;\n"
                "    color_out = vec4(yuv2rgb * yuv_color, alpha);\n"
                "}\n";

const char kSphereYUV420PWithLogoFrag[] =
        "#version 300 es\n"
                "precision mediump float;\n"
                "uniform sampler2D yTexture;\n"
                "uniform sampler2D uTexture;\n"
                "uniform sampler2D vTexture;\n"
                "uniform sampler2D logo_texture;\n"
                "uniform vec3 cameraPos;\n"
                "in vec2 v_TexCoord;\n"
                "in vec4 vary_position;\n"
                "out vec4 color_out;\n"
                "uniform mat3 yuv2rgb;\n"
                "void main() {\n"
                "    vec3 yuv_color;\n"
                "    yuv_color.r = texture(yTexture, v_TexCoord).r - 16.0 / 255.0;\n"
                "    yuv_color.g = texture(uTexture, v_TexCoord).r - 128.0 / 255.0;\n"
                "    yuv_color.b = texture(vTexture, v_TexCoord).r - 128.0 / 255.0;\n"
                "    vec4 main_color = vec4(yuv2rgb * yuv_color, 1.0f);\n"
                "    vec4 logo_color = texture(logo_texture, v_TexCoord);\n"
                "    color_out = mix(main_color, logo_color, logo_color.a);\n"
                "    if (dot((vec3(0.0f, 0.0f, 0.0f) - cameraPos), (vary_position.xyz - cameraPos)) <"
                "dot(cameraPos, cameraPos)) {\n"
                "        color_out.a = 0.0f;\n"
                "    }\n"
                "}\n";

const char kSphereVideoFragStrAndroid[] =
        "#version 300 es\n"
                "#ifdef GL_OES_EGL_image_external_essl3 \n"
                "    #extension GL_OES_EGL_image_external_essl3 : require\n"
                "#else\n"
                "    #extension GL_OES_EGL_image_external : require\n"
                "#endif\n"
                "precision mediump float;\n"
                "uniform samplerExternalOES Etexture;\n"
                "uniform vec3 cameraPos;\n"
                "in vec2 v_TexCoord;\n"
                "in vec4 vary_position;\n"
                "out vec4 color_out;\n"
                "void main() {\n"
                "    float alpha = 1.0f;\n"
                "    if (dot((vec3(0.0f, 0.0f, 0.0f) - cameraPos), (vary_position.xyz - cameraPos)) <"
                "dot(cameraPos, cameraPos)) {\n"
                "        alpha = 0.0f;\n"
                "    }\n"
                "    color_out = vec4(texture(Etexture, v_TexCoord).rgb, alpha);\n "
                "}\n";

// Sphere Image shader

const char kSphereImgVerStr[] =
        "#version 300 es\n"
                "in vec4 a_position;\n"
                "in vec4 a_tex_coord;\n"
                "out vec2 v_TexCoord;\n"
                "out vec4 vary_position;\n"
                "uniform mat4 vModel;\n"
                "uniform mat4 vProjection;\n"
                "uniform mat4 vLookat;\n"
                "void main() { \n"
                "    vary_position = a_position;\n"
                "    gl_Position = vProjection * vLookat * vModel * a_position;\n"
                "    v_TexCoord.x = a_tex_coord.x;\n"
                "    v_TexCoord.y = 1.0 - a_tex_coord.y;\n"
                "}\n";

const char kSphereImgFragStr[] =
        "#version 300 es\n"
                "precision mediump float;\n"
                "uniform vec3 cameraPos;\n"
                "uniform sampler2D rgbTexture;\n"
                "in vec2 v_TexCoord;\n"
                "in vec4 vary_position;\n"
                "out vec4 color_out;\n"
                "void main() {\n"
                "    color_out = texture(rgbTexture, v_TexCoord);\n"
                "    if (dot((vec3(0.0f, 0.0f, 0.0f) - cameraPos), (vary_position.xyz - cameraPos)) < dot(cameraPos, cameraPos)) {\n"
                "        color_out.a = 0.0f;\n"
                "    }\n"
                "}\n";

const char kSphereImgFragStrWithLogo[] =
        "#version 300 es\n"
                "precision mediump float;\n"
                "uniform vec3 cameraPos;\n"
                "uniform sampler2D rgbTexture;\n"
                "uniform sampler2D logo_texture;\n"
                "in vec2 v_TexCoord;\n"
                "in vec4 vary_position;\n"
                "out vec4 color_out;\n"
                "void main() {\n"
                "    vec4 main_color = texture(rgbTexture, v_TexCoord);\n"
                "    vec4 logo_color = texture(logo_texture, v_TexCoord);\n"
                "    color_out = mix(main_color, logo_color, logo_color.a);\n"
                "    color_out.a = 1.0f;\n"
                "    if (dot((vec3(0.0f, 0.0f, 0.0f) - cameraPos), (vary_position.xyz - cameraPos)) < dot(cameraPos, cameraPos)) {\n"
                "        color_out.a = 0.0f;\n"
                "    }\n"
                "}\n";

///////////////
// VR shader
//////////////

// VR view don't need to do y axis tex coord reverse. Do in the x axis.
const char kVrVerStr[] =
        "#version 300 es\n"
                "in vec4 a_position;\n"
                "in vec4 a_tex_coord;\n"
                "out vec2 v_TexCoord;\n "
                "uniform mat4 vModel;\n"
                "uniform mat4 vProjection;\n"
                "uniform mat4 vLookat;\n"
                "uniform mat4 uSTMatrix;\n"
                "void main() { \n"
                "    gl_Position = vProjection * vLookat * vModel * a_position;\n"
                "    v_TexCoord.x =  1.0 - a_tex_coord.x;\n"
                "    v_TexCoord.y =  a_tex_coord.y;\n"
                "}\n";

const char kVrFragStrAndr[] =
        "#version 300 es\n"
                "#ifdef GL_OES_EGL_image_external_essl3 \n"
                "    #extension GL_OES_EGL_image_external_essl3 : require\n"
                "#else\n"
                "    #extension GL_OES_EGL_image_external : require\n"
                "#endif\n"
                "precision mediump float;\n"
                "uniform samplerExternalOES Etexture;\n"
                "in vec2 v_TexCoord;\n"
                "out vec4 color_out;\n"
                "uniform mat3 yuv2rgb;\n"
                "void main() {\n"
                "    color_out = texture(Etexture, v_TexCoord);\n "
                "}\n";

const char kVrFragYuv[] =
        "#version 300 es\n"
                "precision mediump float;\n"
                "in vec2 v_TexCoord;\n"
                "out vec4 color_out;\n"
                "uniform mat3 yuv2rgb;\n"
                "uniform sampler2D yTexture;\n"
                "uniform sampler2D uTexture;\n"
                "uniform sampler2D vTexture;\n"
                "void main() {\n"
                "    vec3 yuv_color;\n"
                "    yuv_color.r = texture(yTexture, v_TexCoord).r - 16.0 / 255.0;\n"
                "    yuv_color.g = texture(uTexture, v_TexCoord).r - 128.0 / 255.0;\n"
                "    yuv_color.b = texture(vTexture, v_TexCoord).r - 128.0 / 255.0;\n"
                "    color_out = vec4(yuv2rgb * yuv_color, 1.0f);\n"
                "}\n";

const char kVrYuvWithLogoFrag[] =
        "#version 300 es\n"
                "precision mediump float;\n"
                "in vec2 v_TexCoord;\n"
                "out vec4 color_out;\n"
                "uniform mat3 yuv2rgb;\n"
                "uniform sampler2D yTexture;\n"
                "uniform sampler2D uTexture;\n"
                "uniform sampler2D vTexture;\n"
                "uniform sampler2D logo_texture;\n"
                "void main() {\n"
                "    vec3 yuv_color;\n"
                "    yuv_color.r = texture(yTexture, v_TexCoord).r - 16.0 / 255.0;\n"
                "    yuv_color.g = texture(uTexture, v_TexCoord).r - 128.0 / 255.0;\n"
                "    yuv_color.b = texture(vTexture, v_TexCoord).r - 128.0 / 255.0;\n"
                "    vec4 main_color = vec4(yuv2rgb * yuv_color, 1.0f);\n"
                "    vec4 logo_color = texture(logo_texture, v_TexCoord);\n"
                "    color_out = mix(main_color, logo_color, logo_color.a);\n"
                "}\n";

const char kVrFragNV12WithLogo[] =
        "#version 300 es\n"
                "precision mediump float;\n"
                "uniform sampler2D yTexture;\n"
                "uniform sampler2D uvTexture;\n"
                "uniform sampler2D logo_texture;\n"
                "in vec2 v_TexCoord;\n"
                "out vec4 color_out;\n"
                "uniform mat3 yuv2rgb;\n"
                "void main() {\n"
                "    vec3 yuv_color;\n"
                "    yuv_color.r = texture(yTexture, v_TexCoord).r - 16.0 / 255.0;\n"
                "    yuv_color.gb = texture(uvTexture, v_TexCoord).ra - 128.0 / 255.0;\n"
                "    vec4 main_color = vec4(yuv2rgb * yuv_color, 1.0f);\n"
                "    vec4 logo_color = texture(logo_texture, v_TexCoord);\n"
                "    color_out = mix(main_color, logo_color, logo_color.a);\n"
                "    color_out.a = 1.0f;\n"
                "}\n";

const char kVrImgFragStr[] =
        "#version 300 es\n"
                "precision mediump float;\n"
                "uniform sampler2D rgbTexture;\n"
                "in vec2 v_TexCoord;\n"
                "out vec4 color_out;\n"
                "void main() {\n"
                "    color_out = texture(rgbTexture, v_TexCoord);\n"
                "}\n";

const char kVrImgFragwithLogo[] =
        "#version 300 es\n"
                "precision mediump float;\n"
                "uniform sampler2D rgbTexture;\n"
                "uniform sampler2D logo_texture;\n"
                "in vec2 v_TexCoord;\n"
                "out vec4 color_out;\n"
                "void main() {\n"
                "    vec4 main_color = texture(rgbTexture, v_TexCoord);\n"
                "    vec4 logo_color = texture(logo_texture, v_TexCoord);\n"
                "    color_out = mix(main_color, logo_color, logo_color.a);\n"
                "}\n";

/////////////////
// Little planet video shader
/////////////////

const char kLittlePlaVerStr[] =
        "#version 300 es\n"
                "in vec4 a_position; \n"
                "in vec4 a_tex_coord;\n"
                "out vec2 v_TexCoord;\n "
                "uniform mat4 vProjection;\n"
                "void main() { \n"
                "    gl_Position = vProjection * a_position;\n"
                "    v_TexCoord = vec2(1.0, 1.0) - a_tex_coord.xy;\n"
                "}\n";

const char kLittlePlaFragStr[] =
        "#version 300 es\n"
                "precision mediump float; \n"
                "in vec2 v_TexCoord;\n"
                "out vec4 color_out;\n"
                "uniform sampler2D yTexture;\n"
                "uniform sampler2D uvTexture;\n"
                "uniform mat3 yuv2rgb;\n"
                "uniform float uScale, uAspect;\n"
                "uniform mat3 uTransform;\n"
                "\n"
                "#define PI 3.141592653589793\n"
                "\n"
                "void main() { \n"
                "    vec2 rads = vec2(PI * 2., PI);\n"
                "    vec2 pnt = (v_TexCoord - .5) * vec2(uScale, uScale * uAspect);\n"
                "    // Project to Sphere\n"
                "    float x2y2 = pnt.x * pnt.x + pnt.y * pnt.y;\n"
                "    vec3 sphere_pnt = vec3(2. * pnt, x2y2 - 1.) / (x2y2 + 1.);\n"
                "    sphere_pnt *= uTransform;\n"
                "    // Convert to Spherical Coordinates\n"
                "    float lon = atan(sphere_pnt.y, sphere_pnt.x);\n"
                "    float lat = acos(sphere_pnt.z);\n"
                "    // judge the value\n"
                "    vec2 tex_coord = vec2(lon, lat) / rads;\n"
                "    // texture\n"
                "    vec3 color;\n"
                "    color.r = texture(yTexture, tex_coord).r - 16.0 / 255.0;\n"
                "    color.gb = texture(uvTexture, tex_coord).ra - 128.0 / 255.0;\n"
                "    color_out = vec4(yuv2rgb * color, 1.0);\n"
                "    \n"
                "}\n";

const char kLittlePlaFragStrAndr[] =
        "#version 300 es\n"
                "#ifdef GL_OES_EGL_image_external_essl3 \n"
                "    #extension GL_OES_EGL_image_external_essl3 : require\n"
                "#else\n"
                "    #extension GL_OES_EGL_image_external : require\n"
                "#endif\n"
                "precision mediump float;\n"
                "uniform samplerExternalOES Etexture;\n"
                "in vec2 v_TexCoord;\n"
                "out vec4 color_out;\n"
                "uniform float uScale, uAspect;\n"
                "uniform mat3 uTransform;\n"
                "\n"
                "#define PI 3.141592653589793\n"
                "\n"
                "void main() { \n"
                "    vec2 rads = vec2(PI * 2., PI);\n"
                "    vec2 pnt = (v_TexCoord - .5) * vec2(uScale, uScale * uAspect);\n"
                "    // Project to Sphere\n"
                "    float x2y2 = pnt.x * pnt.x + pnt.y * pnt.y;\n"
                "    vec3 sphere_pnt = vec3(2. * pnt, x2y2 - 1.) / (x2y2 + 1.);\n"
                "    sphere_pnt *= uTransform;\n"
                "    // Convert to Spherical Coordinates\n"
                "    float lon = atan(sphere_pnt.y, sphere_pnt.x);\n"
                "    float lat = acos(sphere_pnt.z);\n"
                "    // judge the value\n"
                "    vec2 tex_coord = vec2(lon, lat) / rads;\n"
                "    if (sign(tex_coord.y) == -1.0) {\n"
                "        tex_coord.y += 1.;\n"
                "    } else if (sign(tex_coord.x) == -1.0) {\n"
                "        tex_coord.x += 1.;\n"
                "    }\n"
                "    // texture\n"
                "    color_out = texture(Etexture, tex_coord);\n"
                "    \n"
                "}\n";

const char kLittlePlaVideoFragYuv[] =
        "#version 300 es\n"
                "precision mediump float;\n"
                "uniform sampler2D yTexture;\n"
                "uniform sampler2D uTexture;\n"
                "uniform sampler2D vTexture;\n"
                "in vec2 v_TexCoord;\n"
                "out vec4 color_out;\n"
                "uniform float uScale, uAspect;\n"
                "uniform mat3 uTransform;\n"
                "uniform mat3 yuv2rgb;\n"
                "\n"
                "#define PI 3.141592653589793\n"
                "\n"
                "void main() { \n"
                "    vec2 rads = vec2(PI * 2., PI);\n"
                "    vec2 pnt = (v_TexCoord - .5) * vec2(uScale, uScale * uAspect);\n"
                "    // Project to Sphere\n"
                "    float x2y2 = pnt.x * pnt.x + pnt.y * pnt.y;\n"
                "    vec3 sphere_pnt = vec3(2. * pnt, x2y2 - 1.) / (x2y2 + 1.);\n"
                "    sphere_pnt *= uTransform;\n"
                "    // Convert to Spherical Coordinates\n"
                "    float lon = atan(sphere_pnt.y, sphere_pnt.x);\n"
                "    float lat = acos(sphere_pnt.z);\n"
                "    // judge the value\n"
                "    vec2 tex_coord = vec2(lon, lat) / rads;\n"
                "    if (sign(tex_coord.y) == -1.0) {\n"
                "        tex_coord.y += 1.;\n"
                "    } else if (sign(tex_coord.x) == -1.0) {\n"
                "        tex_coord.x += 1.;\n"
                "    }\n"
                "    // texture\n"
                "    vec3 yuv_color;\n"
                "    yuv_color.r = texture(yTexture, tex_coord).r - 16.0 / 255.0;\n"
                "    yuv_color.g = texture(uTexture, tex_coord).r - 128.0 / 255.0;\n"
                "    yuv_color.b = texture(vTexture, tex_coord).r - 128.0 / 255.0;\n"
                "    color_out = vec4(yuv2rgb * yuv_color, 1.0f);\n"
                "    \n"
                "}\n";

// Little planet image shader
const char kLittlePlaImgVerStr[] =
        "#version 300 es\n"
                "in vec4 a_position; \n"
                "in vec4 a_tex_coord;\n"
                "out vec2 v_TexCoord;\n "
                "uniform mat4 vProjection;\n"
                "void main() { \n"
                "    gl_Position = vProjection * a_position;\n"
                "    v_TexCoord = vec2(1.0, 1.0) - a_tex_coord.xy;\n"
                "}\n";

const char kLittlePlaImgFragStr[] =
        "#version 300 es\n"
                "precision mediump float; \n"
                "in vec2 v_TexCoord;\n"
                "out vec4 color_out;\n"
                "uniform sampler2D rgbTexture;\n"
                "uniform float uScale, uAspect;\n"
                "uniform mat3 uTransform;\n"
                "\n"
                "#define PI 3.141592653589793\n"
                "\n"
                "void main() { \n"
                "    vec2 rads = vec2(PI * 2., PI);\n"
                "    vec2 pnt = (v_TexCoord - .5) * vec2(uScale, uScale * uAspect);\n"
                "    // Project to Sphere\n"
                "    float x2y2 = pnt.x * pnt.x + pnt.y * pnt.y;\n"
                "    vec3 sphere_pnt = vec3(2. * pnt, x2y2 - 1.) / (x2y2 + 1.);\n"
                "    sphere_pnt *= uTransform;\n"
                "    // Convert to Spherical Coordinates\n"
                "    float lon = atan(sphere_pnt.y, sphere_pnt.x);\n"
                "    float lat = acos(sphere_pnt.z);\n"
                "    // judge the value\n"
                "    vec2 tex_coord = vec2(lon, lat) / rads;\n"
                "    // texture\n"
                "    color_out = texture(rgbTexture, tex_coord);\n"
                "    \n"
                "}\n";

const char kPositionName[] = {"a_position"};
const char kTexCoordName[] = {"a_tex_coord"};
const char kLookatName[] = {"vLookat"};
const char kModelName[] = {"vModel"};
const char kProjectionName[] = {"vProjection"};

const char kSamplerY[] = {"yTexture"};
const char kSamplerU[] = {"uTexture"};
const char kSamplerV[] = {"vTexture"};
const char kSamplerUV[] = {"uvTexture"};
const char kColorMat[] = {"yuv2rgb"};

// sphere uniforms
const char kCameraPos[] = {"cameraPos"};

// little planet uniforms
const char kScale[] = {"uScale"};
const char kAspect[] = {"uAspect"};
const char kTransform[] = {"uTransform"};

// image uniforms
const char kSamplerRgb[] = {"rgbTexture"};
const char kSamplerLogo[] = {"logo_texture"};

const GLfloat kFov = (const GLfloat) (100.0 * M_PI / 180.0);
const GLfloat kFishEyeFov = (const GLfloat) (110.0 * M_PI / 180.0);

#endif
// GL_RENDER_SHADER_COMMON_H
