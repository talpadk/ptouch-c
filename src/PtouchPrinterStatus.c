#include "PtouchPrinterStatus.h"

#include <stdio.h>


void PtouchPrinterStatus_updateStatusFromBuffer(PtouchPrinterStatus *this, uint8_t buffer[32]){
  this->error1     = buffer[8];
  this->error2     = buffer[9];
  this->mediaWidth = buffer[10];
  this->mediaType  = buffer[11];
  this->tapeColour = buffer[24];
  this->textColour = buffer[25];
}

#define PRINT_WIDTH "% 13s" 


static void printMediaWidth(PtouchPrinterStatus *this){
  printf(PRINT_WIDTH, "Media width: ");
  switch (this->mediaWidth){
  default:
    printf("Unknown !?\n");
    break;
  case 0:
    printf("No Tape\n");
    break;
  case 4:
    printf("3.5mm\n");
    break;
  case 6:
    printf("6mm\n");
    break;
  case 9:
    printf("9mm\n");
    break;
  case 12:
    printf("12mm\n");
    break;
  case 18:
    printf("18mm\n");
    break;
  case 24:
    printf("24mm\n");
    break;
  }
}

static void printMediaType(PtouchPrinterStatus *this){
  printf(PRINT_WIDTH, "Media type: ");
  switch (this->mediaType){
  default:
    printf("Unknown !?\n");
    break;
  case 0x00:
    printf("No media\n");
    break;
  case 0x01:
    printf("Laminated tape\n");
    break;
  case 0x03:
    printf("Non-laminated tape\n");
    break;
  case 0x11:
    printf("Heat-shrink tube\n");
    break;
  case 0xFF:
    printf("Incompatible tape\n");
    break;
  }
}

static void printTextColour(PtouchPrinterStatus *this){
  printf(PRINT_WIDTH, "Text colour: ");
  switch (this->textColour) {
  default:
    printf("Unknown !?\n");
    break;
  case 0x01:
    printf("White\n");
    break;
  case 0x04:
    printf("Red\n");
    break;
  case 0x05:
    printf("Blue\n");
    break;
  case 0x08:
    printf("Black\n");
    break;
  case 0x0A:
    printf("Gold\n");
    break;
  case 0x62:
    printf("Blue(F)\n");
    break;
  case 0xF0:
    printf("Clearning\n"); //I wonder if that is suppoed to be cleaning
    break;
  case 0xF1:
    printf("Stensil\n");
    break;
  case 0x02:
    printf("Other\n");
    break;
  case 0xFF:
    printf("Incompatible\n");
    break;
  }
}

static void printTapeColour(PtouchPrinterStatus *this){
  printf(PRINT_WIDTH, "Tape colour: ");
  switch (this->tapeColour) {
  default:
    printf("Unknown !?\n");
    break;
  case 0x01:
    printf("White\n");
    break;
  case 0x02:
    printf("Other\n");
    break;
  case 0x03:
    printf("Clear\n");
    break;
  case 0x04:
    printf("Red\n");
    break;
  case 0x05:
    printf("Blue\n");
    break;
  case 0x06:
    printf("Yellow\n");
    break;
  case 0x07:
    printf("Green\n");
    break;
  case 0x08:
    printf("Black\n");
    break;
  case 0x09:
    printf("Clear(White text)\n");
    break;
  case 0x20:
    printf("Matte White\n");
    break;
  case 0x21:
    printf("Matte Clear\n");
    break;
  case 0x22:
    printf("Matte Silver\n");
    break;
  case 0x23:
    printf("Satin Gold\n");
    break;
  case 0x30:
    printf("Blue(D)\n");
    break;
  case 0x31:
    printf("Red(D)\n");
    break;
  case 0x40:
    printf("Fluorescent Orange\n");
    break;
  case 0x41:
    printf("Fluorescent Yellow\n");
    break;
  case 0x50:
    printf("Berry Pink(S)\n");
    break;
  case 0x51:
    printf("Light Gray(S)\n");
    break;
  case 0x52:
    printf("Lime Green(S)\n");
    break;
  case 0x60:
    printf("Yellow(F)\n");
    break;
  case 0x61:
    printf("Pink(F)\n");
    break;
  case 0x62:
    printf("Blue(F)\n");
    break;
  case 0x70:
    printf("White(Heat-shrink Tube)\n");
    break;
  case 0x90:
    printf("White(Flex. ID)\n");
    break;
  case 0x91:
    printf("Yellow(Flex. ID)\n");
    break;
  case 0xF0:
    printf("Clearning\n");
    break;
  case 0xF1:
    printf("Stencil\n");
    break;
  case 0xFF:
    printf("Incompatible\n");
    break;
  }
}



void PtouchPrinterStatus_printToStdOut(PtouchPrinterStatus *this){
  printMediaWidth(this);
  printMediaType(this);
  printTextColour(this);
  printTapeColour(this);
}
