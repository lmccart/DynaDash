//
//  FaceInput.h
//  DynaDash
//
//  Created by Lauren McCarthy on 8/17/14.
//
//

#include "ofMain.h"
#include "SmileDetector.h"

class FaceInput {
    
public:
    void setup();
    void update();
    void draw();
    vector<bool> detectFaces();
    vector<float> status;
	vector <float> maxStatus;
    
    vector<ofVideoGrabber> cams;
    vector<SmileDetector> smiles;
    
    int camsInited;
    
};