MEM_MODULE_VERSION = 1.0
MEM_MODULE_SITE = $(BR2_EXTERNAL_MEM_MOD_PATH)/package/mem_module
MEM_MODULE_SITE_METHOD = local

define KERNEL_MODULE_BUILD_CMDS
        $(MAKE) -C '$(@D)' LINUX_DIR='$(LINUX_DIR)' CC='$(TARGET_CC)' LD='$(TARGET_LD)' modules
endef

$(eval $(kernel-module))
$(eval $(generic-package))
