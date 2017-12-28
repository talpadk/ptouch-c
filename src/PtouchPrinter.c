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

void PtouchPrinter_addError(PtouchPrinter *this, const char* error){
  fprintf(stderr, "%s\n", error);
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
    return 0;
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

libusb_device *PtouchPrinter_findPrinterByIndex(PtouchPrinter *this, uint32_t index){
  uint32_t numberOfDevicesFound = 0;
  ssize_t numberOfDevices = libusb_get_device_list(usbContext_, &(this->deviceList));

  if (numberOfDevices < 0){
    PtouchPrinter_addUsbError(0, "Failed to get device list", numberOfDevices);
    return 0;
  }
  else {
    for (ssize_t i=0; i<numberOfDevices; i++){
      uint32_t isSupported = PtouchPrinter_deviceIsSupported(this->deviceList[i]);
      if (isSupported){
	if (index==numberOfDevicesFound){
	  return this->deviceList[i];
	}
	numberOfDevicesFound++;
      }
    }
  }
 return 0;
}


void PtouchPrinter_init(PtouchPrinter *this){
  this->device = 0;
  this->deviceList = 0;
  this->deviceHandle = 0;
  this->reattachKernelDriverWhenDone = 0;
}


uint8_t PtouchPrinter_createByIndex(PtouchPrinter *this, uint32_t index){
  enum libusb_error errorCode;

  PtouchPrinter_init(this);
  this->device = PtouchPrinter_findPrinterByIndex(this, index);
  if (this->device==0){
    PtouchPrinter_addError(this, "Unable to find the printer with the requested index");
    return 0;
  }

  errorCode = libusb_open(this->device, &(this->deviceHandle));
  if (errorCode != LIBUSB_SUCCESS){
    this->deviceHandle = 0;
    PtouchPrinter_addUsbError(this, "Unable to open the USB device", errorCode);
    return 0;
  }

  //Attempt to detatch any kernel drivers for interface 0
  errorCode = libusb_detach_kernel_driver(this->deviceHandle, 0);
  if (errorCode == LIBUSB_SUCCESS){
    this->reattachKernelDriverWhenDone = 1;
  }
  
  errorCode = libusb_claim_interface(this->deviceHandle, 0);
  if (errorCode != LIBUSB_SUCCESS){
    PtouchPrinter_addUsbError(this, "Unable to claim USB device", errorCode);
  }
  
  return 1;
}
