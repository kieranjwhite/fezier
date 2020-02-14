LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE    := tiles
LOCAL_SRC_FILES := render.c rendering/draw.c rtu.c

#for debugging
#LOCAL_CFLAGS += -O0 -gdwarf-2 -g3 -march=armv7-a -mfloat-abi=softfp -mfpu=vfpv3 -mfpu=neon -ftree-vectorize -DNDK -DHAVE_TYPEOF -DHAVE_BUILTIN_CHOOSE_EXPR -DHAVE_BUILTIN_TYPES_COMPATIBLE -std=c99

#for release -- be wary of enabling neon though, it may trigger asserts when comparing wiped area to dirty area in draw_canvasCheckAndClear, presumabely due to neon and normal fp giving different results to some calculations
#didn't debug further as we can't run debugger when APP_ABI is armeabi-v7a-hard
LOCAL_CFLAGS += -O3 -mfloat-abi=softfp -mfpu=vfpv3 -mfpu=neon -ftree-vectorize -DNDK -DHAVE_TYPEOF -DHAVE_BUILTIN_CHOOSE_EXPR -DHAVE_BUILTIN_TYPES_COMPATIBLE -std=c99
#LOCAL_CFLAGS += -O3 -march=armv7-a -mfloat-abi=softfp -mfpu=vfpv3 -mfpu=neon -ftree-vectorize -DNDK -DHAVE_TYPEOF -DHAVE_BUILTIN_CHOOSE_EXPR -DHAVE_BUILTIN_TYPES_COMPATIBLE -std=c99

#for profiling
#LOCAL_CFLAGS += -O3 -march=armv7-a -mfloat-abi=softfp -mfpu=vfpv3 -mfpu=neon -ftree-vectorize -DNDK -DHAVE_TYPEOF -DHAVE_BUILTIN_CHOOSE_EXPR -DHAVE_BUILTIN_TYPES_COMPATIBLE -std=c99 -pg

LOCAL_LDLIBS := -llog
#LOCAL_STATIC_LIBRARIES := android-ndk-profiler
LOCAL_STATIC_LIBRARIES :=

#comment out to debug
APP_ABI := armeabi-v7a-hard

include $(BUILD_SHARED_LIBRARY)

#$(call import-module,android-ndk-profiler)
