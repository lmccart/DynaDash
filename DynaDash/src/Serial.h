//
//  Serial.h
//  DynaDash
//
//  Created by Lauren McCarthy on 10/1/14.
//
//

#include "ofMain.h"

class Serial {
    
public:
    
    bool setup(const char *portName, int baudRate, int nBytesInSegment, unsigned char begByte, unsigned char endByte);
    void update();
    bool serialHasGoodData(void);
    unsigned char readSerialByte(int byteNo);
    bool writeSerialByte(unsigned char aByte);
    void confirmSerialDataProcessingComplete(void);
    void close();
    
    
private:
    ofSerial	serialPort;
};
