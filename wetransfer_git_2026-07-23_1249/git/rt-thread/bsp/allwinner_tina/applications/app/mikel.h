#include <stdint.h>

struct mikel_t
{
    uint8_t Left;
    uint8_t Right;
    uint8_t Arrows:2;
    uint8_t Reserved:2;
    uint8_t StringId:4;
    uint8_t Year;
    uint8_t Month;
    uint8_t Day;
    uint8_t Hour;
    uint8_t Minute;
    uint8_t Second;
    uint8_t String[32];
    uint8_t CRCH;
    uint8_t CRCL;
};

/* UART
    Data Bits   8 Bits
    Parity      None
    Stop Bits   1 Bit
    Baud Rate   57600
    */

//  Arrow DirecƟon
#define NO_DIRECTION    0
#define DOWN_DIRECTION  1
#define UP_DIRECTION    2
#define BI_DIRECTION    3

//  String Id
#define MCI_No_String   0
#define MCI_Fire_Alarm  1
#define MCI_Busy        2
#define MCI_No_Service  4
#define MCI_Maintenance 5


/*
1- Yatay Dikey bilgisi
2- Renk Kominasyonu


Bosluk eklenecek
Soru isareti
X char
*/