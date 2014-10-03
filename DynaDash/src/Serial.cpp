//
//  Serial.cpp
//  DynaDash
//
//  Created by Lauren McCarthy on 10/1/14.
//
//

#include "Serial.h"

void Serial::setup() {
    
    serial.listDevices();
    vector <ofSerialDeviceInfo> deviceList = serial.getDeviceList();
    
    int baud = 57600;
    serial.setup(0, 57600); //open the first device and talk to it at 57600 baud
    memset(bytesReadString, 0, 15);
    sendBytes = false;
}

void Serial::update() {
    if (sendBytes) {
        serial.writeByte(0xFE); // begin
        for (int i=0; i<14; i++) {
            serial.writeByte(bytesToSend[i]);
        }
        serial.writeByte(0xFF); // end
        
        sendBytes = false;
    }
    
    unsigned char bytesReturned[14];
    
    memset(bytesReadString, 0, 15);
    memset(bytesReturned, 0, 14);
    
//    while( (nRead = serial.readBytes( bytesReturned, 3)) > 0){
//        nTimesRead++;
//        nBytesRead = nRead;
//    };
//    
    memcpy(bytesReadString, bytesReturned, 14);
}

