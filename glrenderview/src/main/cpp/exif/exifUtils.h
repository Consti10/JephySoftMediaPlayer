//
// Created by MacBook on 2017/4/10.
//

#ifndef EVOCLIENTFORANDROID_EXIFUTILS_H
#define EVOCLIENTFORANDROID_EXIFUTILS_H

#include <jni.h>

jstring
Java_com_evomotion_utils_ExifUtils_getWhiteBalance(JNIEnv *env, jobject type, jstring filePath_);

jstring Java_com_evomotion_utils_ExifUtils_getExposureBiasValue(JNIEnv *env, jobject type,
                                                                jstring filePath_);

#endif //EVOCLIENTFORANDROID_EXIFUTILS_H
