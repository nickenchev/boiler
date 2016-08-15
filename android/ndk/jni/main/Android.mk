LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := main

SDL_PATH := ../SDL
GAME_SRC_PATH := ../../../../src/terrain

LOCAL_C_INCLUDES := $(LOCAL_PATH)/$(SDL_PATH)/include

# Add your application source files here...
LOCAL_SRC_FILES := $(SDL_PATH)/src/main/android/SDL_android_main.c \
	$(GAME_SRC_PATH)/main.cpp \
	$(GAME_SRC_PATH)/terrainpart.cpp

LOCAL_SHARED_LIBRARIES := SDL2 boiler

LOCAL_LDLIBS := -lGLESv1_CM -lGLESv2 -llog

include $(BUILD_SHARED_LIBRARY)
