SRCS = \
	   src/maxb.cpp \
	   src/platform_linux.cpp \
	   src/stdafx.cpp

CXX    ?= g++
RM      = @rm
MKDIR   = @mkdir -p
NAME    = maxb
OBJSDIR = obj
OBJS    = $(addprefix $(OBJSDIR)/,$(SRCS:.cpp=.o))
DEPS    = $(addprefix $(OBJSDIR)/,$(SRCS:.cpp=.d))
PCH     = $(OBJSDIR)/src/stdafx.h
GCH     = $(PCH).gch

override CXXFLAGS += -std=c++14

ifeq ($(DEBUG),1)
override CXXFLAGS += -g
else
override CXXFLAGS += -O3 -DNDEBUG
endif

override CXXFLAGS += -Iexternal/asio/include
override LDLIBS   += -lpthread

.SUFFIXES:
.SUFFIXES: .d .cpp .h .o

.PHONY: all
all: $(NAME)

.PHONY: clean
clean:
	$(RM) -rf $(NAME) $(OBJSDIR) *~

-include $(DEPS)

$(OBJS): $(GCH)

$(GCH): src/stdafx.h
	$(MKDIR) $(dir $@)
	$(CXX) $(CXXFLAGS) -x c++-header -c -o $@ $^

$(NAME): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDLIBS)

$(OBJSDIR)/%.o: %.cpp
	$(MKDIR) $(dir $@)
	$(CXX) $(CXXFLAGS) -MMD -include $(PCH) -c -o $@ $<
