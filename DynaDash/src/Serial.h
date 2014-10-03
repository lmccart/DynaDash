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
    void setup();
	void update();
    
private:
    
    ofSerial serial;
    char bytesRead[14];				// data from serial, we will be trying to read 14
    char bytesReadString[15];		// a string needs a null terminator, so we need 14 + 1 bytes
    
    char bytesToSend[14];
    bool sendBytes;

};