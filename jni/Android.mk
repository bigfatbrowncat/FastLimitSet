LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE    := FastLimitSet
LOCAL_SRC_FILES := FastLimitSet.cpp

include $(BUILD_SHARED_LIBRARY)
