#include <avr/wdt.h>
#include <avr/io.h>
#include <util/delay.h>

#include <LUFA/Common/Common.h>
#include <LUFA/Drivers/USB/USB.h>

#include "reset.h"

#define BOOTLOADER_START_ADDRESS 0x7000

#define MAGIC_BOOT_KEY 0xDC42ACCA

static uint32_t boot_key ATTR_NO_INIT;

void bl_jump_check() ATTR_INIT_SECTION(3);
void bl_jump_check()
{
    // If the reset source was the bootloader and the key is correct,
    // clear it and jump to the bootloader
    if ((MCUSR & (1 << WDRF)) && (boot_key == MAGIC_BOOT_KEY))
    {
        boot_key = 0;
        ((void (*)(void))BOOTLOADER_START_ADDRESS)();
    }
}

void bootloader_reset()
{
    // Set the bootloader key to the magic value and force a reset
    boot_key = MAGIC_BOOT_KEY;
    wdt_enable(WDTO_15MS);
    while (1);
}
