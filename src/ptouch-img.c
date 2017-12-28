///#exe

#include "PtouchPrinter.h"

int main(){
  PtouchPrinter_initUsb();
  PtouchPrinter_scanForPrinters(1);
}
