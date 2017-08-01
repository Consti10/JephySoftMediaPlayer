//
// Created by zhuyuanxuan on 03/03/2017.
// Copyright (c) 2017 Evomotion. All rights reserved.
//

#include "TypeEvaluator.h"

#include <math.h>

const float kZero = 0.00001f;

namespace evo {
    TypeEvaluator::TypeEvaluator(int length) : m_length(length) {}

    //////////////
    // NormalEvaluator
    /////////////

    NormalEvaluator::NormalEvaluator(int length) : TypeEvaluator(length) {}

    void NormalEvaluator::FloatEvaluate(float fraction, float *start_value, float *end_value,
                                        float **result) {
        if (m_length < 1) {
            return;
        }

        for (int i = 0; i < m_length; ++i) {
            (*result)[i] = (end_value[i] - start_value[i]) * fraction + start_value[i];
        }
    }

    //////////////
    // ZeroEvaluator
    /////////////

    ZeroEvaluator::ZeroEvaluator(int length) : TypeEvaluator(length) {
        m_abs_acceler = -1.0f;
        for (int i = 0; i < m_length; ++i) {
            m_acceleration_direct[i] = 0;
        }
        m_end_vals = new float[length];
    }

    ZeroEvaluator::~ZeroEvaluator() {
        if (m_end_vals != nullptr) {
            delete m_end_vals;
        }
    }

    void ZeroEvaluator::FloatEvaluate(float fraction, float *start_value, float *velocity,
                                      float **result) {
        float current_t = fraction * m_duration / 1000.0f;

        // assign the direction do this once
        if (!m_acceleration_direct[0]) {
            for (int i = 0; i < m_length; ++i) {
//                m_acceleration_direct[i] = velocity[i] > 0.0001f ? -1 : 1;
                float a = m_abs_acceler;
                if (velocity[i] > kZero) {
                    a *= -1.0f;
                }
                m_end_vals[i] = start_value[i] + (-0.5f * velocity[i] * velocity[i] / a);
                LOGE("xx", "End val %d: %f", i, m_end_vals[i]);
            }
            m_acceleration_direct[0] = 1;
        }
        for (int i = 0; i < m_length; ++i) {
            float a = m_abs_acceler;
            if (velocity[i] > kZero) {
                a *= -1.0f;
            }
            (*result)[i] = (float) (
                    start_value[i] + velocity[i] * current_t +
                    0.5 * a * current_t * current_t);

            if (current_t > fabsf(velocity[i] / m_abs_acceler)) {
                (*result)[i] = m_end_vals[i];
//                LOGE("xx", "Go to end of %d val is %f", i, m_end_vals[i]);
            }
        }
    }

    void ZeroEvaluator::set_acceleration(float a) {
        m_abs_acceler = a;
    }

    void ZeroEvaluator::set_duration(int duration) {
        m_duration = duration;
    }
}

