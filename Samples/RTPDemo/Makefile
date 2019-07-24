TARGET  := rtpDemo
TOP_DIR = $(PWD)

CROSS	:= $(cross)

#compile and lib parameter
CC      := $(CROSS)gcc
CXX	:= $(CROSS)g++

LIBS    := -lpthread

LDFLAGS :=
DEFINES :=
INCLUDE := -I$(TOP_DIR)/include

CFLAGS  := -g -Wall -O2 -fstack-protector-all -Wl,-z,relro,-z,now,-z,noexecstack -std=c++11 -Wno-deprecated-declarations $(DEFINES) $(INCLUDE)
CXXFLAGS:= $(CFLAGS)
SHARE   := -fPIC -o

#源文件，自动找所有.c和.cpp文件，并将目标定义为同名.o文件
SRCEXTS := .cpp .cc
DIRS    := $(shell find $(TOP_DIR) -maxdepth 2 -type d)
SOURCE  := $(foreach dir,$(DIRS),$(wildcard $(addprefix $(dir)/*, $(SRCEXTS))))
OBJS    := $(sort $(SOURCE))
 
 .PHONY :  
  
everything : $(TARGET)
  
all : $(TARGET)  
objs : $(OBJS)

clean :
	rm -fr *.o 
	rm -fr $(TARGET)

$(TARGET) : $(OBJS)
	$(CXX) $(CXXFLAGS) $(SHARE) $@ $(OBJS) $(LDFLAGS) $(LIBS)

