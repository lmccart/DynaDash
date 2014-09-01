//
//  Analyzer.h
//  DynaDash
//
//  Created by Lauren McCarthy on 8/17/14.
//
//

#include "ofMain.h"
#include "SQLiteC++.h"
#include "Feedback.h"
#include "AudioInput.h"
#include "VideoInput.h"

class Analyzer {
    
public:
    Analyzer();
    ~Analyzer();
    
    void setup();
    void update();
    void draw();
    void reset();
    void setMode(int mode);
    
    enum {
        TRAINING = 0,
        RECORDING = 1
    };
    bool curMode;
    
    AudioInput audioInput;
    VideoInput videoInput;
    Feedback feedback;
    SQLite::Database db;

private:
    float lastUpdate;
    float talkTime[4] = {0, 0, 0, 0};
    float talkRatio[4] = {0, 0, 0, 0};
};