FROM ubuntu:18.04

LABEL description="Build environment for DDR GroovyMAME"

RUN apt-get update && apt-get install -y \
    make \
    cmake \
    zip \
    g++ \
    libtool \
    mingw-w64-i686-dev \
    g++-mingw-w64-i686 \
    libdw-dev \
    libelf-dev \
    python \
    pkgconf \
    libsdl2-dev \
    libfontconfig1-dev \
    libqt5widgets5 \
    qtbase5-dev \
    libsdl2-ttf-dev \
    qt5-default

RUN mkdir -p /ddr-mame
WORKDIR /ddr-mame

COPY /Common.mk /ddr-mame/Common.mk
COPY /Makefile /ddr-mame/Makefile
COPY /Package.mk /ddr-mame/Package.mk
COPY /ddrio /ddr-mame/ddrio
COPY /dist /ddr-mame/dist
COPY /GroovyMAME /ddr-mame/GroovyMAME

RUN make build-in-docker