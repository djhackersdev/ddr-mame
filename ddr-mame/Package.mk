#
# Packiging build rules
#

include Common.mk

PACKAGE_LINUX_ZIP := "ddr-mame-linux.zip"

$(BUILD_PACKAGE_LINUX_DIR):
	$(V)mkdir -p $@
	$(V)cp -r $(GROOVYMAME_DIR)/artwork $@/artwork
	$(V)cp -r $(GROOVYMAME_DIR)/hash $@/hash
	$(V)cp -r $(GROOVYMAME_DIR)/language $@/language
	$(V)mkdir $@/roms
	$(V)mkdir $@/save
	$(V)cp $(GROOVYMAME_DIR)/mamearcade64 $@/mamearcade64

$(BUILD_PACKAGE_DIR)/$(PACKAGE_LINUX_ZIP): $(BUILD_PACKAGE_LINUX_DIR)
	$(V)echo ... $@
	$(V)cd $^ && zip -r $@ .

package: package-linux

package-linux: \
		$(BUILD_PACKAGE_LINUX_DIR) \
		$(BUILD_PACKAGE_DIR)/$(PACKAGE_LINUX_ZIP)

clean-package: \
		clean-package-linux

clean-package-linux:
	$(V)rm -rf $(BUILD_PACKAGE_LINUX_DIR)
	$(V)rm -rf $(BUILD_PACKAGE_DIR)/$(PACKAGE_LINUX_ZIP)