//
// Created by zhuyuanxuan on 01/03/2017.
// Copyright (c) 2017 Evomotion. All rights reserved.
//

#ifndef EVOCLIENTFORANDROID_ANIMATIONTHREAD_H
#define EVOCLIENTFORANDROID_ANIMATIONTHREAD_H

#include "FuncWrapper.h"

namespace evo {
    struct AnimatorLooper {
        GlAnimator *gl_animator;
    };

    class AnimationThread {
    public:
        static int create(FuncWrapper *func_wrapper);
    };

    void *progressing(void *wrapper);

    void delay_ms(float time_in_ms);
}
#endif
// EVOCLIENTFORANDROID_ANIMATIONTHREAD_H
