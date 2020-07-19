#include <libusb-1.0/libusb.h>
#include <stdio.h>
#include <stdlib.h>

struct piuio_device_usb_io_ctx {
    struct libusb_context* ctx;
    struct libusb_device_handle* dev;
};

static struct libusb_device_handle* io_usb_get_device_handle(
        struct libusb_context* ctx, uint16_t vid, uint16_t pid)
{
    int ret;
	struct libusb_device** dev_list;
	struct libusb_device* device;
    struct libusb_device_handle* handle;

    dev_list = NULL;
    device = NULL;

	ssize_t num_dev = libusb_get_device_list(ctx, &dev_list);

    /* find device */
	for (ssize_t i = 0; i < num_dev; i++) {
		struct libusb_device_descriptor dev_desc_tmp;
		struct libusb_device* dev_tmp;

        dev_tmp = dev_list[i];

		libusb_get_device_descriptor(dev_tmp, &dev_desc_tmp);

		/* for handling multiple devices with the same pid and vid */
		/*
        device_id_tmp = ((libusb_get_bus_number(dev_tmp) << 8) |
                libusb_get_port_number(dev_tmp));
        */

		if (dev_desc_tmp.idVendor == vid && dev_desc_tmp.idProduct == pid) {
			device = dev_tmp;
			break;
		}
	}

	/* not found */
	if (device == NULL) {
		libusb_free_device_list(dev_list, 1);
        printf("ERROR: Could not find device %04X:%04X\n", vid, pid);
		return NULL;
	}

	ret = libusb_open(device, &handle);
	if (ret != LIBUSB_SUCCESS) {
		libusb_free_device_list(dev_list, 1);

        printf("ERROR: Could not open device %04X:%04X: %s\n", vid, pid,
            libusb_error_name(ret));
        return NULL;
	}

	libusb_free_device_list(dev_list, 1);

	return handle;
}

void* piuio_device_usb_open(uint16_t vid, uint16_t pid, uint16_t config, uint16_t iface)
{
    int ret;
    struct libusb_context* ctx;
    struct libusb_device_handle* dev;
    struct piuio_device_usb_io_ctx* handle;

    printf("Opening %04X:%04X, config %d, iface %d\n", vid, pid, config,
        iface);

	ret = libusb_init(&ctx);

	if (ret != LIBUSB_SUCCESS) {
        printf("ERROR: (%04X:%04X) Initializing libusb context failed: %s\n",
            vid, pid, libusb_error_name(ret));
        return NULL;
    }

	/* dont print any error messages to stderr, change this for debugging */
	// libusb_set_option(ctx, LIBUSB_OPTION_LOG_LEVEL, LIBUSB_LOG_LEVEL_NONE);

	dev = io_usb_get_device_handle(ctx, vid, pid);

	if (!dev) {
		libusb_exit(ctx);
		return NULL;
	}

    ret = libusb_set_configuration(dev, config);

    if (ret != LIBUSB_SUCCESS) {
        libusb_close(dev);
        libusb_exit(ctx);

        printf("ERROR: (%04X:%04X) Setting configuration failed\n", vid, pid,
            libusb_error_name(ret));
		return NULL;
    }

    /* check if the device is attached to the kernel, detach it first then */
	if (libusb_kernel_driver_active(dev, iface) == 1) {
		ret = libusb_detach_kernel_driver(dev, iface);
		if (ret != LIBUSB_SUCCESS) {
            libusb_close(dev);
            libusb_exit(ctx);

            printf(
                "ERROR: (%04X:%04X) Detaching interface %d from kernel failed: %s\n",
                vid, pid, iface, libusb_error_name(ret));
            return NULL;
        }
	}

	ret = libusb_claim_interface(dev, iface);

	if (ret != LIBUSB_SUCCESS) {
        libusb_close(dev);
        libusb_exit(ctx);

        printf("ERROR: (%04X:%04X) Claiming interface %d failed: %s\n", vid, pid,
            iface, libusb_error_name(ret));
        return NULL;
    }

    handle = (struct piuio_device_usb_io_ctx*) malloc(sizeof(struct piuio_device_usb_io_ctx));

    handle->ctx = ctx;
    handle->dev = dev;

    printf("Opened %04X:%04X, handle %p\n", vid, pid, handle);

    return handle;
}

int32_t piuio_device_usb_control_transfer(void* handle, uint8_t request_type,
        uint8_t request, uint16_t value, uint16_t index, uint8_t* data,
        uint16_t len, uint32_t timeout)
{
    struct piuio_device_usb_io_ctx* dev = (struct piuio_device_usb_io_ctx*) handle;
    int32_t ret;

    ret = libusb_control_transfer(dev->dev, request_type, request, value,
        index, data, len, timeout);

    if (ret != len) {
        printf("ERROR: Control transfer, handle %p failed: %s\n", handle,
            libusb_error_name(ret));
    }

    return ret;
}

void piuio_device_usb_close(void* handle)
{
    struct piuio_device_usb_io_ctx* dev = (struct piuio_device_usb_io_ctx*) handle;

    printf("Closing handle %p\n", handle);

    libusb_close(dev->dev);
    libusb_exit(dev->ctx);

    free(dev);
}
