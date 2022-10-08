/* Ant Bms UART protocol thanks to   // https://github.com/klotztech/VBMS/wiki/Serial-protocol


https://github.com/syssi/esphome-ant-bms :
│                ANT-BMS                       │
│                                              │
│  Comm                            Temp        │
└─[oooo]──[oooooooo]──[oooooooo]──[oooo]─┘
   ││││
   ││││      (ESP32)
   ││││
   │││└─ TXD (GPIO16)
   ││└── RXD (GPIO17)
   │└─── GND (GND)
   └──── VCC (3.3V)

┌──────────┐                ┌─────────┐
│                │<----- RX ----->│               │
│ ANT-BMS        │<----- TX ----->│ ESP32/        │
│                │<----- GND ---->│ ESP8266       │<-- 3.3V
│                │<----- 3.3V --->│               │<-- GND
└──────────┘                └─────────┘

The connector is a 4 Pin JST 1.25mm. It's important to connect VCC too because the BMS doesn't respond/start if you connect TXD, RXD and GND only.

Supported devices

    ANT-BLE16ZMUB (7-16S, 320A, 2021-11-26) // Robo Durden, who wrote this arduino code
    16ZMB-TB-7-16S-300A (7-16S, 300A, 2021-08-12)
    24AHA-TB-24S-200A (10-24S, 200A, 2021-09-28)
    ANT 16S 100A (16S, 100A, 2020)
    ANT 24S 200A (8-24S, 200A, 2020)
   
*/
   
#ifdef _DEBUG
  #define DEB(txt) {Serial.print(txt);}
  #define DEB2(txt,format) {Serial.print(txt,format);}
  #define DEBUG(txt, val) {Serial.print(txt); Serial.print(": "); Serial.print(val);}
  #define DEBUG2(txt, val,format) {Serial.print(txt); Serial.print(": "); Serial.print(val,format);}
  #define DEBUGT(txt, val) {Serial.print(txt); Serial.print(": "); Serial.print(val); Serial.print("\t");}
  #define DEBUGT2(txt, val,format) {Serial.print(txt); Serial.print(": "); Serial.print(val,format); Serial.print("\t");}
  #define DEBUGLN(txt, val) {Serial.print(txt); Serial.print(": "); Serial.println(val);}
  #define DEBUGLN2(txt, val,format) {Serial.print(txt); Serial.print(": "); Serial.println(val,format);}
#else
  #define DEB(txt)
  #define DEB2(txt,format)
  #define DEBUG(txt, val)
  #define DEBUG2(txt, val,format)
  #define DEBUGT(txt, val)
  #define DEBUGT2(txt, val,format)
  #define DEBUGLN(txt, val)
  #define DEBUGLN2(txt, val,format)
#endif


#include <EEPROM.h>

#define RXD2 17   // wrongly soldered RX2 to 17 instead of 16 ?
#define TXD2 16


unsigned long iNow = 0;
unsigned long iTimeRequest = TIME_RequestBms;


uint8_t aBmsRequest[6] = {0x5A, 0x5A, 0x00, 0x00, 0x00, 0x00};
#define BMS_DataSize 140
uint8_t aBmsHead[4] = {0xAA, 0x55, 0xAA, 0xFF};

#define EEPROM_VERSION 1

typedef struct
{
  uint32_t iVersion = EEPROM_VERSION;
  float fU = 0;
  float fI = 0;
  float fP = 0;
  unsigned long iTimeLast = 0;
  float fE = 0;   // Energy in Wh
  float fE_in = 0;
  float fE_out = 0;

  float fET = 0;  // total Energy in kWh
  float fET_in = 0;
  float fET_out = 0;

} BatteryData;

BatteryData oBattery;

void SaveEeprom()
{
  DEBUGLN("save eeprom, fET",oBattery.fET)
  EEPROM.put(0,oBattery);
  EEPROM.commit(); 
}


typedef struct
{
  float fU = 0;             // 4 - 69  Voltage data    0.000 V
  float afU[32];
  float fI = 0;             // 70 - 73  Current   int   0.0 A
  uint8_t iRemain = 0;        // 74  Percentage of remaining battery   u8
  float fCapacityAh = 0;   // 75 - 78  Battery physical capacity   u32   .000000 AH
  float fRemainingAh = 0;  // 79 - 82   The remaining battery capacity  u32   .000000 AH
  float fCycleAh = 0;      // 83 - 86   Total battery cycle capacity  u32   .000AH
  uint32_t iSeconds = 0;      // 87 - 90   Accumulated from boot time seconds  u32   S
  int16_t aiTemp[5];         // 91 - 102  Actual temperature  short   degree
  uint8_t wMosCharge = 0;     // 103   Charge mos tube status flag   u8  (after analysis)
  uint8_t wMosDischarge = 0;  // 104   Discharge mos tube status flag  u8  (after analysis)
  uint8_t wBalanced = 0;      // 105   Balanced status flag  u8  (resolved below)
  uint16_t iTireLength = 0;    // 106 - 107   Tire length   u16   MM
  uint16_t iPulsesPerWeek = 0;   // 108 - 109   The number of pulses per week   u16   N
  uint8_t   wRelaySwitch = 0;     // 110   Relay switch  u8  does not show
  int32_t iP = 0;                   // 111 - 114   Current Power   int   W
  uint8_t iCellMax = 0;      // 115   Maximum number of monomer strings   u8  None
  float fCellMax = 0;          // 116 - 117   The highest monomer   u16   0.000V
  uint8_t iCellMin = 0;     // 118   Lowest monomer string   u8  None
  float fCellMin = 0;          // 119 - 120   Lowest monomer  u16   0.000V
  float fCellAverage = 0;      // 121 - 122   Average   u16   0.000V
  uint8_t iBattEff = 0;         // 123   The number of effective batteries   u8  S
  float fMosDisDS = 0;      // 124 - 125   Detected discharge tube Voltage between D-S poles   u16   0.0V not to be displayed
  float fMosDisG = 0;       // 126 - 127   Discharge MOS transistor driving voltage  u16   0.0V not display
  float fMosChargeG = 0;    // 128 - 129   Charging MOS tube driving voltage   u16   0.0V not display
  uint16_t wCntrlEqual = 0; //   130 - 131   When the detected current is 0, the comparator initial value Control equalization corresponds to 1 equalization  u16   is not displayed
  uint32_t wEqualization = 0; // 132 - 135   (1 - 32 bits corresponds to 1 - 32 string equalization) corresponds to bit 1 displays the color at the corresponding voltage  u32   
  uint16_t wSysLog = 0;       // 136 - 137   The system log is sent to the serial port data
                              // 0 - 4: Status   5 - 9: Battery number     10 - 14: Sequential order     15: Charge and discharge (1 discharge, 0 charge)  u16   
} AntBmsData;

AntBmsData oData;


float fET_Old = 0;
unsigned long iTimeEeprom = 0;
void BatteryUpdate(BatteryData& oB, AntBmsData& oD)
{
  unsigned long iTime = millis();
  if (  (oB.iTimeLast) && ((iTime-oB.iTimeLast) < 5000) )
  {
    uint16_t iMs =  iTime-oB.iTimeLast;
    float fE = ((float)oD.iP * iMs) / 3600000;
    oB.fE += fE;
    oB.fET += fE/1000;
    if (fE > 0)
    {
      oB.fE_out += fE;
      oB.fET_out += fE/1000;
    }
    else
    {
      oB.fE_in -= fE;
      oB.fET_in -= fE/1000;
    }
    if (  (abs(oB.fET-fET_Old) > 1.0)  && (iTime-iTimeEeprom > (6*3600000) ) )   // save to eeprom only once in 6 hours. SPI flash chip might only have 10,000 cycles
    {
      SaveEeprom();
      fET_Old = oB.fET;
      iTimeEeprom = iTime;
    }
  }
  oB.iTimeLast = iTime;
}

uint16_t Swap2(uint8_t* buffer, uint16_t i){  return ((uint16_t)buffer[i]<<8) + buffer[i+1];  }
uint32_t Swap4(uint8_t* buffer, uint16_t i){  return ((uint32_t)buffer[i]<<24) + ((uint32_t)buffer[i+1]<<16) + ((uint32_t)buffer[i+2]<<8) + buffer[i+3];  }
int32_t Swap4i(uint8_t* buffer, uint16_t i){  return ((int32_t)buffer[i]<<24) + ((int32_t)buffer[i+1]<<16) + ((int32_t)buffer[i+2]<<8) + buffer[i+3]; }

void BmsDataCopy(AntBmsData& oData, uint8_t* buffer)
{
  oData.fU = (float)Swap2(buffer,4)/10;
  for (int i=0; i<32; i++)  oData.afU[i] = (float)Swap2(buffer,6+2*i)/1000;
  oData.fI = (float)Swap4i(buffer,70)/10;
  oData.iRemain = buffer[74];
  oData.fCapacityAh = (float)Swap4(buffer,75)/1000000;
  oData.fRemainingAh = (float)Swap4(buffer,79)/1000000;
  oData.fCycleAh = (float)Swap4(buffer,83)/1000;
  oData.iSeconds = Swap4(buffer,87);
  for (int i=0; i<5; i++)  oData.aiTemp[i] = (int16_t)Swap2(buffer,91+2*i);

  oData.wMosCharge = buffer[103];
  oData.wMosDischarge = buffer[104];
  oData.wBalanced = 0;      buffer[105];
  oData.iTireLength = 0;    Swap2(buffer,106);
  oData.iPulsesPerWeek = 0;   Swap2(buffer,108);
  oData.wRelaySwitch = 0;     buffer[110];
  
  oData.iP = (int32_t) Swap4(buffer,111);
  oData.iCellMax = buffer[115];
  oData.fCellMax = (float)Swap2(buffer,116)/1000;          // 116 - 117   The highest monomer   u16   0.000V
  oData.iCellMin = buffer[118];;     // 118   Lowest monomer string   u8  None
  oData.fCellMin = (float)Swap2(buffer,119)/1000;          // 119 - 120   Lowest monomer  u16   0.000V
  oData.fCellAverage = (float)Swap2(buffer,121)/1000;      // 121 - 122   Average   u16   0.000V
  oData.iBattEff = buffer[123];;         // 123   The number of effective batteries   u8  S

  oData.fMosDisDS = (float)Swap2(buffer,124)/10;      // 124 - 125   Detected discharge tube Voltage between D-S poles   u16   0.0V not to be displayed
  oData.fMosDisG = (float)Swap2(buffer,126)/10;       // 126 - 127   Discharge MOS transistor driving voltage  u16   0.0V not display
  oData.fMosChargeG = (float)Swap2(buffer,128)/10;    // 128 - 129   Charging MOS tube driving voltage   u16   0.0V not display
  oData.wCntrlEqual = Swap2(buffer,130);    //   130 - 131   When the detected current is 0, the comparator initial value Control equalization corresponds to 1 equalization  u16   is not displayed
  oData.wEqualization = Swap4(buffer,132);  // 132 - 135   (1 - 32 bits corresponds to 1 - 32 string equalization) corresponds to bit 1 displays the color at the corresponding voltage  u32   
  oData.wSysLog = Swap4(buffer,136);        // 136 - 137   The system log is sent to the serial port data
}

boolean BmsDataRead(AntBmsData& oData, int iAvail)
{
  if (iAvail < BMS_DataSize  )
    return false;
    
  //DEBUGT(" iAvail", iAvail);
  uint8_t buffer[BMS_DataSize];
  while (iAvail >= BMS_DataSize)  // bms data might be misaligned
  {
    for (int i=0; i < sizeof(aBmsHead); i++)
    {
       iAvail--;
       if (aBmsHead[i] != (buffer[i] = Serial2.read()))
       {
          DEBUGT2(i,aBmsHead[i,HEX]) DEBUGLN2("not matching",buffer[i],HEX)
          break;
       }
    }
    //DEB("head found\t")

    iAvail -= BMS_DataSize-sizeof(aBmsHead);
    for (int i=sizeof(aBmsHead); i < BMS_DataSize; i++)
       buffer[i] = Serial2.read();
       
    uint16_t expected = 0;
    for (int i = sizeof(aBmsHead); i < BMS_DataSize - 2; i++) 
    {
      //Serial.print(buffer[i] < 16 ? " 0" : " "); Serial.print(buffer[i],HEX);
      expected += buffer[i];
    }
    //Serial.println();
    uint16_t checksum = (buffer[BMS_DataSize - 2] << 8) + buffer[BMS_DataSize - 1]; // big endian
    if (checksum != expected)
    {
      DEB("checksum failure: ") DEB2(expected,HEX) DEB(" != ") DEB2(checksum,HEX) DEB("\n")
      break;
    }
    
    BmsDataCopy(oData,buffer);
    return true;
  }
  while (Serial2.available())  Serial2.read(); // empty buffer :-/
  return false;  
}


void BmsDataLog(AntBmsData& o)
{
  DEBUGLN2("U [V]",o.fU,3)
  DEB("afU [V]:")
  for (int i=0; i<o.iBattEff; i++)  { DEB(" ") DEB2(o.afU[i],3)}
  DEB("\n")
  DEBUGLN2("I [A]",o.fI,3)
  DEBUGLN("iP [W]",o.iP)
  DEBUGLN("remain [%]",o.iRemain)
  DEBUGLN2("iCapacityAh [Ah]",o.fCapacityAh,3)
  DEBUGLN2("iRemainingAh [Ah]",o.fRemainingAh,3)
  DEBUGLN2("iCycleAh [Ah]",o.fCycleAh,3)
  DEBUGLN("iSeconds",o.iSeconds)
  DEB("aiTemp [°C]:")
  for (int i=0; i<5; i++){ DEB(" ") DEB(o.aiTemp[i])  }
  DEB("\n")


  DEBUGT("iCellMax",o.iCellMax)
  DEBUGLN2("fCellMax [V]",o.fCellMax,3)
  DEBUGT("iCellMin",o.iCellMin)
  DEBUGLN2("fCellMin [V]",o.fCellMin,3)
  DEBUGLN2("fCellAverage [V]",o.fCellAverage,3)
  DEBUGLN("iBattEff",o.iBattEff)

  DEBUGT("wMosCharge",o.wMosCharge)
  DEBUGT("wMosDischarge",o.wMosDischarge)
  DEBUGLN("wBalanced",o.wBalanced)
  
  DEBUGLN("iTireLength",o.iTireLength)
  DEBUGLN("iPulsesPerWeek",o.iPulsesPerWeek)
  DEBUGLN("wRelaySwitch",o.wRelaySwitch)

  DEBUGT2("fMosDisDS [V]",o.fMosDisDS,3)
  DEBUGT2("fMosDisG [V]",o.fMosDisG,3)
  DEBUGLN2("fMosChargeG [V]",o.fMosChargeG,3)
  
  DEBUGT("wCntrlEqual",o.wCntrlEqual)
  DEBUGLN2("wEqualization",o.wEqualization,BIN)
  DEBUGT("wSysLog",o.wSysLog)
  DEBUGLN2("wSysLog.Status",o.wSysLog &0b11111,BIN)
  DEBUGT("wSysLog.BatteryNumber",o.wSysLog &0b1111100000 >> 5)
  DEBUGT("wSysLog.SequentialOrder",o.wSysLog &0b111110000000000 >> 10)
  DEBUGLN("wSysLog.ChargeDischarge",o.wSysLog &0b1000000000000000 >> 15)
}
