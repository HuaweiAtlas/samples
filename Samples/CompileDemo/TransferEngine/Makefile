TARGET := libTransferEngine.so
#HOST COMPILER
DDK_PATH = $(ddk_path)
SRC_FILE := $(PWD)/
OUT_DIR := $(SRC_FILE)../out

# Atlas 500 Compiler Path
COMPILE_PATH := $(compile_path)

CROSS   := $(COMPILE_PATH)/arm/cross_compile/install/bin/aarch64-linux-gnu-

#compile and lib parameter
CC      := $(CROSS)gcc
AR      := $(CROSS)gcc-ar
RANLIB  := $(CROSS)gcc-ranlib
CXX     := $(CROSS)g++

LIBS    := -L$(DDK_PATH)/lib64 \
            -lmmpa \
            -lc_sec -lslog\
            -lmatrix -lprofilerclient\
            -lrt -ldl 
LDFLAGS :=
DEFINES :=
INCLUDE := -I$(SRC_FILE) \
           -I$(DDK_PATH)/include/inc \
           -I$(DDK_PATH)/include/third_party/protobuf/include \
           -I$(DDK_PATH)/include/third_party/cereal/include \
           -I$(DDK_PATH)/include/libc_sec/include \
            
CFLAGS  := -g -Wall -O3 -std=c++11 -fPIC $(DEFINES) $(INCLUDE)
CXXFLAGS:= $(CFLAGS)
SHARE   := -shared -o

  
#source file
SOURCE  := $(SRC_FILE)/*.cpp
OBJS    := $(SOURCE)

.PHONY : 

everything : $(TARGET)
  
all : $(TARGET)
  
objs : $(OBJS)
                
clean :
	rm -fr *.o
	rm -fr $(TARGET)
    
$(TARGET) : $(OBJS)
	$(CXX) $(CXXFLAGS) $(SHARE) $@ $(OBJS) $(LDFLAGS) $(LIBS)
	cp -f $(TARGET) $(OUT_DIR)
