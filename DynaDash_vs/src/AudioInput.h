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
    vector<float>status;
    ofSoundStream soundStream;
    AudioAnalyzer mics[4];
    
private:
    float threshold;
    float minimumThreshold;
    float decayRate;
};