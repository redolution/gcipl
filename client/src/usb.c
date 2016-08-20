#include <libusb-1.0/libusb.h>

#include "log.h"
#include "usb.h"

static libusb_device_handle *usb_current_device = NULL;

int usb_init(void)
{
    int rc = libusb_init(NULL);
    if (rc)
    {
        putlog(ERROR, "Failed to initialize libusb: %s", libusb_error_name(rc));
        goto error;
    }

    libusb_set_debug(NULL, LIBUSB_LOG_LEVEL_INFO);

    return 0;

error:
    return 1;
}

void usb_deinit(void)
{
    libusb_exit(NULL);
}

int usb_open_device(void)
{
    usb_current_device = libusb_open_device_with_vid_pid(NULL, 0x03EB, 0x1337);
    if (usb_current_device == NULL)
    {
        putlog(WARN, "Device not found");
        goto error;
    }

    int rc = libusb_claim_interface(usb_current_device, 0);
    if (rc)
    {
        putlog(ERROR, "Failed to claim interface: %s", libusb_error_name(rc));
        goto close_device;
    }

    return 0;

close_device:
    libusb_close(usb_current_device);
    usb_current_device = NULL;

error:
    return 1;
}

void usb_close_device(void)
{
    int rc = libusb_release_interface(usb_current_device, 0);
    if (rc)
        putlog(ERROR, "Failed to release interface: %s", libusb_error_name(rc));

    libusb_close(usb_current_device);
    usb_current_device = NULL;
}

int usb_control_request(uint8_t bmRequestType, uint8_t bRequest, uint16_t wValue,
        uint16_t wIndex, unsigned char *data, uint16_t wLength, unsigned int timeout)
{
    int rc = libusb_control_transfer(usb_current_device, bmRequestType, bRequest, wValue,
            wIndex, data, wLength, timeout);
    if (rc < 0 && rc != LIBUSB_ERROR_TIMEOUT)
        putlog(ERROR, "Control transfer failed: %s", libusb_error_name(rc));

    return rc;
}

    /* Bulk transfer draft
    int transferred;
    printf("transfer: %d\n", libusb_bulk_transfer(avr, 2 | LIBUSB_ENDPOINT_IN, buf, 512, &transferred, 0));

    write(STDOUT_FILENO, buf, (size_t) transferred);
    */
