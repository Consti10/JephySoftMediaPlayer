//
// Created by zhuyuanxuan on 01/03/2017.
// Copyright (c) 2017 Evomotion. All rights reserved.
//

#ifndef EVOCLIENTFORANDROID_FUNCWRAPPER_H
#define EVOCLIENTFORANDROID_FUNCWRAPPER_H


namespace evo {
    class GlAnimator;

    struct FuncWrapper {
        GlAnimator *gl_animator;

        int (*inner_loop)(GlAnimator *);

        int (*thread_stop)(GlAnimator *);
    };

    enum AnimationType {
        NULL_ANIMATION_TYPE,
        ANIMATION_INERTIA,
        ANIMATION_RESET,
        ANIMATION_TRANSFORM
    };


}
#endif
// EVOCLIENTFORANDROID_FUNCWRAPPER_H
