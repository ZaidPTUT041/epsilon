HANDY_TARGETS += bootloader

# stack protector
SFLAGS += -fstack-protector-strong

bootloader_src = $(ion_device_bootloader_src) $(liba_internal_flash_src) $(kandinsky_minimal_src)
bootloader_obj = $(call flavored_object_for,$(bootloader_src),leavebootloader $(MODEL))
$(BUILD_DIR)/bootloader.$(EXE): $(bootloader_obj)

$(BUILD_DIR)/bootloader.$(EXE): LDFLAGS += -Lion/src/$(PLATFORM)/shared/flash -Lion/src/$(PLATFORM)/bootloader/
$(BUILD_DIR)/bootloader.$(EXE): LDSCRIPT = ion/src/$(PLATFORM)/epsilon-bootloader/bootloader/bootloader_flash.ld
