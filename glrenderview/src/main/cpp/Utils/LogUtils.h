//
// Created by zhuyuanxuan on 18/11/2016.
// Copyright (c) 2016 Evomotion. All rights reserved.
//

#ifndef GL_RENDER_LOG_UTILS_H
#define GL_RENDER_LOG_UTILS_H

#define EVO_DEBUG

#     ifdef EVO_DEBUG

#       ifdef __ANDROID__
// LOGS ANDROID
#           include <android/log.h>

#           define LOGV(tag, ...) __android_log_print(ANDROID_LOG_VERBOSE, tag,__VA_ARGS__)
#           define LOGD(tag, ...) __android_log_print(ANDROID_LOG_DEBUG  , tag,__VA_ARGS__)
#           define LOGI(tag, ...) __android_log_print(ANDROID_LOG_INFO   , tag,__VA_ARGS__)
#           define LOGW(tag, ...) __android_log_print(ANDROID_LOG_WARN   , tag,__VA_ARGS__)
#           define LOGE(tag, ...) __android_log_print(ANDROID_LOG_ERROR  , tag,__VA_ARGS__)
#           define LOGSIMPLE(tag, ...)
#       else
// LOGS NO ANDROID
#           include <stdio.h>
#           define LOGV(tag, ...) printf("  ");printf(__VA_ARGS__); printf("\t -  <%s> \n", tag);
#           define LOGD(tag, ...) printf("  ");printf(__VA_ARGS__); printf("\t -  <%s> \n", tag);
#           define LOGI(tag, ...) printf("  ");printf(__VA_ARGS__); printf("\t -  <%s> \n", tag);
#           define LOGW(tag, ...) printf("  * Warning: "); printf(__VA_ARGS__); printf("\t -  <%s> \n", tag);
#           define LOGE(tag, ...) printf("  *** Error:  ");printf(__VA_ARGS__); printf("\t -  <%s> \n", tag);
#           define LOGSIMPLE(tag, ...) printf(" ");printf(__VA_ARGS__);
#       endif  // ANDROID

#     else
// no debug
#       define LOGV(tag, ...)
#       define LOGD(tag, ...)
#       define LOGI(tag, ...)
#       define LOGW(tag, ...)
#       define LOGE(tag, ...)
#       define LOGSIMPLE(tag, ...)
#     endif  // EVO_DEBUG
#endif  // GL_RENDER_LOG_UTILS_H
