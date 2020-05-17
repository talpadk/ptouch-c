#ifndef PTOUCHPRINTERSTATUS_H
#define PTOUCHPRINTERSTATUS_H

#include <stdint.h>


typedef struct {
  uint8_t error1;
  uint8_t error2;
  uint8_t mediaWidth;
  uint8_t mediaType;
  uint8_t tapeColour;
  uint8_t textColour;
  
} PtouchPrinterStatus;

void PtouchPrinterStatus_updateStatusFromBuffer(PtouchPrinterStatus *this, uint8_t buffer[32]);

void PtouchPrinterStatus_printToStdOut(PtouchPrinterStatus *this);

#endif //PTOUCHPRINTERSTATUS_H 
