#include <stdio.h>
#include <stdlib.h>

#include <gctypes.h>
#include <gcutil.h>
#include <ogc/cache.h>
#include <ogc/consol.h>
#include <ogc/exi.h>
#include <ogc/pad.h>
#include <ogc/video.h>
#include <ogc/system.h>

#define BYTES_TO_READ 256

static void *xfb = NULL;
GXRModeObj *rmode;

u8 ipl_buffer[BYTES_TO_READ] ATTRIBUTE_ALIGN(32);

// We bring our own copies of these libogc functions to read at 32MHz
static u32 read_rom(void *buf, u32 len, u32 offset)
{
    u32 ret;
    u32 loff;

    DCInvalidateRange(buf, len);

    if (EXI_Lock(EXI_CHANNEL_0, EXI_DEVICE_1, NULL) == 0)
        return 0;

    if (EXI_Select(EXI_CHANNEL_0, EXI_DEVICE_1, EXI_SPEED32MHZ) == 0)
    {
        EXI_Unlock(EXI_CHANNEL_0);
        return 0;
    }

    ret = 0;
    loff = offset << 6;

    if (EXI_Imm(EXI_CHANNEL_0, &loff, 4, EXI_WRITE, NULL) == 0)
        ret |= 0x0001;

    if (EXI_Sync(EXI_CHANNEL_0) == 0)
        ret |= 0x0002;

    if (EXI_Dma(EXI_CHANNEL_0, buf, len, EXI_READ, NULL) == 0)
        ret |= 0x0004;

    if (EXI_Sync(EXI_CHANNEL_0) == 0)
        ret |= 0x0008;

    if (EXI_Deselect(EXI_CHANNEL_0) == 0)
        ret |= 0x0010;

    if (EXI_Unlock(EXI_CHANNEL_0) == 0)
        ret |= 0x00020;

    return ret != 0;
}

void SYS_ReadROM(void *buf, u32 len, u32 offset)
{
    u32 cpy_cnt;

    while (len > 0)
    {
        cpy_cnt = (len > 256) ? 256 : len;
        while(read_rom(buf, cpy_cnt, offset) == 0);
        offset += cpy_cnt;
        buf += cpy_cnt;
        len -= cpy_cnt;
    }
}

// https://gist.github.com/ccbrown/9722406
void DumpHex(const void *data, size_t size)
{
    char ascii[17];
    ascii[16] = '\0';
    size_t i, j;

    for (i = 0; i < size; ++i)
    {
        printf("%02X ", ((unsigned char *) data)[i]);

        if (((unsigned char *) data)[i] >= ' ' && ((unsigned char *) data)[i] <= '~')
        {
            ascii[i % 16] = ((unsigned char *) data)[i];
        }
        else
        {
            ascii[i % 16] = '.';
        }

        if ((i + 1) % 8 == 0 || i + 1 == size)
        {
            printf(" ");

            if ((i + 1) % 16 == 0)
            {
                printf("|  %s\n", ascii);
            }
            else if (i+1 == size)
            {
                ascii[(i + 1) % 16] = '\0';

                if ((i + 1) % 16 <= 8)
                    printf(" ");

                for (j = (i + 1) % 16; j < 16; ++j)
                    printf("   ");

                printf("|  %s\n", ascii);
            }
        }
    }
}

int main()
{
    VIDEO_Init();
    rmode = VIDEO_GetPreferredMode(NULL);
    xfb = MEM_K0_TO_K1(SYS_AllocateFramebuffer(rmode));
    VIDEO_Configure(rmode);
    VIDEO_SetNextFramebuffer(xfb);
    VIDEO_SetBlack(FALSE);
    VIDEO_Flush();
    VIDEO_WaitVSync();
    if (rmode->viTVMode & VI_NON_INTERLACE)
        VIDEO_WaitVSync();

    console_init(xfb, 20, 20, rmode->fbWidth, rmode->xfbHeight, rmode->fbWidth * 2);
    printf("\n\nIPL dumptest");

    PAD_Init();

    *(volatile unsigned long *) 0xcc00643c = 0x00000000; //allow 32mhz exi bus

    while (1)
    {
        int buttonsDown = 0;
        while ((buttonsDown & PAD_BUTTON_A) == 0)
        {
            VIDEO_WaitVSync();
            PAD_ScanPads();

            buttonsDown = PAD_ButtonsDown(0);
        }

        printf("\nReading %d first bytes of IPL ROM\n", BYTES_TO_READ);
        SYS_ReadROM(ipl_buffer, BYTES_TO_READ, 0);
        DumpHex(ipl_buffer, BYTES_TO_READ);
    }
}
