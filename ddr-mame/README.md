# DDR MAME
This is a fork based on GroovyMAME 0.222 stable. However, the project has been stripped down to
minimize the size and reduce build times. It focuses on and supports the
[ksys573 driver](GroovyMame/src/mame/drivers/ksys573.cpp), only.

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

## Running
The sub-folder `dist` contains various scripts and configuration files that allow you to handle
various bootstrap tasks required to run MAME on a headless setup.

The scripts were developed for running on a ITG dedicated cabinet with a 31kHz 640x480 resolution
monitor. If you have a different monitor, you will likely have to tweak some properties. If you do
so, extending them and keeping things flexible is the way to go and your contribution to extend
support for different cabinet/setup types is highly appreciated.

On a headless setup, unpack the distribution package to a folder of your choice. This should deploy
all the files from `dist`, the `mamearcade64` executable and a bunch of folders MAME needs.

Make sure to also deploy the ROM and CHD images of the games you want to deploy and put them into
the `roms` folder.

Run the following command to run MAME with default settings (in 640x480 resolution mode):
```shell
./cabinet-startx.sh
```

This will launch MAME and drop you in the main menu where you can select a game.

There are bunch of environment variables to tweak the startup behavior of mame.

This let's you choose a different resolution, if supported (currently 480 only):
```shell
DDR_MAME_RES_VERT=480 ./cabinet-startx.sh
```

The following command launches DDR Extreme (rom name `ddrextrm`) directly:
```shell
DDR_MAME_GAME=ddrextrm ./cabinet-startx.sh
```

If you are experiencing issues, you can easily enable verbose log output and make it pipe to a
`mame.log` log file in the same directory:
```shell
DDR_MAME_MODE=debug ./cabinet-startx.sh
```

## Features and changes
* Remove emulator warning and game info OSD screens appearing every time you start a game
* Native support for IO hardware implementation the DDRIO interface in the 
[ksys573.cpp module](GroovyMAME/src/mame/drivers/ksys573.cpp)
* Offset fix: Adjust audio offset to compensate audio lag (credit to PlasmaVenom)
* Add DDR Extreme Pro and DDR Extreme Pro Clarity to the list of games (credit to PlasmaVenom)

### Offset fix
Compensate audio lag by adjusting a global offset that shifts audio playback to allow you
synchronizing the arrows to the music.

This is an option you have to play around with. Use an easy song to test the values you set and
tweak them accoring to how the timing feels (early/late).

Access it by pressing the `Tab` key when a game is booted, go to `Slider Controls` and go to the
bottom item `OFFSET FIX`.

### Using DDRIO implementations
Depending on your hardware, check if there are implementations of the DDRIO interface available.

To use one, rename the file to `ddrio.so` and place it next to the `mamearcade64` executable. MAME
will pick it up automatically. If something doesn't work, check stdout for any errors.

For further troubleshooting and information, refer to the readme files of each DDRIO implementation.

## General notes about GroovyMAME
* When running with a default `mame.ini`, GroovyMAME goes into 15 kHz mode by default which doesn't
work on desktop machines with your todays standard monitors connected. You can use the following
command to run it for a quick test: `./mamearcade64 -monitor auto -switchres -prescale 0 -window -resolution 1920x1080@60 -inipath .`. Replace the resolution with a lower one if necessary.

## DDR roms
There is a package that includes all the CHD images and ROM files of the DDR games that are
supported by the 573 hardware available in the bemani pig stall.

A complete list of supported games can be found at the end of the
[ksys573.cpp module](GroovyMAME/src/mame/drivers/ksys573.cpp).

### Installing the games
Some games, e.g. pre 5th mix, have an installation process that you have to follow, otherwise you
will just see the game booting into a checksum error screen.

1. Start the game, it will show a checksum error and fails to boot.
1. Hit the `Tab` key on your keyboard and go to `DIP Switches` and flip DIP switch 4 from the on position `Flash ROM` to the off position `CD-ROM Drive`.
1. Change the security cassette by going to the `Slot Devices` sub-menu and changing the `cassette`
from `game` to `install`.
1. Reset the machine, e.g. press the `F3` key on your keyboard.
1. The game might also update the CD-ROM drive before installing the game. You might get prompted
with a `NOTE 604` that informs you about the procedure and asks you to press the test button.
1. Let the game install. This will be permanent and will be stored in the `nvram` directory defined
in the `mame.ini` config file.
1. Once installation completes, the game prompts you with a `INFO 506` message to instert the game
disc and security cassette. Do so by repeating the steps above switching DIP switch 4 back to `Flash ROM` and the security cassette to `game`.
1. Reset the machine again.
1. Depending on the game, you might get another prompt that tells you to clear the RTC by pressing
the test button.

## mame.ini notes
All "code" blocks are excerpts from `mame.ini` showing only the relevant options that are being
discussed per section.

Descriptions for each property from the `mame.ini` file can be found at the top of the
[emuopts.cpp file](GroovyMAME/src/emu/emuopts.cpp).

### Audio latency
```text
#
# OSD SOUND OPTIONS
#
audio_latency 0
```

The `audio_latency` option definitely impacts audio latency for the games ([reference](https://github.com/mamedev/mame/pull/5344/files)). Set it to `0`.

### Core performance
```text
#
# CORE PERFORMANCE OPTIONS
#
autoframeskip 0
frameskip 0
seconds_to_run 0
throttle 1
syncrefresh 1
autosync 0
sleep 0
speed 1.0
refreshspeed 0
lowlatency 1
```

Disable `autoframeskip` and `frameskip` as any kind of framskipping introduces hick-ups during
gameplay, e.g. arrows not scrolling smooth.

`throttle` needs to be enabled to bind the emulation target performance to 100%. Otherwise, the
game is rendered too fast and audio also plays too fast.

However, we don't want `sleep` to be active to introduce performance issues.

Enable `lowlatency` to draw frames before throttling to reduce input latency and `syncrefresh` to
throttle on VBLANK. This should improve general accuracy. 

### Monitor and switchres options
These are not well documented and are not included when a default `mame.ini` is generated.

```text
#
# CORE SWITCHRES OPTIONS
#
modeline_generation       1
monitor                   arcade_31
orientation               horizontal
connector                 auto
interlace                 1
doublescan                1
super_width               2560
changeres                 0
powerstrip                0
lock_system_modes         0
lock_unsupported_modes    0
refresh_dont_care         0
dotclock_min              0
sync_refresh_tolerance    2.0
frame_delay               0
vsync_offset              0
black_frame_insertion     0
modeline                  auto
ps_timing                 auto
lcd_range                 auto
crt_range0                auto
crt_range1                auto
crt_range2                auto
crt_range3                auto
crt_range4                auto
crt_range5                auto
crt_range6                auto
crt_range7                auto
crt_range8                auto
crt_range9                auto
```

The excerpt above shows a configuration for a 31 kHz 640x480 CRT monitor, e.g. In The Groove
dedicated cabinet.

Depending on your monitor, `changeres` should be off, otherwise MAME switches to the native
resolution of the game. 

If you have a different type of monitor, e.g. a tri-sync or 15 kHz only monitor, there are different
pre-sets that allow `switchres` to operate according to the modelines defined in the
[monitor.cpp module](GroovyMAME/src/emu/switchres/monitor.cpp).
```
custom Define your own custom by means of -crt_range lines
pal PAL TV - 50 Hz/625
ntsc NTSC TV - 60 Hz/525
generic_15 Generic 15.7 kHz
arcade_15 Arcade 15.7 kHz - standard resolution
arcade_15ex Arcade 15.7-16.5 kHz - extended resolution
arcade_25 Arcade 25.0 kHz - medium resolution
arcade_31 Arcade 31.5 kHz - high resolution
arcade_15_25 Arcade 15.7/25.0 kHz - dual-sync
arcade_15_25_31 Arcade 15.7/25.0/31.5 kHz - tri-sync
m2929 Makvision 2929D
d9200 Wells Gardner D9200
d9400 Wells Gardner D9400
d9800 Wells Gardner D9800
k7000 Wells Gardner K7000
k7131 Wells Gardner 25K7131
m3129 Wei-Ya M3129
h9110 Hantarex MTC 9110
polo Hantarex Polo
pstar Hantarex Polostar 25
ms2930 Nanao MS-2930, MS-2931
ms929 Nanao MS9-29
vesa_480 VESA GTF
vesa_600 VESA GTF
vesa_768 VESA GTF
vesa_1024 VESA GTF
```

## Running ddr-mame/GroovyMAME on Linux
The following things are important to know to make things work on Linux.

### Running MAME as root causes various issues
Running MAME as root causes weird issues like files do not get written, e.g. `nvram` files, creating
default configuration doesn't work, keyboard inputs don't work, sound doesn't work etc.

One option to solve this is running MAME always as user. There are a few more things that might need
to be configured if you are not running this from a desktop user environment, e.g. headless on a
cabinet.

### MAME cannot find and open any sound device with the ALSA sound driver
On a headless setup, to allow mame to find any sound devices using ALSA when running as normal user
just give everyone permissions to read/write them:
```shell
sudo chmod -R a+rw /dev/snd
```

### Libusb access error when using DDRIO implementations that use libusb
Another permission issue when running MAME with a DDRIO implementation that uses libusb. You have
to create a udev rule for the device using the VID and PID of it and set the right permissions.

Use `lsusb` to figure out the VID and PID of the USB device. Then, create a udev rule in
`/etc/udev/rules.d`, e.g. for the PIUIO USB device `/etc/udev/rules.d/50-piuio.rules` with the
following contents:
```text
SUBSYSTEM=="usb", ATTRS{idVendor}=="0547", MODE="0666"
SUBSYSTEM=="usb_device", ATTRS{idVendor}=="0547", MODE="0666"
```

Replace the VID `0547` with the one of your USB device.

Reload the udev rules without rebooting:
```shell
udevadm control --reload-rules && udevadm trigger
```

## GroovyMAME strip down process, notes
* The build system uses a bunch of lua scripts to determine what to build. These scripts are located
in the `scripts` sub-folder. Relevant folders for stripping: `scripts/target/mame`, `scripts/src`
* `scripts/target/mame/mame.lua`: Remove other projects that are not `mame`
* `scripts/target/mame/arcade.lua`
* Set entries of all of the following arrays to `null`, e.g. `MY_ARRAY["asdf"] = null` (use search + replace):
    * `CPUS`
    * `SOUNDS`
    * `VIDEOS`
    * `MACHINES`
    * `BUSES`
    * `FORMATS`
* Further below in the file, you have all the drivers listed by vendor, e.g. `konami`. Remove all entries that are not matching the vendor you want to compile for from the `links` structure returned
by the `linkProjects_mame_arcade` function.
* Remove all entries that not match your selected vendor from the function
`createProjects_mame_arcade`
* Depending on which games you want to support, start removing any drivers that you do not need to
run the games of your choice from the source list of the selected vendor.
* Strip the driver list from any drivers you don't use: `src/mame/arcade.flt` and
`src/mame/mame.lst`
* Next some trial and error: Compile the project and wait for errors regarding missing components,
e.g. CPU, missing, search for the name in the list (the names might differ slightly) and enable the component by setting it to `TRUE` again. The stripped down source file list for the mame project
might need additional files when they appear as missing or unresolved references in the compiler
output. Continue doing that until everything compiles.
* Once everything compiles, remove the files that are not used from the `src` folder. Iterate the different sub-folders relating to `drivers`, `devices`, `machines`. I picked a folder and just
removed the contents and then compiled the project. Any errors about missing files or references, I fixed by moving the required files from a copy of the MAME project back to the folders.
* Saving disk space. There are a bunch of files that are not required for building mame, for
example:
    * Remove unused languages from `language` folder
    * Tests in `3rdparty/bgfx/3rdparty/glslang` and `3rdparty/bgfx/3rdparty/glsl-optimizer`
    * The `hash` folder: Remove everything except for the system, e.g. `psx.xml`, that you need. You might also want to look reducing the number of games in the file.
    * `regtests`: Remove everything except for `regtests.mak`
    * Various folders in root that are not used/required for the fork, e.g. docs
