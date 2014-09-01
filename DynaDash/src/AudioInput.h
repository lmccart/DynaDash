//
//  AudioInput.h
//  DynaDash
//
//  Created by Lauren McCarthy on 8/17/14.
//
//
#include "ofMain.h"
#include "AudioAnalyzer.h"


class AudioInput {
    
public:
    void setup();
    void update();
    float status[4] = {0, 0, 0, 0};
    ofSoundStream soundStream;
    AudioAnalyzer mics[4];
    
private:
    float threshold;
    float minimumThreshold;
    float decayRate;
};