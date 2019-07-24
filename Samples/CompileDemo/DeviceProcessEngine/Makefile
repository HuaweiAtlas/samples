TARGET := libDeviceProcessEngine.so
#DEVICE COMPILER      
DDK_PATH = $(ddk_path)
SRC_FILE := $(PWD)/
OUT_DIR := $(SRC_FILE)../out

COMPILE_PATH := $(compile_path)

CROSS   := $(DDK_PATH)/toolchains/aarch64-linux-gcc6.3/bin/aarch64-linux-gnu-

#compile and lib parameter
CC      := $(CROSS)gcc
AR      := $(CROSS)ar
RANLIB  := $(CROSS)ranlib
CXX     := $(CROSS)g++

LIBS    := -L$(DDK_PATH)/device/lib/ \
            -lmmpa \
            -lc_sec -lslog\
            -lmatrixdaemon -lprofilerclient\
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
