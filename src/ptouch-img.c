///#exe

#include "PtouchPrinter.h"

int main(){
  if (!PtouchPrinter_initUsb()){
    return 1;
  }
  PtouchPrinter_scanForPrinters(1);


  return 0;
}
