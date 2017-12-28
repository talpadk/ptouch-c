#include "PtouchPrinter.h"


#include <stdio.h>

static libusb_context *usbContext_=0;

	
typedef struct {
  uint16_t vendor;
  uint16_t product;
  const char *name; 
} PrinterId;

#define NUMBER_OF_SUPPORTED_PRINTERS (3)
PrinterId supportedPrinters_[NUMBER_OF_SUPPORTED_PRINTERS] = {
  {.vendor=0x04F9, .product=0x205E, .name="PT-H500"},
  {.vendor=0x04F9, .product=0x205F, .name="PT-E500"},
  {.vendor=0x04F9, .product=0x2061, .name="PT-P700"}
};


void PtouchPrinter_addUsbError(PtouchPrinter *this, const char* error, enum libusb_error code){
  fprintf(stderr, "%s %s (%d)\n", error, libusb_error_name(code), code);
}

uint8_t PtouchPrinter_initUsb(void){
  if (usbContext_!=0) { return 1; }
  
  int result = libusb_init(&usbContext_);
  if (result != LIBUSB_SUCCESS){
    PtouchPrinter_addUsbError(0,"Failed to open libUSB", result);
    return 0;
  }
  return 1;
}

//Requires libusb >= 1.0.16 as we don't check for errors
uint32_t PtouchPrinter_deviceIsSupported(libusb_device *device){
  struct libusb_device_descriptor descriptor;
  libusb_get_device_descriptor(device, &descriptor);
  
  for (uint32_t i=0; i<NUMBER_OF_SUPPORTED_PRINTERS; i++){
    if (descriptor.idVendor  == supportedPrinters_[i].vendor &&
	descriptor.idProduct == supportedPrinters_[i].product){
      return i+1;
    }
  }
  return 0;
}

uint32_t PtouchPrinter_scanForPrinters(uint8_t printResults){
  uint32_t numberOfDevicesFound = 0;

  libusb_device **deviceList;
  ssize_t numberOfDevices = libusb_get_device_list(usbContext_, &deviceList);

  if (numberOfDevices < 0){
    PtouchPrinter_addUsbError(0, "Failed to get device list", numberOfDevices);
  }
  else {
    for (ssize_t i=0; i<numberOfDevices; i++){
      uint32_t isSupported = PtouchPrinter_deviceIsSupported(deviceList[i]);
      if (isSupported){
	printf("#%d %s\n", numberOfDevicesFound, supportedPrinters_[isSupported-1].name);
	numberOfDevicesFound++;
      }
    }
    
    libusb_free_device_list(deviceList, 0);
  }

 return numberOfDevicesFound;
}


uint8_t PtouchPrinter_createByIndex(PtouchPrinter *this, uint32_t index){
  int result = 0;
}
