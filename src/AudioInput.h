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
    AudioInput();
    ~AudioInput();
    void update();
    
    float status[4] = {0, 0, 0, 0};
};