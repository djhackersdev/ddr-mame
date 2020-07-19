FROM ubuntu:18.04

LABEL description="Build environment for DDRIO implementations for ddr-groovymame"

RUN apt-get update && apt-get install -y \
    make \
    zip \
    gcc \
    git \
    libusb-1.0-0-dev

RUN mkdir -p /ddrio
WORKDIR /ddrio

COPY /Common.mk /ddrio/Common.mk
COPY /Makefile /ddrio/Makefile
COPY /src /ddrio/src

RUN make package