# Main Application
LOCAL_PATH := ../../src/boiler

include $(CLEAR_VARS)

APP_ABI := armeabi
LOCAL_MODULE := boiler
LOCAL_CPP_FEATURES += exceptions
LOCAL_LDLIBS := -lGLESv3 -latomic -lz
LOCAL_SHARED_LIBRARIES := SDL2 SDL2_image
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH) ../../include
LOCAL_C_INCLUDES := /usr/local/include \
	../../include

LOCAL_SRC_FILES := core/engine.cpp \
	core/entity.cpp \
	core/part.cpp \
	core/component.cpp \
	core/tmxloader.cpp \
	core/spriteloader.cpp \
	core/spritesheet.cpp \
	core/spritesheetframe.cpp \
	core/base64.cpp \
	core/quadtree.cpp \
	core/rect.cpp \
	core/raycaster.cpp \
	core/ray.cpp \
	core/bmfont.cpp \
	core/spriteanimation.cpp \
	core/line.cpp \
	core/fileutils.cpp \
	video/renderer.cpp \
	video/model.cpp \
	video/fontloader.cpp \
	video/vertexdata.cpp \
	video/openglrenderer.cpp \
	video/opengltexture.cpp \
	video/openglmodel.cpp \
	video/shaderprogram.cpp \
	video/imageloader.cpp camera/camera.cpp \
	camera/platformercamera.cpp \
	camera/pancamera.cpp \
	containers/gridposition.cpp \
	util/tinyxml2.cpp \
	util/jsoncpp.cpp

include $(BUILD_SHARED_LIBRARY)