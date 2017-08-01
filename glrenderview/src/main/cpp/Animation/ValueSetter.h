//
// Created by zhuyuanxuan on 02/03/2017.
// Copyright (c) 2017 Evomotion. All rights reserved.
//

#ifndef EVOCLIENTFORANDROID_VALUESETTER_H
#define EVOCLIENTFORANDROID_VALUESETTER_H

#include <stdarg.h>

#include "LogUtils.h"

namespace evo {
    class ValueSetter {
    public:
        virtual ~ValueSetter();

        virtual void SetFloatVal(int length, float *array) = 0;
    };

    template<typename T>
    class AnimeValueSetter : public ValueSetter {
        typedef void(T::*SetterOneParam)(float);

        typedef void(T::*SetterTwoParam)(float, float);

        typedef void(T::*SetterThreeParam)(float, float, float);

    public:
        AnimeValueSetter(T *obj) : m_controller{obj} {}

        virtual ~AnimeValueSetter() {}

        void SetFuncP(SetterOneParam one) {
            m_fun_1 = one;
        }

        void SetFuncP(SetterTwoParam two) {
            m_fun_2 = two;
        }

        void SetFuncP(SetterThreeParam three) {
            m_fun_3 = three;
        }

        void SetFloatVal(int length, float *array) override {
            if (m_controller == nullptr) {
                LOGE("ValueSetter", "controller invalid");
                return;
            }

            switch (length) {
                case 1:
                    if (m_fun_1 == nullptr) {
                        LOGE("Value setter", "No 1 arg func");
                    }
                    (m_controller->*m_fun_1)(array[0]);
//                    LOGE("xxx", "set value %f.", array[0]);
                    break;
                case 2:
                    (m_controller->*m_fun_2)(array[0], array[1]);
//                    LOGE("xxx", "set value %f %f", array[0], array[1]);
                    break;
                case 3:
                    (m_controller->*m_fun_3)(array[0], array[1], array[2]);
//                    LOGE("xxx", "Three params %f %f %f", array[0], array[1], array[2]);
                default:
                    break;
            }
        }


    private:
        T *m_controller;
        SetterOneParam m_fun_1;
        SetterTwoParam m_fun_2;
        SetterThreeParam m_fun_3;
    };

}

#endif
// EVOCLIENTFORANDROID_VALUESETTER_H
