#include "PtouchPrinter.h"


#include <stdio.h>

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


//void PtouchPrinter_addUsbError(PtouchPrinter *this, const char* error, libusb_error code){
  //  fprintf(stderr, "%s %s (%d)\n", error, usb_error_name(code), code);
//}

void PtouchPrinter_initUsb(void){
  usb_init();
  usb_find_busses();
  usb_find_devices();
}


uint32_t PtouchPrinter_deviceIsSupported(struct usb_device *device){
  for (uint32_t i=0; i<NUMBER_OF_SUPPORTED_PRINTERS; i++){
    if (device->descriptor.idVendor  == supportedPrinters_[i].vendor &&
	device->descriptor.idProduct == supportedPrinters_[i].product){
      return i+1;
    }
  }
  return 0;
}

uint32_t PtouchPrinter_scanForPrinters(uint8_t printResults){
  uint32_t numberOfDevicesFound = 0;
  struct usb_bus *busses = usb_get_busses();

  struct usb_bus *bus;
  for (bus = busses; bus; bus = bus->next) {
    struct usb_device *dev;
    for (dev = bus->devices; dev; dev = dev->next) {
      uint32_t isSupported = PtouchPrinter_deviceIsSupported(dev);
      if (isSupported){
	printf("#%d %s\n", numberOfDevicesFound, supportedPrinters_[isSupported-1].name);
	numberOfDevicesFound++;
      }
    }
  }

  return numberOfDevicesFound;
}


uint8_t PtouchPrinter_createByIndex(PtouchPrinter *this, uint32_t index){
  int result = 0;
}
