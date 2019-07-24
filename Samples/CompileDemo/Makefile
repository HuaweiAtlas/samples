TARGET  := compile_demo_atlas500
#HOST COMPILE
CUR_DIR=$(shell pwd)

# Atlas 500 ddk path
DDK_PATH := $(ddk_path)

# Atlas 500 Compiler Path
COMPILE_PATH := $(compile_path)

CROSS   := $(COMPILE_PATH)/arm/cross_compile/install/bin/aarch64-linux-gnu-

#compile and lib parameter
CC      := $(CROSS)gcc
AR      := $(CROSS)gcc-ar
RANLIB  := $(CROSS)gcc-ranlib
CXX := $(CROSS)g++

DDK_INCLUDE_DIR := ${DDK_PATH}/include
THIRD_PARTY_DIR := ${DDK_INCLUDE_DIR}/third_party

LIBS    := -L$(DDK_PATH)/lib64 -lc_sec -lprotobuf -ldrvhdc_host -ldrvdevdrv -lprofilerclient -lslog -lmatrix -lmmpa -lmemory\
           -L$(CUR_DIR) -lpthread

LDFLAGS :=
DEFINES :=
INCLUDE :=  -I$(DDK_INCLUDE_DIR) \
            -I$(DDK_INCLUDE_DIR)/inc \
            -I$(DDK_INCLUDE_DIR)/libc_sec/include \
            -I$(THIRD_PARTY_DIR) \
            -I$(THIRD_PARTY_DIR)/protobuf/include \
            -I$(THIRD_PARTY_DIR)/cereal/include \
            -I$(CUR_DIR)

CFLAGS  := -g -Wall -O2 -std=c++11 -Wno-deprecated-declarations $(DEFINES) $(INCLUDE)
CXXFLAGS:= $(CFLAGS)
  
#source file
SOURCE  := $(wildcard $(CUR_DIR)/main.cpp) 
OBJS    := $(SOURCE)
 
.PHONY : 
  
everything : $(TARGET)
all : $(TARGET)
objs : $(OBJS)

clean :
	rm -fr *.o 
	rm -fr $(TARGET)

$(TARGET) : $(OBJS)
	$(CXX) $(CXXFLAGS) -o  $@ $(OBJS) $(LDFLAGS) $(LIBS)
	cp -f $(TARGET) $(CUR_DIR)/out/
