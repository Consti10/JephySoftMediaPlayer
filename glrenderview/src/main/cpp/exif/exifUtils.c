//
// Created by MacBook on 2017/4/7.
//
#include <jni.h>
#include <stddef.h>
#include "exifUtils.h"
#include "exif.h"


JNIEXPORT jstring JNICALL
Java_com_evomotion_utils_ExifUtils_getWhiteBalance(JNIEnv *env, jobject type, jstring filePath_) {
    const char *filePath = (*env)->GetStringUTFChars(env, filePath_, 0);
    void **ifdArray;
    TagNodeInfo *tag;
    int result;
    ifdArray = createIfdTableArray(filePath, &result);
    tag = getTagInfo(ifdArray, IFD_EXIF, TAG_WhiteBalance);
    jstring wb = NULL;
    if (tag) {
        if (!tag->error) {
            wb = (*env)->NewStringUTF(env, (const char *) tag->byteData);
        }
        freeTagInfo(tag);
    }

    (*env)->ReleaseStringUTFChars(env, filePath_, filePath);
    return wb;
}

JNIEXPORT jstring JNICALL
Java_com_evomotion_utils_ExifUtils_getExposureBiasValue(JNIEnv *env, jobject type,
                                                        jstring filePath_) {
    const char *filePath = (*env)->GetStringUTFChars(env, filePath_, 0);
    void **ifdArray;
    TagNodeInfo *tag;
    int result;
    ifdArray = createIfdTableArray(filePath, &result);
    tag = getTagInfo(ifdArray, IFD_EXIF, TAG_ExposureBiasValue);
    jstring ev = NULL;
    if (tag) {
        if (!tag->error) {
            ev = (*env)->NewStringUTF(env, (const char *) tag->byteData);
        }
        freeTagInfo(tag);
    }

    (*env)->ReleaseStringUTFChars(env, filePath_, filePath);
    return ev;
}