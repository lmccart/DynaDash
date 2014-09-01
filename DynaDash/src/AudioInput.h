//
//  AudioInput.h
//  DynaDash
//
//  Created by Lauren McCarthy on 8/17/14.
//
//
#include "ofMain.h"


class AudioInput {
    
public:
    void setup(ofBaseApp *app);
    void update();
    float status[4] = {0, 0, 0, 0};
    ofSoundStream soundStream;
    
private:
    float threshold;
    float minimumThreshold;
    float decayRate;
};