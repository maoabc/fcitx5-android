//
// Created by mao on 2025/9/10.
//

#ifndef FCITX5_ANDROID_MY_LOG_H
#define FCITX5_ANDROID_MY_LOG_H
#include "android/log.h"

#define LOG_TAG "MyCusFcitx"
#define  ALOGI(...)  __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define  ALOGE(...)  __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)
#define  ALOGD(...)  __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)
#define  ALOGW(...)  __android_log_print(ANDROID_LOG_WARN, LOG_TAG, __VA_ARGS__)

#endif //FCITX5_ANDROID_MY_LOG_H
