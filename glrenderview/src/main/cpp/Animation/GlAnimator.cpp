//
// Created by zhuyuanxuan on 01/03/2017.
// Copyright (c) 2017 Evomotion. All rights reserved.
//

#include "GlAnimator.h"

#include "AnimationThread.h"

const char TAG[] = "GlAnimator";

typedef std::chrono::high_resolution_clock HighClock;

namespace evo {

    GlAnimator *GlAnimator::OfFloatArray(int length, float *start, float *end,
                                         TypeEvaluator *evaluator, ValueSetter *setter) {
        GlAnimator *gl_animator;
        gl_animator = new GlAnimator();
        gl_animator->m_values_holder = new AnimationPropertyValuesHolder<float *>(start, end,
                                                                                  length,
                                                                                  evaluator);
        gl_animator->m_values_holder->SetValueSetter(setter);
        LOGE(TAG, "Create animation: ");
        for (int i = 0; i < length; ++i) {
            LOGI(TAG, "[%d]: from %f ~ %f.", i, start[i], end[i]);
        }

        return gl_animator;
    }

    GlAnimator::GlAnimator() {
        m_animator_callback = nullptr;
        m_wrapper = nullptr;
        m_shutdown = nullptr;
        m_status = SHUTDOWN;
    }

    GlAnimator::~GlAnimator() {
        LOGI(TAG, "Destruct of GlAnimator");
        if (m_wrapper != nullptr) { delete m_wrapper; }
        if (m_values_holder != nullptr) { delete m_values_holder; }
        if (m_animator_callback != nullptr) { delete m_animator_callback; }
    }


    void GlAnimator::SetAnimateCallback(BaseGlAnimateCallback *callback) {
        m_animator_callback = callback;
    }

    void GlAnimator::set_duration(const int duration) {
        GlAnimator::m_duration = duration;
    }

    int GlAnimator::StartAnimator() {
        m_wrapper = new FuncWrapper();
        m_wrapper->gl_animator = this;
        m_wrapper->inner_loop = &LoopInThread;
        m_wrapper->thread_stop = &OnThreadStop;

        m_last_frame_time = 0;

        if (m_animator_callback != nullptr) {
            int if_start = m_animator_callback->OnAnimationStart();
            if (if_start) {
                LOGI(TAG, "OnAnimationStart return 1, stop this animation");
                m_status = SHUTDOWN;
                return 1;
            }
        }

        m_status = RUNNING;
        int success = AnimationThread::create(m_wrapper);
        if (success) {
            LOGE(TAG, "Create animation thread fail!");
            m_status = SHUTDOWN;
            return 1;
        }
        return 0;
    }

    ////////////
    //  private functions
    ///////////

    /**
     * First judge the animation status, if OK, call Interpolator, Evaluator
     * to calculate, when finish, call callback function.
     */
    int GlAnimator::ProcessAnimation() {
        // judge the status
        if (m_status != RUNNING) {
            return 1;
        }
        // first calculate fraction
        double fraction;
        if (m_last_frame_time == 0) {
            fraction = 0.0f;
            m_start_time_point = HighClock::now();
        } else {
            std::chrono::high_resolution_clock::time_point now = HighClock::now();
            long ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                    now - m_start_time_point).count();
            fraction = ms * 1.0f / m_duration;

            fraction = fraction >= 1.0f ? 1.0f : fraction;
        }

        m_last_frame_time = 1;

        // Interpolator
        // ...

        // Evaluator
        if (m_values_holder != nullptr) {
            m_values_holder->CalculateValue((float) fraction);
        } else {
            LOGE(TAG, "No ValuesHolder, exit!");
            return 1;
        }

        if (fraction < 1.0f) {
            // continue animation
            return 0;
        } else {
            LOGI(TAG, "Will stop thread");
            return 1;
        }
    }

    int GlAnimator::GetStatus() {
        return m_status == SHUTDOWN ? 0 : 1;
    }

    void GlAnimator::StopAnimator(std::function<void()> func) {
        m_status = WILL_STOP;
        m_shutdown = func;
    }

    // friend function
    int LoopInThread(GlAnimator *animator_ptr) {
        return animator_ptr->ProcessAnimation();
    }

    int OnThreadStop(GlAnimator *animator) {
        // will judge the status in callback functions so should set status first
        animator->m_status = SHUTDOWN;

        if (animator->m_animator_callback != nullptr) {
            animator->m_animator_callback->onAnimationEnd();
        }

        if (animator->m_shutdown != nullptr) {
            animator->m_shutdown();
        }

        LOGI(TAG, "Shut down animation");
        return 0;
    }
}