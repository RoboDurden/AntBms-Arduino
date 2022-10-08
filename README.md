# AntBms-Arduino
implementation of the Ant BMS UART protocol for the Arudion IDE :-)

Ant Bms UART protocol thanks to   // https://github.com/klotztech/VBMS/wiki/Serial-protocol


https://github.com/syssi/esphome-ant-bms :
```
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
```
  
The connector is a 4 Pin JST 1.25mm. It's important to connect VCC too because the BMS doesn't respond/start if you connect TXD, RXD and GND only.

Supported devices

    ANT-BLE16ZMUB (7-16S, 320A, 2021-11-26) // Robo Durden, who wrote this arduino code
    16ZMB-TB-7-16S-300A (7-16S, 300A, 2021-08-12)
    24AHA-TB-24S-200A (10-24S, 200A, 2021-09-28)
    ANT 16S 100A (16S, 100A, 2020)
    ANT 24S 200A (8-24S, 200A, 2020)
   

debug output on ESP32:

```
U [V]: 52.500
afU [V]: 3.280 3.280 3.280 3.281 3.284 3.284 3.284 3.285 3.284 3.282 3.285 3.284 3.285 3.285 3.285 3.284
I [A]: 1.400
iP [W]: 63
remain [%]: 91
iCapacityAh [Ah]: 280.000
iRemainingAh [Ah]: 251.132
iCycleAh [Ah]: 4169.005
iSeconds: 24404754
aiTemp [°C]: 14 14 13 17 -40
iCellMax: 8	fCellMax [V]: 3.285
iCellMin: 1	fCellMin [V]: 3.280
fCellAverage [V]: 3.283
iBattEff: 16
wMosCharge: 1	wMosDischarge: 1	wBalanced: 0
iTireLength: 0
iPulsesPerWeek: 0
wRelaySwitch: 0
fMosDisDS [V]: 0.000	fMosDisG [V]: 13.100	fMosChargeG [V]: 12.500
wCntrlEqual: 527	wEqualization: 0
wSysLog: 7239	wSysLog.Status: 111
wSysLog.BatteryNumber: 7	wSysLog.SequentialOrder: 7	wSysLog.ChargeDischarge: 1
```
   
