#include <avr/interrupt.h>
#include <avr/io.h>
#include <avr/power.h>
#include <avr/wdt.h>
#include <stdio.h>
#include <string.h>

#include <LUFA/Drivers/USB/USB.h>
#include <LUFA/Platform/Platform.h>

#include "descriptors.h"

USB_ClassInfo_CDC_Device_t VirtualSerial_CDC_Interface =
{
    .Config =
    {
        .ControlInterfaceNumber = INTERFACE_ID_CDC_CCI,
        .DataINEndpoint =
        {
            .Address = CDC_TX_EPADDR,
            .Size = CDC_TXRX_EPSIZE,
            .Banks = 1,
        },
        .DataOUTEndpoint =
        {
            .Address = CDC_RX_EPADDR,
            .Size = CDC_TXRX_EPSIZE,
            .Banks = 1,
        },
        .NotificationEndpoint =
        {
            .Address = CDC_NOTIFICATION_EPADDR,
            .Size = CDC_NOTIFICATION_EPSIZE,
            .Banks = 1,
        },
    },
};

int main()
{
    // Disable watchdog if enabled by bootloader/fuses
    MCUSR &= ~(1 << WDRF);
    wdt_disable();

    clock_prescale_set(clock_div_1);

    USB_Init();

    GlobalInterruptEnable();

    while (1)
    {
        // Must throw away unused bytes from the host, or it will lock up while waiting for the
        // device
        CDC_Device_ReceiveByte(&VirtualSerial_CDC_Interface);
        CDC_Device_SendString(&VirtualSerial_CDC_Interface, "hello\r\n");

        //CDC_Device_USBTask(&VirtualSerial_CDC_Interface);
        //USB_USBTask();
    }
}

void EVENT_USB_Device_ConfigurationChanged()
{
    CDC_Device_ConfigureEndpoints(&VirtualSerial_CDC_Interface);
}

void EVENT_USB_Device_ControlRequest()
{
    CDC_Device_ProcessControlRequest(&VirtualSerial_CDC_Interface);
}
