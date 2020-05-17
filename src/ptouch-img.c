///#exe

#include "PtouchPrinter.h"

int main(){
  PtouchPrinter printer;
  
  if (!PtouchPrinter_initUsb()){
    return 1;
  }
  PtouchPrinter_scanForPrinters(1);

  PtouchPrinter_createByIndex(&printer, 0);

  PtouchPrinterStatus_printToStdOut(&(printer.printerStatus));

  return 0;
}
