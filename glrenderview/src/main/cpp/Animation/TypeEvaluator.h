//
// Created by zhuyuanxuan on 03/03/2017.
// Copyright (c) 2017 Evomotion. All rights reserved.
//

#ifndef EVOCLIENTFORANDROID_TYPEEVALUATOR_H
#define EVOCLIENTFORANDROID_TYPEEVALUATOR_H

#include "LogUtils.h"

namespace evo {
    class TypeEvaluator {
    public:
        virtual ~TypeEvaluator() {}

        TypeEvaluator(int length);

        virtual void
        FloatEvaluate(float fraction, float *start_value, float *velocity, float **result) = 0;

    protected:
        int m_length;
    };

    class NormalEvaluator : public TypeEvaluator {
    public:
        NormalEvaluator(int length);

        virtual ~NormalEvaluator() {}

        void FloatEvaluate(float fraction, float *start_value, float *end_value,
                           float **result) override;

    };

    class ZeroEvaluator : public TypeEvaluator {
    public:
        ZeroEvaluator(int length);

        virtual ~ZeroEvaluator();

        /**
         * Calcualte result until 0, X = V0t + (1/2)at^2
         * @param fraction fraction
         * @param start_value the start X(the distance)
         * @param velocity useless
         * @param result will return value by result
         */
        void FloatEvaluate(float fraction, float *start_value, float *velocity,
                float **result) override;

        void set_acceleration(float a);

        void set_duration(int duration);

    private:
        float m_abs_acceler;
        int m_duration;
        int m_acceleration_direct[10];
        float *m_end_vals;
    };
}

#endif
// EVOCLIENTFORANDROID_TYPEEVALUATOR_H
