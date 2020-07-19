V ?= @

PROJECT_DIR := $(shell pwd)

BUILD_DIR := $(PROJECT_DIR)/build
BUILD_BIN_DIR := $(BUILD_DIR)/bin
BUILD_DOCKER_DIR := $(BUILD_DIR)/docker

DIST_DIR := $(PROJECT_DIR)/dist

DDRIO_DIR := $(PROJECT_DIR)/ddrio

GROOVYMAME_DIR := $(PROJECT_DIR)/GroovyMAME

BUILD_PACKAGE_DIR := $(BUILD_DIR)/package
BUILD_PACKAGE_LINUX_DIR := $(BUILD_PACKAGE_DIR)/linux
BUILD_PACKAGE_WIN_DIR := $(BUILD_PACKAGE_DIR)/win