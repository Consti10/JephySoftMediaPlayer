//
// Created by MacBook on 2017/5/17.
//

#ifndef EVOCLIENTFORANDROID_VIDEOUTIL_H
#define EVOCLIENTFORANDROID_VIDEOUTIL_H

#include <jni.h>

jbyteArray
Java_com_evomotion_utils_VideoUtil_nativeConvertNativeDataToByteArray(JNIEnv *env, jclass type,
                                                                      jlong nativePoint,
                                                                      jlong size);

#endif //EVOCLIENTFORANDROID_VIDEOUTIL_H
