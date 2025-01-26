SIMICS_SHM_VERSION = 1.0
SIMICS_SHM_SITE = $(BR2_EXTERNAL_SIMICS_SHM_PATH)/package/simics_shm
SIMICS_SHM_SITE_METHOD = local

define KERNEL_MODULE_BUILD_CMDS
        $(MAKE) -C '$(@D)' LINUX_DIR='$(LINUX_DIR)' CC='$(TARGET_CC)' LD='$(TARGET_LD)' modules
endef

$(eval $(kernel-module))
$(eval $(generic-package))
