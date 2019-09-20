# 参与编译的源文件列表(要求子目录下不存在同名源文件)
objs = \
	../../output/mbox.o \
	
mbox_sample_objs = \
	../../output/mbox.o \
	../../output/mbox_sample.o

HAMBOX_OBJS :=  $(addprefix $(OBJDIR)/, $(objs))
HAMBOX_TEST_OBJS :=  $(addprefix $(OBJDIR)/, $(mbox_sample_objs))

