//
//  ExpressionInput.h
//  DynaDash
//
//  Created by Lauren McCarthy on 8/17/14.
//
//

#include "ofMain.h"
#include "SmileDetector.h"

class ExpressionInput {
    
public:
    void setup();
    void update();
    void draw();
    vector<float> status;
    
    vector<ofVideoGrabber> cams;
    vector<SmileDetector> smiles;
    
    int camsInited;
    
};