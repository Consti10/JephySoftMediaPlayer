//
// Created by zhuyuanxuan on 22/03/2017.
// Copyright (c) 2017 Evomotion. All rights reserved.
//

#ifndef EVOCLIENTFORANDROID_ANIMATEMANAGER_H
#define EVOCLIENTFORANDROID_ANIMATEMANAGER_H

#include "GlAnimator.h"

#include <deque>

namespace evo {

    class AnimateManager {
    public:
        AnimateManager();

        virtual ~AnimateManager();

        /**
         *
         * @param animator GlAnimator obj
         * @param type          enum AnimationType {
                                NULL_ANIMATION_TYPE
                                ANIMATION_INERTIA,
                                ANIMATION_RESET,
                                ANIMATION_TRANSFORM }
         * @param running_type 0 if nothing special, 1 if draw serial, 2 if draw parallel
         */
        void AddAnimators(GlAnimator *animator, AnimationType type, int running_type);

        void CheckingStatus();

        void StopAllAnimation();

    private:
        std::deque<GlAnimator *> m_animators;
        AnimationType m_animation_type;
    };
}
#endif
// EVOCLIENTFORANDROID_ANIMATEMANAGER_H
