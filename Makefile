CXX    ?= g++
RM      = @rm
MKDIR   = @mkdir -p
NAME    = maxb
SRCS    = src/maxb.cpp src/stdafx.cpp
OBJSDIR = obj
OBJS    = $(addprefix $(OBJSDIR)/,$(SRCS:.cpp=.o))
DEPS    = $(addprefix $(OBJSDIR)/,$(SRCS:.cpp=.d))
PCH     = $(OBJSDIR)/stdafx.h
GCH     = $(PCH).gch

override CXXFLAGS += -std=c++11

ifeq ($(DEBUG),1)
override CXXFLAGS += -g
else
override CXXFLAGS += -O3 -DNDEBUG
endif

override CXXFLAGS += -Iexternal/asio/include

.SUFFIXES:
.SUFFIXES: .d .cpp .h .o

.PHONY: all
all: $(NAME)

.PHONY: clean
clean:
	$(RM) -rf $(NAME) $(OBJSDIR) *~

-include $(DEPS)

$(OBJS): $(GCH)

$(GCH): stdafx.h
	$(MKDIR) $(dir $@)
	$(CXX) -c $(CXXFLAGS) -x c++-header -o $@ $^

$(NAME): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

$(OBJSDIR)/%.o: %.cpp
	$(MKDIR) $(dir $@)
	$(CXX) $(CXXFLAGS) -MMD -include $(PCH) -c -o $@ $<
