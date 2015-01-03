# An awfully complicated commands to build LLVM on OS X 10.9
# 1. Download and extract llvm-3.5.0.src.tar.xz
# 2. Follow the instructions in llvm-3.5.0.src/cmake/platforms/Android.cmake excepting the last line
# 3. Instead of that last line, use
#    /Applications/CMake.app/Contents/bin/cmake -DLLVM_ANDROID_TOOLCHAIN_DIR=/Users/il/arm-linux-androideabi-4.8/ -DCMAKE_TOOLCHAIN_FILE=../../llvm-3.5.0.src/cmake/platforms/Android.cmake -DLLVM_BUILD_TOOLS=False -DLLVM_ENABLE_PIC=False -DLLVM_TARGET_ARCH=ARM -DLLVM_TARGETS_TO_BUILD=ARM -DLLVM_TABLEGEN=/Users/il/Projects/padtest/llvm_and_clang/build/bin/llvm-tblgen -DCMAKE_RANLIB=/Users/il/arm-linux-androideabi-4.8/bin/arm-linux-androideabi-ranlib -DCMAKE_AR=/Users/il/arm-linux-androideabi-4.8/bin/arm-linux-androideabi-ar -DCMAKE_LINKER=/Users/il/arm-linux-androideabi-4.8/bin/arm-linux-androideabi-ld ../../llvm-3.5.0.src/

LOCAL_PATH := $(call my-dir)

#-lLLVMARMDisassembler -lLLVMARMCodeGen -lLLVMSelectionDAG -lLLVMAsmPrinter -lLLVMCodeGen -lLLVMScalarOpts -lLLVMInstCombine -lLLVMTransformUtils -lLLVMipa -lLLVMAnalysis -lLLVMTarget -lLLVMCore -lLLVMARMAsmParser -lLLVMMCParser -lLLVMARMDesc -lLLVMARMInfo -lLLVMARMAsmPrinter -lLLVMMC -lLLVMSupport
#-lLLVMJIT -lLLVMExecutionEngine -lLLVMCodeGen -lLLVMScalarOpts -lLLVMInstCombine -lLLVMTransformUtils -lLLVMipa -lLLVMAnalysis -lLLVMTarget -lLLVMMC -lLLVMCore -lLLVMSupport

# LLVMARMDisassembler
include $(CLEAR_VARS)
LOCAL_MODULE          := LLVMARMDisassembler
LOCAL_MODULE_FILENAME := lib$(LOCAL_MODULE)
LOCAL_SRC_FILES := ../llvm-3.5-arm/lib/$(LOCAL_MODULE_FILENAME).a
include $(PREBUILT_STATIC_LIBRARY)

# LLVMARMCodeGen
include $(CLEAR_VARS)
LOCAL_MODULE          := LLVMARMCodeGen
LOCAL_MODULE_FILENAME := lib$(LOCAL_MODULE)
LOCAL_SRC_FILES := ../llvm-3.5-arm/lib/$(LOCAL_MODULE_FILENAME).a
include $(PREBUILT_STATIC_LIBRARY)

# LLVMSelectionDAG
include $(CLEAR_VARS)
LOCAL_MODULE          := LLVMSelectionDAG
LOCAL_MODULE_FILENAME := lib$(LOCAL_MODULE)
LOCAL_SRC_FILES := ../llvm-3.5-arm/lib/$(LOCAL_MODULE_FILENAME).a
include $(PREBUILT_STATIC_LIBRARY)

# LLVMAsmPrinter
include $(CLEAR_VARS)
LOCAL_MODULE          := LLVMAsmPrinter
LOCAL_MODULE_FILENAME := lib$(LOCAL_MODULE)
LOCAL_SRC_FILES := ../llvm-3.5-arm/lib/$(LOCAL_MODULE_FILENAME).a
include $(PREBUILT_STATIC_LIBRARY)

# LLVMCodeGen
include $(CLEAR_VARS)
LOCAL_MODULE          := LLVMCodeGen
LOCAL_MODULE_FILENAME := lib$(LOCAL_MODULE)
LOCAL_SRC_FILES := ../llvm-3.5-arm/lib/$(LOCAL_MODULE_FILENAME).a
include $(PREBUILT_STATIC_LIBRARY)

# LLVMScalarOpts
include $(CLEAR_VARS)
LOCAL_MODULE          := LLVMScalarOpts
LOCAL_MODULE_FILENAME := lib$(LOCAL_MODULE)
LOCAL_SRC_FILES := ../llvm-3.5-arm/lib/$(LOCAL_MODULE_FILENAME).a
include $(PREBUILT_STATIC_LIBRARY)

# LLVMInstCombine
include $(CLEAR_VARS)
LOCAL_MODULE          := LLVMInstCombine
LOCAL_MODULE_FILENAME := lib$(LOCAL_MODULE)
LOCAL_SRC_FILES := ../llvm-3.5-arm/lib/$(LOCAL_MODULE_FILENAME).a
include $(PREBUILT_STATIC_LIBRARY)

# LLVMTransformUtils
include $(CLEAR_VARS)
LOCAL_MODULE          := LLVMTransformUtils
LOCAL_MODULE_FILENAME := lib$(LOCAL_MODULE)
LOCAL_SRC_FILES := ../llvm-3.5-arm/lib/$(LOCAL_MODULE_FILENAME).a
include $(PREBUILT_STATIC_LIBRARY)

# LLVMipa
include $(CLEAR_VARS)
LOCAL_MODULE          := LLVMipa
LOCAL_MODULE_FILENAME := lib$(LOCAL_MODULE)
LOCAL_SRC_FILES := ../llvm-3.5-arm/lib/$(LOCAL_MODULE_FILENAME).a
include $(PREBUILT_STATIC_LIBRARY)

# LLVMAnalysis
include $(CLEAR_VARS)
LOCAL_MODULE          := LLVMAnalysis
LOCAL_MODULE_FILENAME := lib$(LOCAL_MODULE)
LOCAL_SRC_FILES := ../llvm-3.5-arm/lib/$(LOCAL_MODULE_FILENAME).a
include $(PREBUILT_STATIC_LIBRARY)

# LLVMTarget
include $(CLEAR_VARS)
LOCAL_MODULE          := LLVMTarget
LOCAL_MODULE_FILENAME := lib$(LOCAL_MODULE)
LOCAL_SRC_FILES := ../llvm-3.5-arm/lib/$(LOCAL_MODULE_FILENAME).a
include $(PREBUILT_STATIC_LIBRARY)

# LLVMCore
include $(CLEAR_VARS)
LOCAL_MODULE          := LLVMCore
LOCAL_MODULE_FILENAME := lib$(LOCAL_MODULE)
LOCAL_SRC_FILES := ../llvm-3.5-arm/lib/$(LOCAL_MODULE_FILENAME).a
include $(PREBUILT_STATIC_LIBRARY)

# LLVMARMAsmParser
include $(CLEAR_VARS)
LOCAL_MODULE          := LLVMARMAsmParser
LOCAL_MODULE_FILENAME := lib$(LOCAL_MODULE)
LOCAL_SRC_FILES := ../llvm-3.5-arm/lib/$(LOCAL_MODULE_FILENAME).a
include $(PREBUILT_STATIC_LIBRARY)

# LLVMMCParser
include $(CLEAR_VARS)
LOCAL_MODULE          := LLVMMCParser
LOCAL_MODULE_FILENAME := lib$(LOCAL_MODULE)
LOCAL_SRC_FILES := ../llvm-3.5-arm/lib/$(LOCAL_MODULE_FILENAME).a
include $(PREBUILT_STATIC_LIBRARY)

# LLVMARMDesc
include $(CLEAR_VARS)
LOCAL_MODULE          := LLVMARMDesc
LOCAL_MODULE_FILENAME := lib$(LOCAL_MODULE)
LOCAL_SRC_FILES := ../llvm-3.5-arm/lib/$(LOCAL_MODULE_FILENAME).a
include $(PREBUILT_STATIC_LIBRARY)

# LLVMARMInfo
include $(CLEAR_VARS)
LOCAL_MODULE          := LLVMARMInfo
LOCAL_MODULE_FILENAME := lib$(LOCAL_MODULE)
LOCAL_SRC_FILES := ../llvm-3.5-arm/lib/$(LOCAL_MODULE_FILENAME).a
include $(PREBUILT_STATIC_LIBRARY)

# LLVMARMAsmPrinter
include $(CLEAR_VARS)
LOCAL_MODULE          := LLVMARMAsmPrinter
LOCAL_MODULE_FILENAME := lib$(LOCAL_MODULE)
LOCAL_SRC_FILES := ../llvm-3.5-arm/lib/$(LOCAL_MODULE_FILENAME).a
include $(PREBUILT_STATIC_LIBRARY)

# LLVMMC
include $(CLEAR_VARS)
LOCAL_MODULE          := LLVMMC
LOCAL_MODULE_FILENAME := lib$(LOCAL_MODULE)
LOCAL_SRC_FILES := ../llvm-3.5-arm/lib/$(LOCAL_MODULE_FILENAME).a
include $(PREBUILT_STATIC_LIBRARY)

# LLVMSupport
include $(CLEAR_VARS)
LOCAL_MODULE          := LLVMSupport
LOCAL_MODULE_FILENAME := lib$(LOCAL_MODULE)
LOCAL_SRC_FILES := ../llvm-3.5-arm/lib/$(LOCAL_MODULE_FILENAME).a
include $(PREBUILT_STATIC_LIBRARY)

# LLVMJIT
include $(CLEAR_VARS)
LOCAL_MODULE          := LLVMJIT
LOCAL_MODULE_FILENAME := lib$(LOCAL_MODULE)
LOCAL_SRC_FILES := ../llvm-3.5-arm/lib/$(LOCAL_MODULE_FILENAME).a
include $(PREBUILT_STATIC_LIBRARY)

# LLVMExecutionEngine
include $(CLEAR_VARS)
LOCAL_MODULE          := LLVMExecutionEngine
LOCAL_MODULE_FILENAME := lib$(LOCAL_MODULE)
LOCAL_SRC_FILES := ../llvm-3.5-arm/lib/$(LOCAL_MODULE_FILENAME).a
include $(PREBUILT_STATIC_LIBRARY)

# LLVMInterpreter
include $(CLEAR_VARS)
LOCAL_MODULE          := LLVMInterpreter
LOCAL_MODULE_FILENAME := lib$(LOCAL_MODULE)
LOCAL_SRC_FILES := ../llvm-3.5-arm/lib/$(LOCAL_MODULE_FILENAME).a
include $(PREBUILT_STATIC_LIBRARY)


include $(CLEAR_VARS)
LOCAL_MODULE := FastLimitSet

FILE_LIST := $(wildcard $(LOCAL_PATH)/lexer/*.cpp) \
             $(wildcard $(LOCAL_PATH)/parser/*.cpp) \
             $(wildcard $(LOCAL_PATH)/parser/nodes/*.cpp)

LOCAL_SRC_FILES := FastLimitSet.cpp \
                   $(FILE_LIST:$(LOCAL_PATH)/%=%)

LOCAL_C_INCLUDES += $(LOCAL_PATH)/../llvm-3.5-arm/include
LOCAL_CFLAGS += -D__STDC_CONSTANT_MACROS -D__STDC_LIMIT_MACROS -std=c++11 -fexceptions
LOCAL_LDLIBS := -L$(SYSROOT)/usr/lib -lz -latomic -llog

LOCAL_STATIC_LIBRARIES := LLVMJIT \
                          LLVMExecutionEngine \
                          LLVMInterpreter \
                          LLVMARMDisassembler \
                          LLVMARMCodeGen \
                          LLVMSelectionDAG \
                          LLVMAsmPrinter \
                          LLVMCodeGen \
                          LLVMScalarOpts \
                          LLVMInstCombine \
                          LLVMTransformUtils \
                          LLVMipa \
                          LLVMAnalysis \
                          LLVMTarget \
                          LLVMCore \
                          LLVMARMAsmParser \
                          LLVMMCParser \
                          LLVMARMDesc \
                          LLVMARMInfo \
                          LLVMARMAsmPrinter \
                          LLVMMC \
                          LLVMSupport
                          

include $(BUILD_SHARED_LIBRARY)
