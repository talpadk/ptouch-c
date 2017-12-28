#ifndef PTOUCH_PRINTER
#define PTOUCH_PRINTER

#include <usb.h>
#include <stdint.h>

typedef struct {
} PtouchPrinter;

/** 
 * Initialize the USB system, must be called once
 * 
 */
void PtouchPrinter_initUsb(void);

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
