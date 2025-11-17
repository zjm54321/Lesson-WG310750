#include <stc51.h>
#include <stdint.h>

void serial_init(void)
{


    SCON = 0x50;
    TMOD = 0x20;
    TH1  = 0xFD;
    TL1  = 0xFD;
    AUXR = 0X00;
    ES   = 1;
    EA   = 1;
    TR1  = 1;
}