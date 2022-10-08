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
   
