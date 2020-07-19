#include <stdio.h>

#include "piuio/device/device.h"
#include "piuio/device/usb.h"

#define USB_VID	0x0547
#define USB_PID 0x1002

#define USB_CONFIG 0x01
#define USB_IFACE 0x00

#define USB_CTRL_TYPE_IN 0xC0
#define USB_CTRL_TYPE_OUT 0x40
#define USB_CTRL_REQUEST 0xAE
#define USB_REQ_TIMEOUT	10000

#define BUFFER_SIZE 8

static void* _device_handle;

bool piuio_device_open(void)
{
    if (_device_handle) {
        printf("ERROR: Device already opened\n");
        return true;
    }

    _device_handle = piuio_device_usb_open(USB_VID, USB_PID, USB_CONFIG, USB_IFACE);

    return _device_handle;
}

bool piuio_device_read(uint8_t* buffer, uint8_t len)
{
    int32_t res;

    if (!_device_handle) {
        printf("ERROR: Device not opened\n");
        return false;
    }

    if (len < BUFFER_SIZE) {
        printf("ERROR: Read failed, buffer (%d) too small\n", len);
        return false;
    }

    res = piuio_device_usb_control_transfer(_device_handle,
        USB_CTRL_TYPE_IN, USB_CTRL_REQUEST, 0, 0, buffer,
        BUFFER_SIZE, USB_REQ_TIMEOUT);

    if (res != BUFFER_SIZE) {
        printf("ERROR: Read failed: %d\n", res);
        return false;
    }

    return true;
}

bool piuio_device_write(uint8_t* buffer, uint8_t len)
{
    int32_t res;

    if (!_device_handle) {
        printf("ERROR: Device not opened\n");
        return false;
    }

    if (len < BUFFER_SIZE) {
        printf("ERROR: Write failed, buffer (%d) too small\n", len);
        return false;
    }

    res = piuio_device_usb_control_transfer(_device_handle,
        USB_CTRL_TYPE_OUT, USB_CTRL_REQUEST, 0, 0, buffer,
        BUFFER_SIZE, USB_REQ_TIMEOUT);

    if (res != BUFFER_SIZE) {
        printf("ERROR: Write failed: %d\n", res);
        return false;
    }

    return true;
}

void piuio_device_close(void)
{
    if (!_device_handle) {
        printf("ERROR: Device not opened\n");
    }

    piuio_device_usb_close(_device_handle);
}