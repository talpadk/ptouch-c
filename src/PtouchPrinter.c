#include "PtouchPrinter.h"
#include <stdio.h>

#define OUT_ENDPOINT (0x02)
#define  IN_ENDPOINT (0x81)


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
  this->statusInformationBuffer[0] = 0;
}

void PtouchPrinter_updateStatusInformation(PtouchPrinter *this){
  enum libusb_error errorCode;
  int numberOfBytesTransfered;
  errorCode = libusb_bulk_transfer(this->deviceHandle, OUT_ENDPOINT, "\x1BiS", 3, &numberOfBytesTransfered, 3*1000);
  if (errorCode != LIBUSB_SUCCESS){
    PtouchPrinter_addUsbError(this, "Unable to send a request for status", errorCode);
    this->statusInformationBuffer[0] = 0;
    return;
  }
  if (numberOfBytesTransfered != 3){
    PtouchPrinter_addError(this, "Request for status was send too short");
    this->statusInformationBuffer[0] = 0;
    return;
  }
  errorCode = libusb_bulk_transfer(this->deviceHandle, IN_ENDPOINT, this->statusInformationBuffer, 32, &numberOfBytesTransfered, 10*1000);
  if (errorCode != LIBUSB_SUCCESS){
    PtouchPrinter_addUsbError(this, "Unable to receive request for status", errorCode);
    this->statusInformationBuffer[0] = 0;
    return;
  }
  if (numberOfBytesTransfered != 32){
    PtouchPrinter_addError(this, "Request for status was received too short");
    this->statusInformationBuffer[0] = 0;
    return;
  }

  if (this->statusInformationBuffer[0] != 0x80 ||
      this->statusInformationBuffer[1] != 0x20 ||
      this->statusInformationBuffer[2] != 0x42 ||
      this->statusInformationBuffer[3] != 0x30){
    PtouchPrinter_addError(this, "Unexpected status content, dumping content:");
    for (int i=0; i<32; i++){
      fprintf(stderr, "0x%02X ", this->statusInformationBuffer[i]);
    }
    fprintf(stderr, "\n");
    this->statusInformationBuffer[0] = 0;
    return;
  }
  
  printf("media width: %dmm\n", this->statusInformationBuffer[10]);
  
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


  PtouchPrinter_updateStatusInformation(this);
  
  return 1;
}
