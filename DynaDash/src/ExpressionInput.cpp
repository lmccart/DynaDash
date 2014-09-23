//
//  ExpressionInput.cpp
//  DynaDash
//
//  Created by Lauren McCarthy on 8/17/14.
//
//

#include "ExpressionInput.h"

void ExpressionInput::setup() {
	status = vector<float>(4, 0);
	maxStatus = vector<float>(4, 100.0);

    cams = vector<ofVideoGrabber>(4);
    smiles = vector<SmileDetector>(4);
    
    camsInited = 0;
    
    vector<ofVideoDevice> devices = cams[0].listDevices();
    
    for(int i = 0; i < devices.size(); i++){
        if (i < 4) {
            cout << devices[i].id << ": " << devices[i].deviceName;
            if( devices[i].bAvailable ){
                cout << endl;
                cams[i].setDeviceID(devices[i].id);
                cams[i].initGrabber(640, 480);
                smiles[i].setup();
                camsInited++;
            }else{
                cout << " - unavailable " << endl;
            }
        }
    }
    
    if(camsInited != 4) {
        ofLogError() << "Detected " << camsInited << " cameras instead of 4.";
    }
    
}

void ExpressionInput::update() {
    for (int i=0; i<camsInited; i++) {
        cams[i].update();
        if(cams[i].isFrameNew()) {
            smiles[i].update(cams[i]);
            if(smiles[i].getFaceFound()) {
                float cur = smiles[i].getSmileAmount();
				//maxStatus[i] = max(maxStatus[i], cur);
                status[i] = ofLerp(status[i], cur/maxStatus[i], 0.2);
            }
        }
    }
}

void ExpressionInput::draw() {
    ofPushMatrix();
    ofScale(0.25, 0.25);
    for (int i=0; i<camsInited; i++) {
        cams[i].draw(0, 0);
        smiles[i].draw();
        ofTranslate(200, 0);
    }
    ofPopMatrix();
}