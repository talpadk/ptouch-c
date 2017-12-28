#ifndef PTOUCH_PRINTER
#define PTOUCH_PRINTER

#include <libusb.h>
#include <stdint.h>

typedef struct {
  libusb_device **deviceList;
  libusb_device *device;
  libusb_device_handle *deviceHandle;
  uint8_t reattachKernelDriverWhenDone;
} PtouchPrinter;

/** 
 * Initialize the USB system, must be called once
 * 
 */
uint8_t PtouchPrinter_initUsb(void);

/** 
 * Scans the USB busses and searches for supported devices
 * 
 * @param printResults if true the scan will list the usable devices on stdout
 * 
 * @return the number of devices found
 */
uint32_t PtouchPrinter_scanForPrinters(uint8_t printResults);


uint8_t PtouchPrinter_createByIndex(PtouchPrinter *this, uint32_t index);

#endif //PTOUCH_PRINTER
