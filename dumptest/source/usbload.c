/*
 * USB-Load - Homebrew DOL loading code for the GameCube via USB-Gecko
 * - This is messy slapped together code that was untouched once it 
 * was determined that it "worked good enough code", please bare with it.
 *
 * - emu_kidid
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */
#include <stdio.h>
#include <gccore.h>		/*** Wrapper to include common libogc headers ***/
#include <ogcsys.h>		/*** Needed for console support ***/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <malloc.h>
#include <ogc/usbgecko.h>
#include "sidestep.h"

#define GECKO_CHANNEL 0
#define PC_READY 0x80
#define PC_OK    0x81
#define GC_READY 0x88
#define GC_OK    0x89

unsigned int convert_int(unsigned int in)
{
  unsigned int out;
  char *p_in = (char *) &in;
  char *p_out = (char *) &out;
  p_out[0] = p_in[3];
  p_out[1] = p_in[2];
  p_out[2] = p_in[1];
  p_out[3] = p_in[0];  
  return out;
}
#define __stringify(rn) #rn
#define mfspr(rn) ({unsigned int rval;  asm volatile("mfspr %0," __stringify(rn) : "=r" (rval)); rval;})

/****************************************************************************
* Main
****************************************************************************/
void usbload_main ()
{
	AR_Init(NULL, 0); /*** No stack - we need it all ***/
  
	*(volatile unsigned long*)0xcc00643c = 0x00000000; //allow 32mhz exi bus
	ipl_set_config(6); 

	int mram_size = (SYS_GetArenaHi()-SYS_GetArenaLo());
	int aram_size = (AR_GetSize()-AR_GetBaseAddress());
	unsigned char data = 0;
	unsigned int size = 0;
	
	printf("\n\nUSB-Gecko DOL loader for GameCube by emu_kidid\n\n");
	printf("Memory Available: [MRAM] %i KB [ARAM] %i KB\n",(mram_size/1024), (aram_size/1024));
	printf("Largest DOL possible: %i KB\n", mram_size < aram_size ? mram_size/1024:aram_size/1024);

	usb_flush(GECKO_CHANNEL);
	
	printf("\nSending ready\n");
	data = GC_READY;
	usb_sendbuffer_safe(GECKO_CHANNEL,&data,1);

	printf("Waiting for connection via USB-Gecko in Slot B ...\n");
	while((data != PC_READY) && (data != PC_OK)) {
		usb_recvbuffer_safe(GECKO_CHANNEL,&data,1);
	}
	
	if(data == PC_READY)
	{
		printf("Respond with OK\n");
		// Sometimes the PC can fail to receive the byte, this helps
		usleep(100000);
		data = GC_OK;
		usb_sendbuffer_safe(GECKO_CHANNEL,&data,1);
	}
	
	printf("Getting DOL info...\n");
	usb_recvbuffer_safe(GECKO_CHANNEL,&size,4);
	size = convert_int(size);
	printf("Size: %i bytes\n",size);
	printf("Receiving file...\n");
	unsigned char* buffer = (unsigned char*)memalign(32,size);
	unsigned char* pointer = buffer;
	
	if(!buffer) {
		printf("Failed to allocate memory!!\n");
		while(1);
	}
	
	while(size>0xF7D8)
	{
		usb_recvbuffer_safe(GECKO_CHANNEL,(void*)pointer,0xF7D8);
		size-=0xF7D8;
		pointer+=0xF7D8;
	}
	if(size)
	{
		usb_recvbuffer_safe(GECKO_CHANNEL,(void*)pointer,size);
	}
	
	DOLHEADER *dolhdr = (DOLHEADER*)buffer;
	
	printf("DOL Load address: %08X\n", dolhdr->textAddress[0]);
	printf("DOL Entrypoint: %08X\n", dolhdr->entryPoint);
	printf("BSS: %08X Size: %iKB\n", dolhdr->bssAddress, (int)((float)dolhdr->bssLength/1024));
	sleep(1);
	
	DOLtoARAM(buffer);
	while(1);
}
