LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE    := com.giby.StarBird

LOCAL_CFLAGS := -DANDROID_NDK \
                -DDISABLE_IMPORTGL

              
LOCAL_LDLIBS    := -lGLESv1_CM -llog

#LOCAL_C_INCLUDES        := $(LOCAL_PATH)
#LOCAL_C_INCLUDES := $(wildcard $(LOCAL_PATH)/src/*.h)

#MY_LOCAL_C_INCLUDES := 

#LOCAL_C_INCLUDES := $(addprefix $(LOCAL_PATH)/,$(MY_LOCAL_C_INCLUDES)) $(STL_INC_DIR) 

#LOCAL_SRC_FILES         := src/Bound.cpp \
#	src/MainAndroid.c

FILE_LIST := src/MainAndroid.c $(wildcard $(LOCAL_PATH)/src/*.cpp) $(wildcard $(LOCAL_PATH)/src/DLib/*.cpp) $(wildcard $(LOCAL_PATH)/src/FreeGlut/*.c)
LOCAL_SRC_FILES := $(FILE_LIST:$(LOCAL_PATH)/%=%)


include $(BUILD_SHARED_LIBRARY)
