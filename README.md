# DDR System 573 focused build of GroovyMAME for dedicated arcade setups
This project provides a fork of [GroovyMAME](https://github.com/antonioginer/GroovyMAME) developed
for running on dedicated 4 panel arcade setups such DDR, ITG or SMX machines, or custom setups that
want to mimic the arcade expierence as good as possible.

## Why?
The emulation support for Konami's System 573 (Digital) hardware has reached important milestones
making it a viable solution to play the old DDR games on standard PC hardware. The original hardware
is still around, but is showing its age, it gets more difficult to acquire and is not suited for
cabinet types that do not have the old style DDR cabinet wiring.

The goal of this project is to provide a software solution and infrastructure to develop and
maintain it for the purpose of running it on different hardware types with standard PC hardware.

## Features
* Based on [GroovyMAME](https://github.com/antonioginer/GroovyMAME) 0.222 stable and therefore
includes all features GroovyMAME provides, e.g. 15 kHz monitor support
* Linux only, currently (Windows support planned)
* Stripped down version GroovyMAME focusing on the 
[ksys573 driver](ddr-mame/GroovyMame/src/mame/drivers/ksys573.cpp). This reduces build time and
binary size.
* Building in docker containers to ensure builds with consistent environment
* Distribution packages to easily deploy this to a target machine
* Pre-defined configuration file and startup scripts handling setup for target hardware
* DDRIO interface for supporting any type of IO board, e.g. piuio-itg, minimaid...
* Native support of DDRIO interface in 
[ksys573 driver](ddr-mame/GroovyMame/src/mame/drivers/ksys573.cpp)

## Hardware requirements
Anything that runs system 573 emulation well, I guess? I tested this on some 7 year old low spec
Intel Core i3 and had no performance issues.

## Operating system requirements
Currently, Linux only. Ubuntu 16.04 and 18.04 work fine and are tested to be compatible runtime
environments with the platform independent builds created by the docker build containers.

You can use [pumpos](https://dev.s-ul.eu/hackitup/pumpos) as a base and deploy the distribution
packages to an empty and bootable installation.

A solution ~~copy-pasted from pumpos~~ similar to pumpos will be worked on when the higher priority
issues of this project are resolved.

## ddr-mame
Refer to the [readme](ddr-mame/README.md).

## DDRIO
Refer to the [readme](ddrio/README.md).

## Issue reporting
Use the templates when opening issues or merge requests. They save time, avoid a lot of back and
forth, and assist you in providing fundamental information we are always asking for.

## License
Source code license is the Unlicense; you are permitted to do with this as thou wilt. For details,
please refer to the [LICENSE file](LICENSE) included with the source code.