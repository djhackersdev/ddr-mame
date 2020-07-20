include Common.mk

all: \
usage

.PHONY: \
build \
clean \
package

usage:
	$(V)echo "DDRIO implementations for MAME build makefile"
	$(V)echo "  build: Build the project with all available DDRIO implementations"
	$(V)echo "  build-docker: Build the project in a docker container"
	$(V)echo "  clean: Clean up all build output"
	$(V)echo "  package: Package the build output into a distribution zip file"

build: $(BUILD_DIR) $(BUILD_BIN_DIR) $(BIN_DDRIO_DEBUG) $(BIN_DDRIO_PIUIO_ITG)

clean:
	$(V)rm -rf $(BUILD_DIR)
	$(V)rm -rf $(BUILD_BIN_DIR)
	$(V)rm -rf $(BUILD_DOCKER_DIR)

package: $(BUILD_DIR) $(BUILD_BIN_DIR) $(DIST_PACKAGE_DDRIO)

build-docker:
	$(V)docker rm -f $(DOCKER_CONTAINER_NAME) 2> /dev/null || true
	$(V)rm -rf $(BUILD_DOCKER_DIR)
	$(V)mkdir -p $(BUILD_DIR)
	$(V)docker build -t $(DOCKER_IMAGE_NAME) .
	$(V)docker create --name $(DOCKER_CONTAINER_NAME) $(DOCKER_IMAGE_NAME)
	$(V)docker cp $(DOCKER_CONTAINER_NAME):/ddrio/build $(BUILD_DIR)
	$(V)mv $(BUILD_DIR)/build $(BUILD_DIR)/docker

$(BUILD_DIR):
	$(V)mkdir -p $@

$(BUILD_BIN_DIR):
	$(V)mkdir -p $@

$(DIST_PACKAGE_DDRIO): \
		$(BIN_DDRIO_DEBUG) \
		$(BIN_DDRIO_PIUIO_ITG) \
		| $(BUILD_DIR)/
	$(V)echo ... $@
	$(V)zip -j $@ $^

$(BIN_DDRIO_DEBUG): $(SOURCE_DDRIO_DEBUG)
	$(CC) -shared -Wl,-soname,$@ -o $@ -fPIC -I"./src" $^

$(BIN_DDRIO_PIUIO_ITG): $(SOURCE_DDRIO_PIUIO_ITG)
	$(CC) -shared -Wl,-soname,$@ -o $@ -fPIC -I"./src" -lusb-1.0 $^