# DDRIO
DDRIO provides an interface allowing different implementations to be used with
[ddr-mame](../ddr-mame/README.md).

The following implementations are currently available:
* `debug`: Debug implementation for testing and developing
* `piuio-itg`: Use the PIUIO USB hardware of an In The Groove dedicated cabinet

## Building
The project can be built on your current host:
```shell
make build
```

To create a distribution package for deployment to a target setup, you can follow-up with packaging
your build:
```shell
make package
```

The build output will be located in a separate `build` folder.

To ensure a consistent build and runtime environment, run the build in a docker container:
```shell
make build-docker
```

The output from the build inside the docker container will be located in the `build/docker` folder.

## Creating DDRIO implementations
The [ddrio.h header file](src/ddrio/ddrio.h) is the interface the you have to implement. Create a
new sub-folder next to the existing implementations and a `ddrio.c` module that implements the
interface. Checkout the interface documentation and existing examples for reference. To include
your implementation in the build process, tweak the [Makefile](Makefile) to include it accordingly.