//
// Created by zhuyuanxuan on 18/11/2016.
// Copyright (c) 2016 Evomotion. All rights reserved.
//
#include "RenderControl/SphereRenderController.h"
#include "RenderControl/VrRenderController.h"
#include "RenderControl/VideoTextureController.h"
#include "RenderControl/PicTexController.h"
#include "Utils/LogUtils.h"
#include "Animation/GlAnimator.h"
#include "Animation/AnimationCallbackImpl.h"
#include "Animation/AnimateManager.h"
#include "RenderControl/ShaderCommon.h"

#include <jni.h>
#include <android/bitmap.h>

using namespace evo;

const char TAG[] = "RenderView.cpp";
const float kZero = 0.001;

extern "C" {
jlongArray Java_com_evomotion_glrenderview_RenderView_surfaceCreated(
        JNIEnv *env, jclass type, jint texture_type, jint mode);

void Java_com_evomotion_glrenderview_RenderView_surfaceChanged(JNIEnv *env, jclass type, jint width,
                                                           jint height, jlong renderPtr,
                                                           jlong textureControllerPtr);

int Java_com_evomotion_glrenderview_RenderView_drawFrame(JNIEnv *env, jclass type, jlong renderPtr,
                                                     jlong texPtr);

void Java_com_evomotion_glrenderview_RenderView_destroy(JNIEnv *env, jclass type, jlong renderPtr,
                                                    jlong textureControllerPtr, jlong animatePtr);

void Java_com_evomotion_glrenderview_RenderView_onSpanXYNative(JNIEnv *env, jclass type, jfloat x,
                                                           jfloat y, jlong renderPtr);

void Java_com_evomotion_glrenderview_RenderView_onInertiaXYNative(JNIEnv *env, jclass type, jfloat x,
                                                              jfloat y, jlong renderPtr,
                                                              jlong animatePtr);

void Java_com_evomotion_glrenderview_RenderView_onZoomNative(JNIEnv *env, jclass type, jfloat factor,
                                                         jlong renderPtr);

void Java_com_evomotion_glrenderview_RenderView_onSensorRotateXYNative(JNIEnv *env, jclass type,
                                                                   jfloat x,
                                                                   jfloat y, jlong renderPtr);

void Java_com_evomotion_glrenderview_RenderView_setRenderStatusNative(JNIEnv *env, jclass type,
                                                                  jfloatArray angles_,
                                                                  jlong renderPtr);

jfloatArray Java_com_evomotion_glrenderview_RenderView_getRenderStatusNative(JNIEnv *env, jclass type,
                                                                         jlong renderPtr);

void
Java_com_evomotion_glrenderview_RenderView_resetViewNative(JNIEnv *env, jclass type, jlong renderPtr,
                                                       jlong animatePtr);

void Java_com_evomotion_glrenderview_BinocularVrView_updateLookAt(JNIEnv *env, jclass type,
                                                              jfloatArray mat_, jlong renderPtr);

void Java_com_evomotion_glrenderview_BinocularVrView_updateProjection(JNIEnv *env, jclass type,
                                                                  jfloatArray mat_,
                                                                  jlong renderPtr);

void Java_com_evomotion_glrenderview_BinocularVrView_reactiveTex(JNIEnv *env, jclass type,
                                                             jlong texturePtr);

void Java_com_evomotion_glrenderview_RenderView_setBitmap(JNIEnv *env, jclass type, jobject bmp,
                                                      jlong texPtr);

JNIEXPORT void JNICALL
Java_com_evomotion_glrenderview_RenderView_setBitmapAndLogo(JNIEnv *env, jclass type, jobject bmp,
                                                        jobject logo, jlong texPtr);

JNIEXPORT void JNICALL
Java_com_evomotion_glrenderview_RenderView_setLogoTexture(JNIEnv *env, jclass type, jobject logo,
                                                      jlong texPtr);

void Java_com_evomotion_glrenderview_RenderView_changeMode(JNIEnv *env, jclass type, jint mode,
                                                       jlongArray poiners);

jint Java_com_evomotion_glrenderview_RenderView_readPixelsNative(JNIEnv *env, jclass type,
                                                             jbyteArray bytes_, jlong renderPtr);

JNIEXPORT jint JNICALL
Java_com_evomotion_glrenderview_RenderView_nativeDisplayYuv420pPixelBytes(JNIEnv *env, jclass type,
                                                                      jbyteArray buffer_y,
                                                                      jbyteArray buffer_u,
                                                                      jbyteArray buffer_v,
                                                                      jint width,
                                                                      jint height, jlong texPtr);

JNIEXPORT jint JNICALL
Java_com_evomotion_glrenderview_RenderView_nativeDisplayNV12PixelBytes(JNIEnv *env, jclass type,
                                                                   jbyteArray y_, jbyteArray uv_,
                                                                   jint width, jint height,
                                                                   jlong texPtr);

JNIEXPORT jint JNICALL
Java_com_evomotion_glrenderview_RenderView_nativeDisplayYuv420pPixel(JNIEnv *env, jclass type, jlong y,
                                                                 jlong u, jlong v, jint width,
                                                                 jint height, jlong texPtr);

JNIEXPORT jint JNICALL
Java_com_evomotion_glrenderview_RenderView_nativeDisplayNV12Pixel(JNIEnv *env, jclass type, jlong y,
                                                              jlong uv, jint width, jint height,
                                                              jlong texturePtr);

}

JNIEXPORT jlongArray Java_com_evomotion_glrenderview_RenderView_surfaceCreated(
        JNIEnv *env, jclass type, jint textureType, jint mode) {
    TextureType texture_type = TextureType(textureType);
    BaseRenderController *render_controller = 0;
    BaseTextureController *texture_controller = 0;

    // here init different render controller sphere,littlePlanet,fish eye...
    if (mode == VR) {
        render_controller = new VrRenderController(texture_type);
        if (render_controller->InitGl()) {
            LOGE(TAG, "render_controller->InitGl() error");
            return 0;
        }
        LOGI(TAG, "surfaceCreated:  VrRenderController");

        // init diff texture controller
        if (texture_type == YUV420P || texture_type == NV12) {
            texture_controller = new VideoTextureController(render_controller, texture_type);
            texture_controller->Init();
            texture_controller->GenTextures(texture_type);
        } else if (texture_type == IMAGE) {
            // init a image texture controller
            texture_controller = new PicTexController(render_controller);
            texture_controller->Init();
        }
    } else if (mode == SPHERE || mode == LITTLE_PLANET || mode == FISHEYE) {
        render_controller = new SphereRenderController(texture_type, DisplayMode(mode));
        if (render_controller->InitGl()) {
            LOGE(TAG, "render_controller->InitGl() error");
            return 0;
        }
        LOGI(TAG, "surfaceCreated: RenderController with mode: %d.", mode);

        // init diff texture controller
        if (texture_type == YUV420P || texture_type == NV12) {
            texture_controller = new VideoTextureController(render_controller, texture_type);
            // only call init() and GenTextures() in Sphere mode and VR mode
            texture_controller->Init();
            texture_controller->GenTextures(texture_type);
        } else if (texture_type == IMAGE) {
            // init a image texture controller
            texture_controller = new PicTexController(render_controller);
            texture_controller->Init();
        }
    } else {
        LOGE(TAG, "Invalid mode, return!");
        return NULL;
    }

    if (texture_controller == nullptr) {
        return NULL;
    }

    jlongArray long_array = env->NewLongArray(3);
    jlong *tmp_array = env->GetLongArrayElements(long_array, NULL);
    tmp_array[0] = reinterpret_cast<jlong>(render_controller);
    tmp_array[1] = reinterpret_cast<jlong>(texture_controller);
    if (mode != VR) {
        AnimateManager *manager = new AnimateManager();
        tmp_array[2] = reinterpret_cast<jlong>(manager);
    }
    env->ReleaseLongArrayElements(long_array, tmp_array, 0);

    return long_array;
}

JNIEXPORT void Java_com_evomotion_glrenderview_RenderView_surfaceChanged(JNIEnv *env, jclass type,
                                                                     jint width,
                                                                     jint height, jlong renderPtr,
                                                                     jlong textureControllerPtr) {
    LOGI(TAG, "Surface changed width: %d, height: %d.", width, height);
    glViewport(0, 0, width, height);

    if (!(renderPtr && textureControllerPtr)) {
        LOGE(TAG, "invalid ptr %s", __FUNCTION__);
        return;
    }
    BaseRenderController *render = reinterpret_cast<BaseRenderController *>(renderPtr);
    BaseTextureController *texturer = reinterpret_cast<BaseTextureController *>(
            textureControllerPtr);

    bool result = render->InitUniforms(width, height);
    if (result) {
        LOGE(TAG, "Error in renderer->InitUniforms");
        return;
    }
}

/**
 * @return the animation status of render controller
 */
JNIEXPORT int
Java_com_evomotion_glrenderview_RenderView_drawFrame(JNIEnv *env, jclass type, jlong renderPtr,
                                                 jlong texPtr) {
    if (renderPtr == 0 || texPtr == 0) {
        LOGE(TAG, "Invalid render or texture pointer");
        return -1;
    }

    BaseTextureController *texture_controller = reinterpret_cast<BaseTextureController *>(texPtr);
    BaseRenderController *renderer = reinterpret_cast<BaseRenderController *>(renderPtr);

    texture_controller->BindTexture();
    // Need bind texture object every time draw
    if (renderer->get_mode() == VR) {
        texture_controller->BindTexture();
    }

    if (renderer->Draw()) {
        LOGE(TAG, "Error in Draw");
    }

    return renderer->get_animate_status();
}

JNIEXPORT void
Java_com_evomotion_glrenderview_RenderView_destroy(JNIEnv *env, jclass type, jlong renderPtr,
                                               jlong textureControllerPtr, jlong animatePtr) {
    AnimateManager *animate_manager = nullptr;
    if (animatePtr) {
        animate_manager = reinterpret_cast<AnimateManager *>(animatePtr);
        animate_manager->StopAllAnimation();
    }

    if (renderPtr && textureControllerPtr) {
        BaseRenderController *renderer = reinterpret_cast<BaseRenderController *>(renderPtr);
        BaseTextureController *textureController = reinterpret_cast<BaseTextureController *>(
                textureControllerPtr);
        delete renderer;
        delete textureController;
    } else {
        LOGE(TAG, "Invalid pointers in Destroy!");
    }

    if (animate_manager != nullptr) {
        delete animate_manager;
    }
}

JNIEXPORT void Java_com_evomotion_glrenderview_RenderView_onSpanXYNative(JNIEnv *env, jclass type,
                                                                     jfloat x,
                                                                     jfloat y, jlong renderPtr) {
    if (renderPtr) {
        BaseRenderController *renderer = reinterpret_cast<BaseRenderController *>(renderPtr);
        renderer->OnSpanXY(x, y);
    } else {
        LOGE(TAG, "render controller invalid! in %s", __FUNCTION__);
    }
}

JNIEXPORT void
Java_com_evomotion_glrenderview_RenderView_onInertiaXYNative(JNIEnv *env, jclass type, jfloat x,
                                                         jfloat y, jlong renderPtr,
                                                         jlong animatePtr) {
    if (renderPtr == 0 || animatePtr == 0) {
        LOGE(TAG, "Invalid render pointer %s", __FUNCTION__);
        return;
    }

    BaseRenderController *render = reinterpret_cast<BaseRenderController *>(renderPtr);
    float acceleration_abs = 10;
    float *start_val = new float[2];
    float *velocity = new float[2];
    start_val[0] = render->get_lat();
    start_val[1] = render->get_lon();
    render->XY2LatLon(x, y, velocity, velocity + 1);
    LOGE(TAG, "Start inertia, velocity: [lat] %f, [lon] %f\tstart point lat %f, lon %f",
         velocity[0], velocity[1], start_val[0], start_val[1]);


    float dur_lat = fabsf(velocity[0]) / acceleration_abs;
    float dur_lon = fabsf(velocity[1]) / acceleration_abs;
    int duration = (int) (max(dur_lat, dur_lon) * 1000);
    LOGE(TAG, "Duration %d", duration);

    // Evaluator
    ZeroEvaluator *zero_evaluator = new ZeroEvaluator(2);
    zero_evaluator->set_acceleration(acceleration_abs);
    zero_evaluator->set_duration(duration);

    // ValueSetter
    // ref:      void SetRotateStatus(GLfloat lat, GLfloat lon)
    AnimeValueSetter<BaseRenderController> *value_setter =
            new AnimeValueSetter<BaseRenderController>(render);
    value_setter->SetFuncP(&BaseRenderController::SetRotateStatus);

    GlAnimator *animator = GlAnimator::OfFloatArray(2, start_val, velocity, zero_evaluator,
                                                    value_setter);
    animator->set_duration(duration);

    // set callback
    AnimationCallbackInertiaImpl *inertia_callback = new AnimationCallbackInertiaImpl(render);
    GlAnimateCallBack<AnimationCallbackInertiaImpl> *animate_callback =
            new GlAnimateCallBack<AnimationCallbackInertiaImpl>(
                    inertia_callback, &AnimationCallbackInertiaImpl::OnStart,
                    &AnimationCallbackInertiaImpl::OnStop);
    animator->SetAnimateCallback(animate_callback);

    AnimateManager *manager = reinterpret_cast<AnimateManager *>(animatePtr);
    manager->AddAnimators(animator, ANIMATION_INERTIA, 0);
}

JNIEXPORT void Java_com_evomotion_glrenderview_RenderView_onZoomNative(JNIEnv *env, jclass type,
                                                                   jfloat factor,
                                                                   jlong renderPtr) {
    if (renderPtr) {
        BaseRenderController *renderer = reinterpret_cast<BaseRenderController *>(renderPtr);
        renderer->OnZoom(factor);
    } else {
        LOGE(TAG, "Error render controller invalid! in %s", __FUNCTION__);
    }
}

JNIEXPORT void Java_com_evomotion_glrenderview_RenderView_onSensorRotateXYNative(JNIEnv *env,
                                                                             jclass type, jfloat x,
                                                                             jfloat y,
                                                                             jlong renderPtr) {
    if (renderPtr) {
        BaseRenderController *renderer = reinterpret_cast<BaseRenderController *>(renderPtr);
        renderer->OnSensorRotateXY(x, y);
    } else {
        LOGE(TAG, "render controller invalid! in %s", __FUNCTION__);
    }
}

JNIEXPORT void Java_com_evomotion_glrenderview_BinocularVrView_updateLookAt(JNIEnv *env, jclass type,
                                                                        jfloatArray mat_,
                                                                        jlong renderPtr) {
    jfloat *mat = env->GetFloatArrayElements(mat_, NULL);

    if (renderPtr) {
        BaseRenderController *renderer = reinterpret_cast<BaseRenderController *>(renderPtr);

        renderer->UpdateLookat(mat);
    } else {
        LOGE(TAG, "Error render controller invalid! %s", __FUNCTION__);
    }

    env->ReleaseFloatArrayElements(mat_, mat, 0);
}

JNIEXPORT void Java_com_evomotion_glrenderview_BinocularVrView_updateProjection(JNIEnv *env,
                                                                            jclass type,
                                                                            jfloatArray mat_,
                                                                            jlong renderPtr) {
    jfloat *mat = env->GetFloatArrayElements(mat_, NULL);
    if (renderPtr) {
        BaseRenderController *renderer = reinterpret_cast<BaseRenderController *>(renderPtr);
        renderer->UpdateProjection(mat);
    } else {
        LOGE(TAG, "render controller invalid! %s", __FUNCTION__);
    }

    env->ReleaseFloatArrayElements(mat_, mat, 0);
}

JNIEXPORT void Java_com_evomotion_glrenderview_BinocularVrView_reactiveTex(JNIEnv *env, jclass type,
                                                                       jlong texturePtr) {
    BaseTextureController *texture_controller =
            reinterpret_cast<BaseTextureController *>(texturePtr);
    texture_controller->ReActiveTex();
}

JNIEXPORT void Java_com_evomotion_glrenderview_RenderView_setBitmap(JNIEnv *env, jclass type,
                                                                jobject bmp,
                                                                jlong texPtr) {
    if (texPtr) {
        BaseTextureController *texturer = reinterpret_cast<BaseTextureController *>(texPtr);

        LOGI(TAG, "set bitmap ");
        AndroidBitmapInfo info;
        GLvoid *pixels;
        // get info data
        AndroidBitmap_getInfo(env, bmp, &info);

        if (info.format != ANDROID_BITMAP_FORMAT_RGBA_8888) {
            LOGI(TAG, "Bitmap format is not RGBA_8888! It is: %d.", info.format);
        }

        int width = info.width;
        int height = info.height;
        AndroidBitmap_lockPixels(env, bmp, &pixels);

        texturer->SetBmpDataTexture(pixels, width, height, info.format);

        AndroidBitmap_unlockPixels(env, bmp);

    } else {
        LOGW(TAG, "texture controller invalid! %s", __FUNCTION__);
    }
}

void Java_com_evomotion_glrenderview_RenderView_setBitmapAndLogo(JNIEnv *env, jclass type, jobject bmp,
                                                             jobject logo, jlong texPtr) {
    if (!texPtr) {
        return;
    }

    PicTexController *texture = reinterpret_cast<PicTexController *>(texPtr);

    AndroidBitmapInfo tex_info;
    AndroidBitmapInfo logo_info;
    GLvoid *tex_pixel;
    GLvoid *logo_pixel;

    AndroidBitmap_getInfo(env, bmp, &tex_info);
    AndroidBitmap_getInfo(env, logo, &logo_info);

    if (tex_info.format != ANDROID_BITMAP_FORMAT_RGBA_8888 ||
        logo_info.format != ANDROID_BITMAP_FORMAT_RGBA_8888) {
        LOGE(TAG, "Bitmap format is not RGBA_8888! It is: %d.", tex_info.format);
        return;
    }

    int ret;
    if ((ret = AndroidBitmap_lockPixels(env, bmp, &tex_pixel)) < 0) {
        LOGE(TAG, "AndroidBitmap_lockPixels() failed ! error=%d", ret);
    }
    if ((ret = AndroidBitmap_lockPixels(env, logo, &logo_pixel)) < 0) {
        LOGE(TAG, "AndroidBitmap_lockPixels() failed ! error=%d", ret);
    }

    texture->SetBmpAndLogoData(tex_pixel, logo_pixel, tex_info.width, tex_info.height,
                               logo_info.width, logo_info.height);

    AndroidBitmap_unlockPixels(env, bmp);
    AndroidBitmap_unlockPixels(env, logo);
}

JNIEXPORT void
Java_com_evomotion_glrenderview_RenderView_setLogoTexture(JNIEnv *env, jclass type, jobject logo,
                                                      jlong texPtr) {
    if (texPtr == 0) {
        LOGE(TAG, "Invalid texture controller pointer");
        return;
    }
    AndroidBitmapInfo info;
    AndroidBitmap_getInfo(env, logo, &info);

    void *logo_pixel;
    AndroidBitmap_lockPixels(env, logo, &logo_pixel);

    BaseTextureController *texture_controller = reinterpret_cast<BaseTextureController *>(texPtr);
    LOGI(TAG, "Set logo bitmap %u x %u", info.width, info.height);
    if (texture_controller->SetLogoBmpData(logo_pixel, info.width, info.height)) {
        LOGE(TAG, "Set logo bmp data error!");
    }

    AndroidBitmap_unlockPixels(env, logo);
}

JNIEXPORT void Java_com_evomotion_glrenderview_RenderView_setRenderStatusNative(JNIEnv *env,
                                                                            jclass type,
                                                                            jfloatArray angles_,
                                                                            jlong renderPtr) {
    jfloat *angles = env->GetFloatArrayElements(angles_, NULL);

    if (renderPtr) {
        BaseRenderController *renderer = reinterpret_cast<BaseRenderController *>(renderPtr);
        // value need reverse
        // lat = x rotate = pinch / PI -> angles[0]
        // lon = y rotate = yaw / PI_2 -> angles[1]
        renderer->SetRotateZoomStatus(angles[0], angles[1], angles[2]);
    } else {
        LOGE(TAG, "Error render controller invalid! %s", __FUNCTION__);
    }

    env->ReleaseFloatArrayElements(angles_, angles, 0);
}

JNIEXPORT jfloatArray Java_com_evomotion_glrenderview_RenderView_getRenderStatusNative(
        JNIEnv *env, jclass type, jlong renderPtr) {

    jfloatArray angles = env->NewFloatArray(3);
    if (renderPtr) {
        BaseRenderController *renderer = reinterpret_cast<BaseRenderController *>(renderPtr);
        jfloat *temp_array = env->GetFloatArrayElements(angles, NULL);
        temp_array[0] = renderer->get_lat();
        temp_array[1] = renderer->get_lon();
        temp_array[2] = renderer->get_zoom_factor();
        env->ReleaseFloatArrayElements(angles, temp_array, 0);
    } else {
        LOGE(TAG, "Error render controller invalid! %s", __FUNCTION__);
    }
    return angles;
}

// @param val_array [lat, lon] or [lat, lon, zoom factor]
void CalcualteBeforeReset(BaseRenderController &renderer, float *start_val, float *end_val) {
    float tmp_lon = renderer.get_lon();
    float tmp_lat = renderer.get_lat();

    // prepare before reset
    if (tmp_lon > kZero) {
        tmp_lon -= floor(tmp_lon / (2.0 * M_PI)) * (2.0 * M_PI);
        if (tmp_lon > M_PI) {
            tmp_lon -= 2.0 * M_PI;
        }
    } else {
        tmp_lon -= ceil(tmp_lon / (2.0 * M_PI)) * (2.0 * M_PI);
        if (tmp_lon < M_PI * -1) {
            tmp_lon += M_PI * 2.0;
        }
    }
    // now get value between [-PI, PI]
    renderer.SetRotateStatus(tmp_lat, tmp_lon);

    start_val[0] = tmp_lat;
    start_val[1] = tmp_lon;
    start_val[2] = renderer.get_zoom_factor();
}

JNIEXPORT void
Java_com_evomotion_glrenderview_RenderView_resetViewNative(JNIEnv *env, jclass type, jlong renderPtr,
                                                       jlong animatePtr) {
    if (renderPtr == 0 || animatePtr == 0) {
        return;
    }
    BaseRenderController *renderer = reinterpret_cast<BaseRenderController *>(renderPtr);
    // before start, values[lat, lon]
    float *start_val = new float[3];
    float *end_val = new float[3]{0, 0, 0};
    DisplayMode current_mode = renderer->get_mode();
    if (current_mode == LITTLE_PLANET) {
        end_val[0] = (float) (M_PI_2 * -0.9);
    }

    CalcualteBeforeReset(*renderer, start_val, end_val);
    LOGE(TAG, "start %f %f %f, end %f %f.", start_val[0], start_val[1], start_val[2],
         end_val[0], end_val[1]);

    // value setter
    AnimeValueSetter<BaseRenderController> *value_setter =
            new AnimeValueSetter<BaseRenderController>(renderer);
    // ref: void SetRotateStatus(GLfloat lat, GLfloat lon);
    //     void SetRotateZoomStatus(GLfloat lat, GLfloat lon, GLfloat factor);
    value_setter->SetFuncP(&BaseRenderController::SetRotateZoomStatus);

    // evaluator
    NormalEvaluator *evaluator = new NormalEvaluator(3);

    // initiate animator
    GlAnimator *animator = GlAnimator::OfFloatArray(3, start_val, end_val, evaluator,
                                                    value_setter);
    animator->set_duration(400);

    // callback
    AnimationCallbackResetImpl *reset_callback = new AnimationCallbackResetImpl(renderer);
    GlAnimateCallBack<AnimationCallbackResetImpl> *animate_callBack =
            new GlAnimateCallBack<AnimationCallbackResetImpl>(
                    reset_callback,
                    &AnimationCallbackResetImpl::Start,
                    &AnimationCallbackResetImpl::Stop);
    animator->SetAnimateCallback(animate_callBack);

    AnimateManager *animate_manager = reinterpret_cast<AnimateManager *>(animatePtr);
    animate_manager->AddAnimators(animator, ANIMATION_RESET, 0);
}

/**
 * Start a new animation to do the mode switch
 * @param mode the mode want to switch to
 * @param pointers [render_controller, texture_controller, animation_manager]
 */
JNIEXPORT void Java_com_evomotion_glrenderview_RenderView_changeMode(JNIEnv *env, jclass type,
                                                                 jint mode,
                                                                 jlongArray poiners) {
    LOGI(TAG, "Switch to mode %d in C++", mode);

    jlong *ptrs = env->GetLongArrayElements(poiners, 0);

    SphereRenderController *render_controller = reinterpret_cast<SphereRenderController *>(ptrs[0]);

    // get animate manager from 3rd pointers
    AnimateManager *manager = reinterpret_cast<AnimateManager *>(ptrs[2]);

    // prepare transform animation
    DisplayMode prev_mode = render_controller->get_mode();
    if (prev_mode == mode) {
        return;
    }

    if (mode == SPHERE) {
        // from little planet/fisheye to sphere, the zoom factor decide the fov value of old mode
        // and the camera position is always on the sphere so we assume the origin zoom factor is -1
        // fov value_setter
        AnimeValueSetter<SphereRenderController> *value_setter =
                new AnimeValueSetter<SphereRenderController>(render_controller);
        value_setter->SetFuncP(&SphereRenderController::SetFovZoom);

        // evaluator
        NormalEvaluator *evaluator_2 = new NormalEvaluator(2);

        // callback
        AnimationCallbackTransformImpl *animation_callback_impl_sphere =
                new AnimationCallbackTransformImpl(render_controller, DisplayMode(mode));

        GlAnimateCallBack<AnimationCallbackTransformImpl> *animate_sphere_callback =
                new GlAnimateCallBack<AnimationCallbackTransformImpl>(
                        animation_callback_impl_sphere,
                        &AnimationCallbackTransformImpl::OnStart,
                        &AnimationCallbackTransformImpl::OnStop);

        float *start_val = new float[2]{render_controller->get_fov(),
                                        -1.0f};
        float *end_val = new float[2]{kFov, 0};

        GlAnimator *animator_fov = GlAnimator::OfFloatArray(2, start_val, end_val,
                                                            evaluator_2, value_setter);
        animator_fov->set_duration(400);
        animator_fov->SetAnimateCallback(animate_sphere_callback);

        manager->AddAnimators(animator_fov, ANIMATION_TRANSFORM, 0);
    } else if (mode == FISHEYE) {
        if (prev_mode == LITTLE_PLANET) {
            // value setter
            // [latitude, fov]
            AnimeValueSetter<SphereRenderController> *value_setter =
                    new AnimeValueSetter<SphereRenderController>(render_controller);
            value_setter->SetFuncP(&SphereRenderController::SetLatFov);

            // evaluator
            NormalEvaluator *evaluator_2 = new NormalEvaluator(2);

            // callback
            AnimationCallbackTransformImpl *transform_impl = new AnimationCallbackTransformImpl(
                    render_controller, DisplayMode(mode));

            GlAnimateCallBack<AnimationCallbackTransformImpl> *gl_animation_callback =
                    new GlAnimateCallBack<AnimationCallbackTransformImpl>(
                            transform_impl,
                            &AnimationCallbackTransformImpl::OnStart,
                            &AnimationCallbackTransformImpl::OnStop);

            // value
            float *start_val = new float[2]{render_controller->get_lat(),
                                            render_controller->get_fov()};
            float *end_val = new float[2]{0, kFishEyeFov};

            GlAnimator *gl_animator = GlAnimator::OfFloatArray(2, start_val, end_val, evaluator_2,
                                                               value_setter);
            gl_animator->set_duration(400);
            gl_animator->SetAnimateCallback(gl_animation_callback);

            manager->AddAnimators(gl_animator, ANIMATION_TRANSFORM, 0);

            render_controller->set_fov_factor(0);
        } else if (prev_mode == SPHERE) {
            // Sphere -> Fisheye

            // fov value_setter
            // Attention the order is [fov, zoom]
            AnimeValueSetter<SphereRenderController> *val_setter =
                    new AnimeValueSetter<SphereRenderController>(render_controller);
            val_setter->SetFuncP(&SphereRenderController::SetFovZoom);

            // evaluator
            NormalEvaluator *evaluator_2 = new NormalEvaluator(2);

            // callback
            AnimationCallbackTransformImpl *animation_callback_implement =
                    new AnimationCallbackTransformImpl(render_controller, DisplayMode(mode));

            GlAnimateCallBack<AnimationCallbackTransformImpl> *gl_animate_callback =
                    new GlAnimateCallBack<AnimationCallbackTransformImpl>(
                            animation_callback_implement,
                            &AnimationCallbackTransformImpl::OnStart,
                            &AnimationCallbackTransformImpl::OnStop);

            // prepare values
            float sphere_zoom_factor = render_controller->get_zoom_factor();
            float *start_val = new float[2]{render_controller->get_fov(),
                                            sphere_zoom_factor};
            float *end_val = new float[2]{kFishEyeFov, -1.0f};

            // get an animator obj
            GlAnimator *gl_animator = GlAnimator::OfFloatArray(2, start_val, end_val,
                                                               evaluator_2,
                                                               val_setter);
            gl_animator->set_duration(400);
            gl_animator->SetAnimateCallback(gl_animate_callback);

            manager->AddAnimators(gl_animator, ANIMATION_TRANSFORM, 0);
            render_controller->set_fov_factor(0);
            if (sphere_zoom_factor > -1.0f) {
                render_controller->set_face_culling(0);
            }
        }
    } else if (mode == LITTLE_PLANET) {
        if (prev_mode == SPHERE) {
            // sphere -> little planet

            // value setter
            // [latitude, fov, zoom]
            AnimeValueSetter<SphereRenderController> *value_setter =
                    new AnimeValueSetter<SphereRenderController>(render_controller);
            value_setter->SetFuncP(&SphereRenderController::SetLatFovZoom);

            // evaluator
            NormalEvaluator *evaluator_3 = new NormalEvaluator(3);

            // callback
            AnimationCallbackTransformImpl *transform_impl = new AnimationCallbackTransformImpl(
                    render_controller, DisplayMode(mode));

            GlAnimateCallBack<AnimationCallbackTransformImpl> *gl_animation_callback =
                    new GlAnimateCallBack<AnimationCallbackTransformImpl>(
                            transform_impl,
                            &AnimationCallbackTransformImpl::OnStart,
                            &AnimationCallbackTransformImpl::OnStop);

            // value
            float *start_val = new float[3]{render_controller->get_lat(),
                                            render_controller->get_fov(),
                                            render_controller->get_zoom_factor()};
            float *end_val = new float[3]{(float) (M_PI_2 * -0.9f), kFishEyeFov, -1.0f};

            GlAnimator *gl_animator = GlAnimator::OfFloatArray(3, start_val, end_val, evaluator_3,
                                                               value_setter);
            gl_animator->set_duration(400);
            gl_animator->SetAnimateCallback(gl_animation_callback);
            manager->AddAnimators(gl_animator, ANIMATION_TRANSFORM, 0);

            render_controller->set_fov_factor(0);
            render_controller->set_face_culling(0);
        } else if (prev_mode == FISHEYE) {
            // value setter
            // [latitude, fov]
            AnimeValueSetter<SphereRenderController> *value_setter =
                    new AnimeValueSetter<SphereRenderController>(render_controller);
            value_setter->SetFuncP(&SphereRenderController::SetLatFov);

            // evaluator
            NormalEvaluator *evaluator_2 = new NormalEvaluator(2);

            // callback
            AnimationCallbackTransformImpl *transform_impl = new AnimationCallbackTransformImpl(
                    render_controller, DisplayMode(mode));

            GlAnimateCallBack<AnimationCallbackTransformImpl> *gl_animation_callback =
                    new GlAnimateCallBack<AnimationCallbackTransformImpl>(
                            transform_impl,
                            &AnimationCallbackTransformImpl::OnStart,
                            &AnimationCallbackTransformImpl::OnStop);

            // value
            float *start_val = new float[2]{render_controller->get_lat(),
                                            render_controller->get_fov()};
            float *end_val = new float[2]{(float) (M_PI_2 * -0.9f), kFishEyeFov};

            GlAnimator *gl_animator = GlAnimator::OfFloatArray(2, start_val, end_val, evaluator_2,
                                                               value_setter);
            gl_animator->set_duration(400);
            gl_animator->SetAnimateCallback(gl_animation_callback);
            manager->AddAnimators(gl_animator, ANIMATION_TRANSFORM, 0);

            render_controller->set_fov_factor(0);
        }
    }

    env->ReleaseLongArrayElements(poiners, ptrs, 0);
    return;
}

JNIEXPORT jint Java_com_evomotion_glrenderview_RenderView_readPixelsNative(JNIEnv *env, jclass type,
                                                                       jbyteArray bytes_,
                                                                       jlong renderPtr) {
    jbyte *bytes = env->GetByteArrayElements(bytes_, NULL);

    if (0 == renderPtr) {
        LOGE(TAG, "Invalid render pointers in %s", __FUNCTION__);
    }

    BaseRenderController *renderer = reinterpret_cast<BaseRenderController *>(renderPtr);
    int width = 0;
    int height = 0;
    if (renderer) {
        width = renderer->get_view_width();
        height = renderer->get_view_height();
    }

    GLubyte *pixel_data = reinterpret_cast<GLubyte *>(bytes);
    GLubyte *reverse_data;
    reverse_data = (GLubyte *) malloc((size_t) (width * height * 4));
    glReadPixels(0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, reverse_data);

    // up down turn
    for (int h = 0; h < height; ++h) {
        int new_h = height - h - 1;
        memcpy((pixel_data + new_h * width * 4), (reverse_data + h * width * 4),
               (size_t) (width * 4));
    }
    free(reverse_data);

    env->ReleaseByteArrayElements(bytes_, bytes, 0);
    int result = evo::CheckGlError("ReadPixels()");
    return result == 0 ? 0 : 1;
}

jint Java_com_evomotion_glrenderview_RenderView_nativeDisplayYuv420pPixelBytes(JNIEnv *env, jclass type,
                                                                           jbyteArray buffer_y,
                                                                           jbyteArray buffer_u,
                                                                           jbyteArray buffer_v,
                                                                           jint width,
                                                                           jint height,
                                                                           jlong texPtr) {
   if (texPtr == 0) {
        LOGE(TAG, "invalid texture pointer");
        return -1;
    }

    jsize buffer_y_length = env->GetArrayLength(buffer_y);
    jsize buffer_u_length = env->GetArrayLength(buffer_u);
    jsize buffer_v_length = env->GetArrayLength(buffer_v);
    jbyte *y_data = new jbyte[buffer_y_length];
    jbyte *u_data = new jbyte[buffer_u_length];
    jbyte *v_data = new jbyte[buffer_v_length];
    env->GetByteArrayRegion(buffer_y, 0, buffer_y_length, y_data);
    env->GetByteArrayRegion(buffer_u, 0, buffer_u_length, u_data);
    env->GetByteArrayRegion(buffer_v, 0, buffer_v_length, v_data);

    VideoTextureController *texture_controller = reinterpret_cast<VideoTextureController *>(texPtr);
    GLubyte *y_gl_data = reinterpret_cast<GLubyte *>(y_data);
    GLubyte *u_gl_data = reinterpret_cast<GLubyte *>(u_data);
    GLubyte *v_gl_data = reinterpret_cast<GLubyte *>(v_data);
    texture_controller->SetYuvTexData(y_gl_data, u_gl_data, v_gl_data, width, height);

    delete[] y_data;
    delete[] u_data;
    delete[] v_data;
    return 0;
}

jint Java_com_evomotion_glrenderview_RenderView_nativeDisplayNV12PixelBytes(JNIEnv *env, jclass type,
                                                                        jbyteArray y_,
                                                                        jbyteArray uv_, jint width,
                                                                        jint height, jlong texPtr) {
    if (texPtr == 0) {
        LOGE(TAG, "invalid texture pointer");
        return -1;
    }

    jbyte *y = env->GetByteArrayElements(y_, NULL);
    jbyte *uv = env->GetByteArrayElements(uv_, NULL);

    VideoTextureController *texture_controller = reinterpret_cast<VideoTextureController *>(texPtr);
    texture_controller->SetNV12TexData((GLubyte *) y, (GLubyte *) uv, width, height);

    env->ReleaseByteArrayElements(y_, y, 0);
    env->ReleaseByteArrayElements(uv_, uv, 0);

    return 0;
}

jint
Java_com_evomotion_glrenderview_RenderView_nativeDisplayYuv420pPixel(JNIEnv *env, jclass type, jlong y,
                                                                 jlong u, jlong v, jint width,
                                                                 jint height, jlong texPtr) {
    if (texPtr == 0 || y == 0 || u == 0 || v == 0) {
        LOGE(TAG, "invalid pointer");
        return -1;
    }

    VideoTextureController *texture_controller = reinterpret_cast<VideoTextureController *>(texPtr);

    GLubyte *y_gl_data = reinterpret_cast<GLubyte *>(y);
    GLubyte *u_gl_data = reinterpret_cast<GLubyte *>(u);
    GLubyte *v_gl_data = reinterpret_cast<GLubyte *>(v);
    texture_controller->SetYuvTexData(y_gl_data, u_gl_data, v_gl_data, width, height);

    /*  delete y_gl_data;
      delete u_gl_data;
      delete v_gl_data;*/

    return 0;
}

jint
Java_com_evomotion_glrenderview_RenderView_nativeDisplayNV12Pixel(JNIEnv *env, jclass type, jlong y,
                                                              jlong uv, jint width, jint height,
                                                              jlong texturePtr) {
    if (texturePtr == 0 || y == 0 || uv == 0) {
        LOGE(TAG, "invalid pointer");
        return -1;
    }

    VideoTextureController *texture_controller =
            reinterpret_cast<VideoTextureController *>(texturePtr);

    GLubyte *luminance = reinterpret_cast<GLubyte *>(y);
    GLubyte *chrominance = reinterpret_cast<GLubyte *>(uv);

    texture_controller->SetNV12TexData(luminance, chrominance, width, height);

    /*delete luminance;
    delete chrominance;*/

    return 0;
}

