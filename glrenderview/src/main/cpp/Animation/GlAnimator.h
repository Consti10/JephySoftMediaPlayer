//
// Created by zhuyuanxuan on 01/03/2017.
// Copyright (c) 2017 Evomotion. All rights reserved.
//

#ifndef EVOCLIENTFORANDROID_GLANIMATOR_H
#define EVOCLIENTFORANDROID_GLANIMATOR_H

#include "../RenderControl/BaseRenderController.h"
#include "ValueSetter.h"
#include "FuncWrapper.h"
#include "TypeEvaluator.h"
#include "AnimationPropertyValuesHolder.h"

#include <chrono>
#include <functional>

namespace evo {
    enum AnimationStatus {
        NO_STATUS,
        RUNNING,
        WILL_STOP,
        SHUTDOWN
    };

    class BaseGlAnimateCallback {

    public:

        BaseGlAnimateCallback() {}

        virtual ~BaseGlAnimateCallback() {}

        virtual int OnAnimationStart() = 0;

        virtual int onAnimationEnd() = 0;
    };

    template<typename T>
    class GlAnimateCallBack : public BaseGlAnimateCallback {
        typedef int(T::*CallbackFp)();

    public:
        GlAnimateCallBack(T *obj, CallbackFp start, CallbackFp stop) {
            m_impl_instance = obj;
            m_start_fp = start;
            m_stop_fp = stop;
        }

        virtual ~GlAnimateCallBack() {
            if (m_impl_instance != nullptr) {
                delete m_impl_instance;
            }
        }

        int OnAnimationStart() override {
            return (m_impl_instance->*m_start_fp)();
        }

        int onAnimationEnd() override {
            return (m_impl_instance->*m_stop_fp)();
        }

    private:
        T *m_impl_instance;
        CallbackFp m_start_fp;
        CallbackFp m_stop_fp;
    };

    class GlAnimator {

    public:

        static GlAnimator *OfFloatArray(int length, float *start, float *end,
                                        TypeEvaluator *evaluator, ValueSetter *setter);

        GlAnimator();

        virtual ~GlAnimator();

        /**
         * set animation duration, if set duration == -999, means animate to zero.
         * @param duration in millisecond
         */
        void set_duration(const int duration);

        void SetAnimateCallback(BaseGlAnimateCallback *callback);

        int StartAnimator();

        void StopAnimator(std::function<void()> func);

        /**
         *
         * @return 1 if running 0 if shutdown
         */
        int GetStatus();

        friend int LoopInThread(GlAnimator *animator_ptr);

        friend int OnThreadStop(GlAnimator *animator);

    private:
        AnimationStatus m_status;
        FuncWrapper *m_wrapper;
        AnimationPropertyValuesHolder<float *> *m_values_holder;
        std::chrono::high_resolution_clock::time_point m_start_time_point;
        int m_duration;
        long m_last_frame_time;
        BaseGlAnimateCallback *m_animator_callback;
        std::function<void()> m_shutdown;

        /**
         * Every loop processing, will call Evaluator to calcualte value
         * @return 1 if go to end, 0 continue.
         */
        int ProcessAnimation();
    };

    // declare the friend function again
    int LoopInThread(GlAnimator *animator_ptr);

    int OnThreadStop(GlAnimator *animator);
}

#endif
// EVOCLIENTFORANDROID_GLANIMATOR_H
