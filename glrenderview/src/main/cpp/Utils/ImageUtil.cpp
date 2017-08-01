#include <GLES3/gl3.h>
#include <stdlib.h>
#include <string.h>
#include "ImageUtil.h"

JNIEXPORT jbyteArray JNICALL
Java_com_evomotion_utils_ResourceMediaInfoUtil_nv12ptrToByteArray(JNIEnv *env, jclass type, jlong y,
                                                                  jlong uv, jlong width,
                                                                  jlong height) {
    jbyteArray bytes = env->NewByteArray((jsize) (width * height * 3 / 2));
    jbyte *yData = reinterpret_cast<jbyte *>(y);
    jbyte *uvData = reinterpret_cast<jbyte *>(uv);
    env->SetByteArrayRegion(bytes,
                            0,
                            (jsize) (width * height), yData);
    env->SetByteArrayRegion(bytes,
                            (jsize) (width * height),
                            (jsize) (width * height * 1 / 2),
                            uvData);

    return bytes;
}