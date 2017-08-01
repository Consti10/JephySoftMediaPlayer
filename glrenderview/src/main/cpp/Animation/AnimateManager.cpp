//
// Created by zhuyuanxuan on 22/03/2017.
// Copyright (c) 2017 Evomotion. All rights reserved.
//

#include "AnimateManager.h"

const char TAG[] = "AnimateManager";

namespace evo {

    AnimateManager::AnimateManager() {
        m_animation_type = NULL_ANIMATION_TYPE;
    }

    AnimateManager::~AnimateManager() {

    }

    /**
     * the animation list: front[old_animation, new_animation]back
     * use `push_back()`
     */
    void AnimateManager::AddAnimators(GlAnimator *animator, AnimationType type, int running_type) {
        CheckingStatus();

        if (running_type == 2) {
            animator->StartAnimator();
            m_animators.push_back(animator);
            return;
        }

        if (m_animators.size() == 0) {
            animator->StartAnimator();
            m_animators.push_back(animator);
            m_animation_type = type;
        } else {
            if (type == m_animation_type) {
                switch (type) {
                    case ANIMATION_INERTIA:
//                        delete animator;
//                        return;
                        m_animators.front()->StopAnimator([this, animator, type]() {
                            LOGE(TAG, "Stop the last one!");
                            AddAnimators(animator, type, 0);
                        });
                        break;
                    case ANIMATION_RESET:
                        delete animator;
                        return;
                    case ANIMATION_TRANSFORM:
                        delete animator;
                        return;
                    default:
                        delete animator;
                        break;
                }
            } else if (type < m_animation_type) {
                delete animator;
                LOGE(TAG, "skip this animation");
                return;
            } else {
                // skip the previous animation then add this animation
                LOGE(TAG, "Should skip previous");
                m_animators.front()->StopAnimator([this, animator, type]() {
                    LOGE(TAG, "Have stopped the previous animation");
                    AddAnimators(animator, type, 0);
                });
//                delete animator;
            }
        }  // size > 0
    }

    void AnimateManager::CheckingStatus() {
        for (auto it = m_animators.begin(); it != m_animators.end();) {
            GlAnimator *tmp_animator = *it;
            if (tmp_animator->GetStatus()) {
                LOGE(TAG, "Seems there is a old animation running!");
                ++it;
            } else {
                it = m_animators.erase(it);
                delete tmp_animator;
                LOGI(TAG, "Delete finished animation");
            }
        }  // end of loop
    }

    void AnimateManager::StopAllAnimation() {
        for (auto it = m_animators.begin(); it != m_animators.end();) {
            GlAnimator *this_animator = *it;
            if (this_animator->GetStatus()) {
                // still running
                this_animator->StopAnimator([]() {
                });
                ++it;
            } else {
                // already stopped
                it = m_animators.erase(it);
                delete this_animator;
                LOGI(TAG, "Delete finished animation %s ", __PRETTY_FUNCTION__);
            }
        }  // end of loop
    }
}
