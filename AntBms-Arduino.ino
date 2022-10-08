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
      //DEBUGLN("iP [W]",oData.iP)
      BmsDataLog(oData);
    }
  }

  if (iTimeRequest > iNow)
    return;
  iTimeRequest = iNow + TIME_RequestBms;
  Serial2.write(aBmsRequest, sizeof (aBmsRequest));
}
