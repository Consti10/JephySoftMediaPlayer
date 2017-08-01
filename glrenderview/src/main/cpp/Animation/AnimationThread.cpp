//
// Created by zhuyuanxuan on 01/03/2017.
// Copyright (c) 2017 Evomotion. All rights reserved.
//

#include "AnimationThread.h"

#include <pthread.h>

#include "LogUtils.h"

namespace evo {

    const int kDelayTime = 20;

    int AnimationThread::create(FuncWrapper *func_wrapper) {
        pthread_t thread0;
        int rc = pthread_create(&thread0, NULL, progressing, func_wrapper);
        if (rc) {
            LOGE("AnimationThread", "Error thread create fail in transition! %d", rc);
            return rc;
        }

        pthread_detach(thread0);
        return 0;
    }

    void *progressing(void *wrapper) {
        FuncWrapper *func_wrapper = reinterpret_cast<FuncWrapper *>(wrapper);
        GlAnimator *animator = func_wrapper->gl_animator;

        while (1) {
            int result = func_wrapper->inner_loop(animator);
            if (result) {
                break;
            }
            delay_ms(kDelayTime);
        }
        func_wrapper->thread_stop(animator);
        LOGE("InThread", "Stop thread");
        return (void *) 0;
    }

    void delay_ms(float time_in_ms) {
        clock_t start = clock();
        clock_t lay = static_cast<clock_t>(time_in_ms / 1000 * CLOCKS_PER_SEC);
        while ((clock() - start) < lay) {}
    }
}