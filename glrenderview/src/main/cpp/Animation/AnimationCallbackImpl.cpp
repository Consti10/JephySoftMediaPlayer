//
// Created by zhuyuanxuan on 02/03/2017.
// Copyright (c) 2017 Evomotion. All rights reserved.
//

#include "AnimationCallbackImpl.h"

#include <cmath>

#include "LogUtils.h"

const char TAG[] = "AnimationCallbackImpl";
const float kZero = 0.00001f;

namespace evo {
    AnimationCallbackResetImpl::AnimationCallbackResetImpl(BaseRenderController *m_renderer)
            : m_renderer(m_renderer) {
    }

    AnimationCallbackResetImpl::~AnimationCallbackResetImpl() {
        LOGI(TAG, "Destruct of AnimationCallbackResetImpl");
    }

    int AnimationCallbackResetImpl::Start() {
        float tmp_lon = m_renderer->get_lon();
        float tmp_lat = m_renderer->get_lat();

        if (fabsf(tmp_lon) < kZero && fabsf(tmp_lat) < kZero) {
            LOGE(TAG, "Already reset!");
            return 1;
        }
        m_renderer->set_animate_status(RENDER_RESET);
        return 0;
    }


    int AnimationCallbackResetImpl::Stop() {
        m_renderer->set_animate_status(RENDER_DRAWING);
        return 0;
    }

    ////
    // InertiaImpl
    ////

    AnimationCallbackInertiaImpl::AnimationCallbackInertiaImpl(BaseRenderController *renderer)
            : m_renderer{renderer} {}

    AnimationCallbackInertiaImpl::~AnimationCallbackInertiaImpl() {
        LOGE(TAG, "Destruct of AnimationCallbackInertiaImpl");
    }

    int AnimationCallbackInertiaImpl::OnStart() {
        if (m_renderer == nullptr) {
            LOGE(TAG, "Err! render null");
            return 1;
        }
        m_renderer->set_animate_status(RENDER_INERTIA);
        return 0;
    }

    int AnimationCallbackInertiaImpl::OnStop() {
        m_renderer->set_animate_status(RENDER_DRAWING);
        return 0;
    }

    ////
    // TransformImpl
    ////

    AnimationCallbackTransformImpl::AnimationCallbackTransformImpl(
            SphereRenderController *renderer, DisplayMode mode)
            : m_renderer{renderer},
              m_next_mode{mode} {
    }

    AnimationCallbackTransformImpl::~AnimationCallbackTransformImpl() {
        LOGE(TAG, "Destruct of AnimationCallbackTransformImpl");
    }

    int AnimationCallbackTransformImpl::OnStart() {
        m_renderer->set_animate_status(RENDER_TRANSFORM);
        return 0;
    }

    /**
     * After transform rendermode finished, set new rendermode
     * and set render render status to "RENDER_DRAWING"
     * @return 0 when success
     */
    int AnimationCallbackTransformImpl::OnStop() {
        // !! when add more than one animation same time,
        // will set to next mode A.S.A. the first animation finished
        m_renderer->set_mode(m_next_mode);
        m_renderer->set_animate_status(RENDER_DRAWING);
        m_renderer->set_face_culling(1);

        LOGE(TAG, "%s to mode %d", __PRETTY_FUNCTION__, m_next_mode);
        return 0;
    }
}
