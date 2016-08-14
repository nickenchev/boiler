LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := boiler_shared_sdl2
LOCAL_SRC_FILES := ../../deps/libSDL2.so

include $(PREBUILT_SHARED_LIBRARY)
