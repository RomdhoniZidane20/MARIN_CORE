/*
	To build use the following gcc statement 
	(assuming you have the d2xx library in the /usr/local/lib directory).
	gcc -o write main.c -L. -lftd2xx -Wl,-rpath,/usr/local/lib
*/

#include <stdio.h>
#include <sys/time.h>
#include "./ftd2xx.h"

int main(int argc, char *argv[])
{
	FT_STATUS	ftStatus;
	FT_HANDLE	ftHandle0;
	int iport;
	FT_PROGRAM_DATA Data;
	
	if(argc > 1) {
		sscanf(argv[1], "%d", &iport);
	}
	else {
		iport = 0;
	}
	printf("opening port %d\n", iport);
	FT_SetVIDPID(0x0403, 0x6011);
	ftStatus = FT_Open(iport, &ftHandle0);
	
	if(ftStatus != FT_OK) {
		/* 
			This can fail if the ftdi_sio driver is loaded
		 	use lsmod to check this and rmmod ftdi_sio to remove
			also rmmod usbserial
		 */
		printf("FT_Open(%d) failed\n", iport);
		return 1;
	}

	printf("ftHandle0 = %p\n", ftHandle0);
	
	//set data
	Data.Signature1 = 0x00000000;
	Data.Signature2 = 0xffffffff;
	Data.VendorId = 0x0403;				
	Data.ProductId = 0x6001;
	Data.Manufacturer =  "FTDI";
	Data.ManufacturerId = "FT";
	Data.Description = "FT-ICHIRO";
	Data.SerialNumber = NULL;			// if fixed, or NULL
	
	Data.MaxPower = 44;
	Data.PnP = 1;
	Data.SelfPowered = 0;
	Data.RemoteWakeup = 1;
	Data.Rev4 = 1;
	Data.IsoIn = 0;
	Data.IsoOut = 0;
	Data.PullDownEnable = 1;
	Data.SerNumEnable = 1;
	Data.USBVersionEnable = 0;
	Data.USBVersion = 0x110;

	//enable TXDEN pin
	Data.ARIIsTXDEN = 1;
	Data.BRIIsTXDEN = 1;
	Data.CRIIsTXDEN = 1;
	Data.DRIIsTXDEN = 1;

	//write eeprom with data
	ftStatus = FT_EE_Program(ftHandle0, &Data);
	if(ftStatus != FT_OK) {
		printf("FT_EE_Program failed (%d)\n", (int)ftStatus);
		FT_Close(ftHandle0);
	}
	FT_Close(ftHandle0);
	return 0;
}
