MMIO_TEST_VERSION = 1.2
MMIO_TEST_SITE = $(BR2_EXTERNAL_HARBOR_PATH)/package/mmio-test/src
MMIO_TEST_SITE_METHOD = local
MMIO_TEST_LICENSE = MIT

define MMIO_TEST_BUILD_CMDS
	$(TARGET_CC) $(TARGET_CFLAGS) $(TARGET_LDFLAGS) \
		-I$(BR2_EXTERNAL_HARBOR_PATH)/../../../../include \
		-Wall -Wextra -Wpedantic \
		-o $(@D)/mmio-test $(@D)/mmio-test.c
endef

define MMIO_TEST_INSTALL_TARGET_CMDS
	$(INSTALL) -D -m 0755 $(@D)/mmio-test $(TARGET_DIR)/usr/bin/mmio-test
	$(INSTALL) -D -m 0755 $(BR2_EXTERNAL_HARBOR_PATH)/package/mmio-test/S90mmio-test \
		$(TARGET_DIR)/etc/init.d/S90mmio-test
endef

$(eval $(generic-package))
