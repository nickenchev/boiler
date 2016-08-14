LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := boiler_shared_sdl2_image
LOCAL_SRC_FILES := ../../deps/libSDL2_image.so

include $(PREBUILT_SHARED_LIBRARY)
