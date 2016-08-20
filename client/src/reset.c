#include <stdio.h>

#include <libusb-1.0/libusb.h>

#include "reset.h"
#include "usb.h"

static int reset_avr(void)
{
    int rc = usb_open_device();
    if (rc == 0)
    {
        usb_control_request(LIBUSB_ENDPOINT_OUT
                | LIBUSB_REQUEST_TYPE_VENDOR
                | LIBUSB_RECIPIENT_DEVICE,
                0xF0, 0, 0, NULL, 0, 0);

        // TODO Wait for the bootloader to come up

        usb_close_device();
    }

    return 0;
}

struct cmd reset_cmd =
{
    .cmd = "bootloader",
    .desc = "Reset the AVR into the bootloader",
    .args = NULL,
    .arghook = NULL,
    .run = &reset_avr
};
