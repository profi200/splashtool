.SUFFIXES:

# Sources and defines
TARGET   := $(notdir $(CURDIR))
BUILD    := build
INCLUDES := include lodepng tex3ds/include
SOURCES  := source lodepng tex3ds/source
DEFINES  := -DLODEPNG_NO_COMPILE_DISK -D_GNU_SOURCE


# Compiler settings
ARCH     :=
CFLAGS   := $(ARCH) -std=c11 -O2 -g -fstrict-aliasing -ffunction-sections \
			-Wall -Wextra -Wstrict-aliasing=3
CXXFLAGS := $(ARCH) -std=c++14 -O2 -g -fstrict-aliasing -ffunction-sections \
			-Wall -Wextra -Wstrict-aliasing=3
ASFLAGS  := $(ARCH) -O2 -g -x assembler-with-cpp
ARFLAGS  := -rcs
LDFLAGS  := $(ARCH) -O2 -s -static -ffunction-sections -Wl,--gc-sections

PREFIX   :=
CC       := $(PREFIX)gcc
CXX      := $(PREFIX)g++
AS       := $(PREFIX)gcc
AR       := $(PREFIX)gcc-ar


# Do not change anything after this
ifneq ($(BUILD),$(notdir $(CURDIR)))

export OUTPUT := $(CURDIR)/$(TARGET)
export VPATH  := $(foreach dir,$(DATA),$(CURDIR)/$(dir)) \
				 $(foreach dir,$(SOURCES),$(CURDIR)/$(dir))

CPPFILES := $(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.cpp)))
CFILES   := $(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.c)))
SFILES   := $(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.s)))

ifeq ($(strip $(CPPFILES)),)
	export LD := $(CC)
else
	export LD := $(CXX)
endif

export OFILES  := $(CPPFILES:.cpp=.o) $(CFILES:.c=.o) $(SFILES:.s=.o)

export INCLUDE := $(foreach dir,$(INCLUDES),-I$(CURDIR)/$(dir)) -I$(CURDIR)/$(BUILD)


.PHONY: $(BUILD) clean release

$(BUILD):
	@[ -d $@ ] || mkdir -p $@
	@$(MAKE) --no-print-directory -C $(BUILD) -f $(CURDIR)/Makefile

clean:
	@echo clean ...
	@rm -rf $(BUILD) $(TARGET)

release:
	@[ -d $(BUILD) ] || mkdir -p $(BUILD)
	@$(MAKE) --no-print-directory -C $(BUILD) -f $(CURDIR)/Makefile NO_DEBUG=1

else

ifneq ($(strip $(NO_DEBUG)),)
	DEFINES += -DNDEBUG
endif

VERS_STRING := $(shell git describe --tags --match v[0-9]* --abbrev=8 | sed 's/-[0-9]*-g/-/i')
VERS_MAJOR  := $(shell echo "$(VERS_STRING)" | sed 's/v\([0-9]*\)\..*/\1/i')
VERS_MINOR  := $(shell echo "$(VERS_STRING)" | sed 's/.*\.\([0-9]*\).*/\1/')

DEFINES += -DVER_STRING=\"$(VERSION_STR)\"
DEFINES += -DVER_MAJOR=$(shell echo "$(VERSION_STR)" | sed 's/v\([0-9]*\)\..*/\1/i')
DEFINES += -DVER_MINOR=$(shell echo "$(VERSION_STR)" | sed 's/.*\.\([0-9]*\).*/\1/')


# Main target
$(OUTPUT): $(OFILES)
	$(LD) $(LDFLAGS) $(OFILES) $(LIBPATHS) $(LIBS) -o $@
	@echo built ... $(notdir $@)


%.o: %.cpp
	@echo $(notdir $<)
	$(CXX) $(CXXFLAGS) $(DEFINES) $(INCLUDE) -c $< -o $@

%.o: %.c
	@echo $(notdir $<)
	$(CC) $(CFLAGS) $(DEFINES) $(INCLUDE) -c $< -o $@

%.o: %.s
	@echo $(notdir $<)
	$(AS) $(ASFLAGS) $(DEFINES) $(INCLUDE) -c $< -o $@

%.a:
	@echo $(notdir $@)
	$(AR) $(ARFLAGS) $@ $^

endif