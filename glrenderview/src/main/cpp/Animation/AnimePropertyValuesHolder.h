//
// Created by zhuyuanxuan on 02/03/2017.
// Copyright (c) 2017 Evomotion. All rights reserved.
//

#ifndef EVOCLIENTFORANDROID_ANIMEPROPERTYVALUESHOLDER_H
#define EVOCLIENTFORANDROID_ANIMEPROPERTYVALUESHOLDER_H

#include "TypeEvaluator.h"
#include "ValueSetter.h"
#include <cstdlib>

namespace evo {
    template<typename T>
    class AnimePropertyValuesHolder {

    public:
        // @param m_start_val must be a pointer type e.g. (float *)
        AnimePropertyValuesHolder(T m_start_val, T m_stop_val, int m_length,
                                  TypeEvaluator *evaluator) :
                m_stop_val(m_stop_val),
                m_start_val(m_start_val),
                m_length(m_length),
                m_evaluator(evaluator) {
            // malloc
            m_this_frame_val = (T) malloc((sizeof m_start_val[0]) * m_length);
        }

        virtual ~AnimePropertyValuesHolder() {
            if (m_this_frame_val != nullptr) {
                free(m_this_frame_val);
            }

            if (m_evaluator != nullptr) {
                delete m_evaluator;
            }
        }

        void SetValueSetter(ValueSetter *setter) {
            AnimePropertyValuesHolder::m_value_setter = setter;
        }

        T CalculateValue(float fraction) {
            m_evaluator->FloatEvaluate(fraction, m_start_val, m_stop_val, &m_this_frame_val);
            AnimePropertyValuesHolder::AnimateValue();
            return m_this_frame_val;
        }

    private:
        T m_start_val;
        T m_stop_val;
        T m_this_frame_val;
        TypeEvaluator *m_evaluator;
        ValueSetter *m_value_setter;
        int m_length;

        void AnimateValue() {
            m_value_setter->SetFloatVal(m_length, m_this_frame_val);
        }
    };
}

#endif
// EVOCLIENTFORANDROID_ANIMEPROPERTYVALUESHOLDER_H
