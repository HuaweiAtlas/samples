TARGET := mbox-sample

export CC:=g++ -std=c++11

CFLAGS := -I../include -g -Wall -O2 -fPIC -DHAMBOX_TOOL
CXXFLAGS := $(CFLAGS)


RM-F := rm -f


OBJS := mbox.o mbox_sample.o
DEPS := $(patsubst %.o,%.d,$(OBJS))
MISSING_DEPS := $(filter-out $(wildcard $(DEPS)),$(DEPS))
MISSING_DEPS_SOURCES := $(wildcard $(patsubst %.d,%.c,$(MISSING_DEPS)) \
$(patsubst %.d,%.cc,$(MISSING_DEPS)))
CPPFLAGS += -MD

.PHONY : everything deps objs objclean clean rebuild

everything : $(TARGET)

deps : $(DEPS)

objs : $(OBJS)

objclean :
	@$(RM-F) *.o
	@$(RM-F) *.d

clean: objclean
	@$(RM-F) $(TARGET)

rebuild: veryclean everything

ifneq ($(MISSING_DEPS),)
$(MISSING_DEPS) :
	@$(RM-F) $(patsubst %.d,%.o,$@)
endif

-include $(DEPS)

$(TARGET) : $(OBJS)
	$(CC) -g  -fPIC -pthread -o $(TARGET) $(OBJS)
