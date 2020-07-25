# Debugging with real hardware on workstation with USB over IP
Source: <https://wiki.archlinux.org/index.php/USB/IP>

Get access to USB PIUIO hardware of remote machine on development workstation.

## Server side (cabinet)
1. Install usbip. On Ubuntu, it's part of the `linux-tools-generic` package: 
`apt-get install linux-tools-generic`
1. Insert required kernel module: `modprobe usbip-host`
1. List devices: `usbip list -l`
1. Bind device: `usbip bind -b 2-1.6`, replace the bus ID with whatever is shown on the list.
1. Run deamon: `usbipd -D`

Disconnect when done:
`usbip unbind -b 2-1.6`, replace bus ID accordingly

## Client side (development workstation)
1. Also install usbip
1. List remote devices: `usbip list -r <IP OF REMOTE>`
1. Attach remote device: `usbip attach -r <IP REMOTE> -b 2-1.6`, replace bus ID accordingly
1. List imported USB devices: `usbip port`
1. Check if remote device shows up as local: `lsusb`

Disconnect when done:
`usbip detach -p 00`, replace `00` with port number from `usbip port`