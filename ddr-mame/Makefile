include Common.mk

DOCKER_CONTAINER_NAME := "ddr-mame-build"
DOCKER_IMAGE_NAME := "ddr-mame:build"

all: \
usage

.PHONY: \
usage \
build \
build-in-docker \
clean \
build-docker \
import-ddrio

# TODO update usage
usage:
	$(V)echo "ddr-mame project makefile"
	$(V)echo "  build: Build GroovyMAME"
	$(V)echo "  clean: Cleanup the build environment and any build output"
	$(V)echo "  build-docker: Build in a docker container"


build: import-ddrio-header
	$(V)mkdir -p $(BUILD_BIN_DIR)
	$(V)cd $(GROOVYMAME_DIR) && make SUBTARGET=arcade -j $(grep -c ^processor /proc/cpuinfo)
	$(V)cp $(GROOVYMAME_DIR)/mamearcade64 $(BUILD_BIN_DIR)/mamearcade64

# Used by docker container to build. No import-ddrio-header dependency, that is already taken care
# of by build-docker. The docker context does not contain the ddrio folder from the root of the
# whole project
build-in-docker:
	$(V)mkdir -p $(BUILD_BIN_DIR)
	$(V)cd $(GROOVYMAME_DIR) && make SUBTARGET=arcade -j $(grep -c ^processor /proc/cpuinfo)
	$(V)cp $(GROOVYMAME_DIR)/mamearcade64 $(BUILD_BIN_DIR)/mamearcade64

clean:
	$(V)cd $(GROOVYMAME_DIR) && make clean
	$(V)rm -rf $(BUILD_DIR)
	$(V)rm -rf $(DDRIO_DIR)

# Clean the local build environment before copying to avoid polluting the docker environment.
# Unfortunately, this cannot be done after copying because 3rdparty/genie has dependencies to the
# build environment which makes calling make clean fail in the docker container (?!)...
build-docker: import-ddrio-header
	$(V)cd $(GROOVYMAME_DIR) && make clean
	$(V)docker rm -f $(DOCKER_CONTAINER_NAME) 2> /dev/null || true
	$(V)rm -rf $(BUILD_DOCKER_DIR)
	$(V)mkdir -p $(BUILD_DOCKER_DIR)
	$(V)docker build -t $(DOCKER_IMAGE_NAME) .
	$(V)docker create --name $(DOCKER_CONTAINER_NAME) $(DOCKER_IMAGE_NAME)
	$(V)docker cp $(DOCKER_CONTAINER_NAME):/ddr-mame/build $(BUILD_DIR)
	$(V)mv $(BUILD_DIR)/build $(BUILD_DOCKER_DIR)

import-ddrio-header:
	$(V)rm -rf $(DDRIO_DIR)
	$(V)mkdir -p $(DDRIO_DIR)
	$(V)cp $(PROJECT_DIR)/../ddrio/src/ddrio/ddrio.h $(DDRIO_DIR)/ddrio.h

include Package.mk