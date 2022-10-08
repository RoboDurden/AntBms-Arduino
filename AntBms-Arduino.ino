#define _DEBUG

#define TIME_RequestBms 500   // milliseconds

#include "include.h"

void setup()
{
  Serial.begin(115200);
  DEB("Hello AntBms-Arduino :-)\n");
  Serial2.begin(19200, SERIAL_8N1, RXD2, TXD2);
}

void loop()
{
  iNow = millis();

  if (int iAvail = Serial2.available())
  {
    if (BmsDataRead(oData, iAvail))
    {
      DEBUGT("remain [%]",oData.iRemain) DEBUGT("iP [W]",oData.iP)   DEBUGT2("fCellMax [V]",oData.fCellMax,3) DEBUGLN2("fCellMin [V]",oData.fCellMin,3)
      //BmsDataLog(oData);
      BatteryUpdate(oBattery,oData);
      DEBUGT2("fE_out [Wh]",oBattery.fE_out,3) DEBUGT2("fE_in [Wh]",oBattery.fE_in,3) DEBUGLN2("fE [Wh]",oBattery.fE,3)
    }
  }

  if (iTimeRequest > iNow)
    return;
  iTimeRequest = iNow + TIME_RequestBms;
  Serial2.write(aBmsRequest, sizeof (aBmsRequest));
}
