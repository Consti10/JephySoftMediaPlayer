//
// Created by zhuyuanxuan on 27/02/2017.
// Copyright (c) 2017 Evomotion. All rights reserved.
//

#include <jni.h>

#include "md5.h"

extern "C" {
jstring Java_com_evomotion_utils_Md5Generate_signature(JNIEnv *env, jclass type, jstring str_);
}

JNIEXPORT jstring Java_com_evomotion_utils_Md5Generate_signature(
        JNIEnv *env, jclass type, jstring str_) {
    const char *str = env->GetStringUTFChars(str_, 0);

    std::string string = std::string(str);
    string.append("&secret=evomotion@!!!.com");
    std::string md5_result = md5(string);

    env->ReleaseStringUTFChars(str_, str);

    return env->NewStringUTF(md5_result.c_str());
}