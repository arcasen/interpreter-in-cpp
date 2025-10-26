# Makefile - do not edit!

CFLAGS += -std=c99

# Include project file
include $(wildcard *.pro)

# Search path for source and header files
VPATH = $(DEPENDPATH)
INCLUDE = $(addprefix -I,$(INCLUDEPATH))

# Objects
OBJECTS = $(subst .c,.o,$(notdir $(SOURCES)))

# Default: release build (optimized, no debug macro)
release: CFLAGS += -O2
release: $(TARGET)

# Debug build (with -g for symbols and -DDEBUG macro)
debug: CFLAGS += -g -DDEBUG
debug: $(TARGET)

# Build targets
$(TARGET): $(OBJECTS)
	$(LINK.c) -o $@ $^

%.o: %.c
	$(COMPILE.c) $(INCLUDE) -O2 -MMD -MP -MF .depends/$@.d -o $@ $<

# Clean targets
.PHONY: release debug clean
clean:
	rm -rf *.o .depends $(TARGET)

# Create dependencies directory
$(shell [ ! -e .depends ] && mkdir .depends)

# Enable dependency checking
DEPENDS = $(wildcard .depends/*.d)
ifneq ($(DEPENDS),)
include $(DEPENDS)
endif