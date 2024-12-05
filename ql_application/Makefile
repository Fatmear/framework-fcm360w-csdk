.PHONY:all

include $(CURDIR)/../Make.defs


###########################################
#
#			directory path
#
###########################################
SUBDIRS := $(shell find $(CURDIR) -type d )
SUBDIRS	:= $(filter-out $(CURDIR),$(SUBDIRS))

OUTPUTDIR_OBJS:=$(OUTPUTDIR)/objs/ql_application
OUTPUTDIR_DEPS:=$(OUTPUTDIR)/deps/ql_application
OUTPUTDIR_LIB:=$(OUTPUTDIR)/libs


###########################################
#
#	C source and header files 	
#
###########################################
ASRCS	:=
CSRCS	:=
INCPATHS:=
MAKEDEFS:=

$(foreach dir, $(SUBDIRS), $(eval $(call MAKEDEFS_template,$(dir))))

MAKEDEFS += ./Make.defs

include ./Make.defs

CFLAGS := $(foreach incpath, $(INCPATHS), -I "$(incpath)") $(CFLAGS)

###########################################
#
#		target
#
###########################################
LIB_APPLICATION = libql_app.a
ASRCS	:= $(wildcard $(foreach SRC, $(ASRCS), $(addsuffix /$(SRC), $(subst :, ,$(VPATH)))))
CSRCS	:= $(wildcard $(foreach SRC, $(CSRCS), $(addsuffix /$(SRC), $(subst :, ,$(VPATH)))))

ifneq ($(PIO_PROJECT_DIR),)
    CSRCS += $(shell find $(PIO_PROJECT_DIR)/src -type f -name "*.c" | sed "s|^$(PIO_PROJECT_DIR)/||")
	CSRCS += $(shell find $(PIO_PROJECT_DIR)/test -type f -name "*.c" | sed "s|^$(PIO_PROJECT_DIR)/||")
endif

AOBJS = $(addprefix $(OUTPUTDIR_OBJS)/,$(ASRCS:.S=.o))
COBJS = $(addprefix $(OUTPUTDIR_OBJS)/,$(CSRCS:.c=.o))
OBJS = $(AOBJS) $(COBJS)
HDEPS:=$(OBJS:.o=.d)
DEPS = $(addprefix $(OUTPUTDIR_DEPS)/,$(CSRCS:.c=.deps))
all: $(OUTPUTDIR_LIB)/$(LIB_APPLICATION)

depend: $(DEPS)
-include $(HDEPS)
###########################################
#
#		generate lib
#
###########################################

$(OUTPUTDIR_LIB)/$(LIB_APPLICATION): $(OBJS)
	$(Q) -mkdir -p $(dir $@)
	$(call ARCHIVE, $@, $(OBJS))


###########################################
#
#		generate objs
#
###########################################
$(AOBJS): $(OUTPUTDIR_OBJS)/%.o: %.S
	$(Q) -mkdir -p $(dir $@)
	$(call ASSEMBLE, $<, $@)

$(COBJS): $(OUTPUTDIR_OBJS)/%.o: %.c
	$(Q) -mkdir -p $(dir $@)
	$(call COMPILE, $<, $@)

###########################################
#
#		generate deps
#
###########################################
$(DEPS):$(OUTPUTDIR_DEPS)/%.deps: %.c
	$(Q) -mkdir -p $(dir $@)
	$(call DEPEND, $<, $@)



