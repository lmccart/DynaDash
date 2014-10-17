//
//  SerialCom.h
//  DynaDash
//
//  Created by Lauren McCarthy on 10/1/14.
//
//

#include "ofMain.h"

class SerialCom {
    
public:
    
    void setup(const char *portName, int baudRate, int nBytesInSegment, unsigned char begByte, unsigned char endByte);
    void sendStats(vector< vector<int> > stats);
    int update();
    bool serialHasGoodData(void);
    unsigned char readSerialByte(int byteNo);
    bool writeSerialByte(unsigned char aByte);
    void confirmSerialDataProcessingComplete(void);
    void close();
    
    
private:
    ofSerial	serialPort;
};
