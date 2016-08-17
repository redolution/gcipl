#include <avr/interrupt.h>
#include <avr/io.h>
#include <avr/power.h>
#include <avr/wdt.h>

#include <LUFA/Drivers/USB/USB.h>

#include "descriptors.h"
#include "reset.h"

enum requests
{
    REQ_Bootloader = 0xF0,
};

char hello[] = "Hello, World!\n";

int main()
{
    // Disable watchdog if enabled by bootloader/fuses
    MCUSR &= ~(1 << WDRF);
    wdt_disable();

    clock_prescale_set(clock_div_1);

    USB_Init();
    USB_Device_CurrentlySelfPowered = true;

    GlobalInterruptEnable();

    while (1)
    {
        Endpoint_SelectEndpoint(VENDOR_DEBUG_IN_EPADDR);
        Endpoint_Write_Stream_LE(hello, sizeof(hello), NULL);
        Endpoint_ClearIN();
    }
}

void EVENT_USB_Device_ConfigurationChanged()
{
    bool ConfigSuccess = true;

    ConfigSuccess &= Endpoint_ConfigureEndpoint(VENDOR_IN_EPADDR, EP_TYPE_BULK, VENDOR_IO_EPSIZE, 1);
    ConfigSuccess &= Endpoint_ConfigureEndpoint(VENDOR_OUT_EPADDR, EP_TYPE_BULK, VENDOR_IO_EPSIZE, 1);
    ConfigSuccess &= Endpoint_ConfigureEndpoint(VENDOR_DEBUG_IN_EPADDR, EP_TYPE_BULK, VENDOR_IO_EPSIZE, 1);
}

void EVENT_USB_Device_ControlRequest()
{
    switch (USB_ControlRequest.bRequest)
    {
        case REQ_Bootloader:
            bootloader_reset();
    }
}
