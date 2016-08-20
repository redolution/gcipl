#ifndef INC_USB_H
#define INC_USB_H

#include <stdint.h>

int usb_init(void);
void usb_deinit(void);
int usb_open_device(void);
void usb_close_device(void);
int usb_control_request(uint8_t bmRequestType, uint8_t bRequest, uint16_t wValue,
        uint16_t wIndex, unsigned char *data, uint16_t wLength, unsigned int timeout);

#endif
