# ��������Դ�ļ��б�(Ҫ����Ŀ¼�²�����ͬ��Դ�ļ�)
objs = \
	../../output/mbox.o \
	
mbox_sample_objs = \
	../../output/mbox.o \
	../../output/mbox_sample.o

HAMBOX_OBJS :=  $(addprefix $(OBJDIR)/, $(objs))
HAMBOX_TEST_OBJS :=  $(addprefix $(OBJDIR)/, $(mbox_sample_objs))

