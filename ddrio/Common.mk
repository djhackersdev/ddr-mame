V ?= @

PROJECT_DIR := $(shell pwd)

BUILD_DIR ?= $(PROJECT_DIR)/build
BUILD_BIN_DIR ?= $(BUILD_DIR)/bin
BUILD_DOCKER_DIR ?= $(BUILD_DIR)/docker

CC ?= gcc

gitrev := $(shell git rev-parse HEAD)
libcver := $(shell ldd --version | head -n 1)

DIST_PACKAGE_DDRIO := $(BUILD_DIR)/ddrio.zip

BIN_DDRIO_DEBUG := $(BUILD_BIN_DIR)/ddrio-debug.so
BIN_DDRIO_PIUIO_ITG := $(BUILD_BIN_DIR)/ddrio-piuio-itg.so

SOURCE_DDRIO_DEBUG := \
	src/debug/ddrio.c

SOURCE_DDRIO_PIUIO_ITG := \
	src/piuio/device/device.c \
    src/piuio/device/libusb-1.0.c \
    src/piuio/itg/piuio-itg.c \
    src/piuio/ddrio.c

DOCKER_CONTAINER_NAME := "ddrio-groovymame-build"
DOCKER_IMAGE_NAME := "dddrio-groovymame:build"