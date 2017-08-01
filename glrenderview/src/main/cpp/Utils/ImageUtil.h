
#ifndef EVOCLIENTFORANDROID_IMAGEUTIL_H
#define EVOCLIENTFORANDROID_IMAGEUTIL_H

#include <jni.h>

extern "C"
jbyteArray
Java_com_evomotion_utils_ResourceMediaInfoUtil_nv12ptrToByteArray(JNIEnv *env, jclass type, jlong y,
                                                                  jlong uv, jlong width,
                                                                  jlong height);

#endif //EVOCLIENTFORANDROID_IMAGEUTIL_H
