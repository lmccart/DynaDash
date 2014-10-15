//
//  Serial.cpp
//  DynaDash
//
//  Created by Lauren McCarthy on 10/1/14.
//  Serial module for handling all low level serial transmission and
//  data validation.  The serial transmission segment consists of a
//  leading byte, then the data bytes, and ends with a trailing byte.
//   Data validation basically extract data out of this pattern only. 
//

#include "Serial.h"

// Private constants
const bool  IS_FLUSH_LENGTHY_LEAD = true;   // true if flushing lengthy leading bytes, longer than may be two packets of bytesInSegment
// shouldn't have lengthy lead, but in case for what even reasons, the serial buffer is queued
// up, hence making long delay in servicing of serial packets

// Private vars
int             bytesInSegment;             // number of bytes in a single serial transmission segment
unsigned char   leadByte;                   // leading byte in segment
unsigned char   trailByte;                  // trailing byte in segment
unsigned char   *dataBuffer;                // buffer array of data bytes read in (dynamic allocation)
int 			bytesInBuffer;				// number of bytes read into data buffer
bool            hasGoodData;                // true if good data pending retrieval



void Serial::setup(const char *portName, int baudRate, int nBytesInSegment,
                       unsigned char begByte, unsigned char endByte) {
    
    hasGoodData = false;
    bytesInBuffer = 0;
    
    bytesInSegment = nBytesInSegment;
    leadByte = begByte;
    trailByte = endByte;
    dataBuffer = new unsigned char(bytesInSegment);  // dynamically allocate the dataBuffer array
    
    serialPort.listDevices();       // list serial port info to command console
    
    serialPort.setup(portName, baudRate);
}


int Serial::update() {
    int msgTypeByte;
    
    if (serialHasGoodData()) {
        
        msgTypeByte = readSerialByte(1);
        confirmSerialDataProcessingComplete();
        
        ofLogNotice() << "type " << msgTypeByte;
        
        //        if ((tmpByte1=='H') && (tmpByte2=='I'))
        //            goodDataCount++;
        return msgTypeByte;
    }
    return -1;
}

void Serial::sendStats(vector< vector<int> > stats) {
    writeSerialByte(0xFE);
    writeSerialByte(3);
    for (int i=0; i<stats.size(); i++) {
        for (int j=0; j<stats[i].size(); j++) {
            writeSerialByte(stats[i][j]);
        }
    }
    writeSerialByte(0xFF);
}

void Serial::sendParticipants(vector<bool> detected) {
    writeSerialByte(0xFE);
    writeSerialByte(2);
    for (int i=0; i<4; i++) {
        writeSerialByte(int(detected[i]));
    }
    for (int i=0; i<8; i++) {
        writeSerialByte(0);
    }
    writeSerialByte(0xFF);
}


void Serial::close() {
    serialPort.flush();
    serialPort.close();
}



/*  Read serial port, validate data segment, and return true if data segment is good and ready to be retrieved. */
bool Serial::serialHasGoodData(void) {
    unsigned char   flushByte;      // dummy byte to flush lengthy leading bytes
    unsigned int    flushCount;     // number of bytes to flush
    unsigned int    i;
    
    int bytesReadInThisSession;     // number of bytes read from serial port during this session, for timing out purpose
    
    bytesReadInThisSession = 0;
    
    if (hasGoodData)    // Has unprocessed good data from last read session, return and not going to read in new data set.
        return true;    // Need confirmSerialDataProcessingComplete() to signal completion of the data processing.
    
    if (IS_FLUSH_LENGTHY_LEAD && (serialPort.available() > (2 * bytesInSegment)))    // flush away lengthy leading bytes queued up
    {
        bytesInBuffer = 0;      // ignore whatever still left in dataBuffer[]
        
        flushCount = serialPort.available() - (2 * bytesInSegment) + 1;
        for (i=0; i<flushCount; i++)
        {
            flushByte = serialPort.readByte();      // read a byte and do nothing (hence flush)
        }
    }
    
    while ((serialPort.available() > 0) && (bytesInBuffer < bytesInSegment))
    {
        dataBuffer[bytesInBuffer] = serialPort.readByte();    // read in data from serial port bytewise
        bytesReadInThisSession++;
        bytesInBuffer++;
        
        if (bytesInBuffer==1 && dataBuffer[0]!=leadByte)      // bad leading byte
            bytesInBuffer = 0;
        
        if (bytesReadInThisSession > (bytesInSegment * 4))    // Time out and return control back to the calling routine
            return false;                                     // if many incoming bytes are not matching our desired pattern
        // Or else will keep looping here and lock up the app.
    }
    
    if (bytesInBuffer<bytesInSegment)
        return false;                                         // not enough data bytes yet
    else
    {
        if (dataBuffer[bytesInSegment-1] != trailByte)        // bad data segment
        {
            bytesInBuffer = 0;                                // discard all bytes in buffer
            return false;
        }
        else
        {
            hasGoodData = true;
            return true;
        }
    }
}


/*  To allow good serial data to be read bytewise by other applications.  When done reading all data bytes,
 need to call confirmSerialDataProcessingComplete() to signal the completion of serial data processing.  */

unsigned char Serial::readSerialByte(int byteNo) {
    if ((byteNo < 0) || (byteNo >= bytesInSegment))          // prevent index to array out of range
        return 0;
    else
        return dataBuffer[byteNo];
}



/*  Write a byte to the serial port */
bool Serial::writeSerialByte(unsigned char aByte) {
    return serialPort.writeByte(aByte);
}


/*  To confirm that last good serial data set has been processed by main application,
 or else serialHasGoodData() won't read in and validate new set of data.     */
void Serial::confirmSerialDataProcessingComplete(void) {
    hasGoodData = false;
    bytesInBuffer = 0;
}
