COMMON_DIR = common
SRC += $(COMMON_DIR)/matrix.c

VPATH += $(TOP_DIR)/keyboards/keychron/$(COMMON_DIR)

ifeq ($(strip $(DEBOUNCE_TYPE)), custom)
include $(TOP_DIR)/keyboards/keychron/$(COMMON_DIR)/debounce/debounce.mk
endif
