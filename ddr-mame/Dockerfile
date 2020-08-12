FROM ubuntu:18.04

LABEL description="Build environment for DDR GroovyMAME"

ARG DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y \
    make \
    cmake \
    zip \
    g++ \
    libtool \
    g++-mingw-w64 \
    g++-mingw-w64-x86-64 \
    gcc-mingw-w64 \
    gcc-mingw-w64-base \
    gcc-mingw-w64-x86-64 \
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

WORKDIR /usr/x86_64-w64-mingw32/bin/
# No compiler override for this one and proper symlinks are absent from mingw bin dir
RUN ln -s ../../bin/x86_64-w64-mingw32-gcc-ar-posix x86_64-w64-mingw32-gcc-ar

RUN mkdir -p /ddr-mame
WORKDIR /ddr-mame

COPY /Common.mk /ddr-mame/Common.mk
COPY /Makefile /ddr-mame/Makefile
COPY /Package.mk /ddr-mame/Package.mk
COPY /ddrio /ddr-mame/ddrio
COPY /GroovyMAME /ddr-mame/GroovyMAME

WORKDIR /ddr-mame/GroovyMAME

RUN make SUBTRAGET=arcade MINGW64=/usr/x86_64-w64-mingw32 ARCHITECTURE=_x64 TARGETOS=windows CROSS_BUILD=1 OVERRIDE_CC=/usr/bin/x86_64-w64-mingw32-gcc-posix OVERRIDE_LD=/usr/bin/x86_64-w64-mingw32-ld OVERRIDE_CXX=/usr/bin/x86_64-w64-mingw32-g++-posix
#RUN make build-in-docker package