//
//  FaceInput.cpp
//  DynaDash
//
//  Created by Lauren McCarthy on 8/17/14.
//
//

#include "FaceInput.h"

void FaceInput::setup() {
    const float minTimeFace = .1; // .1s until a face is recognized
    const float maxTimeNoFace = 10; // 10s until the face is not present
    
    faceHysteresis = vector<Hysteresis>(4);
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
                faceHysteresis[i].setDelay(minTimeFace, maxTimeNoFace);
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

void FaceInput::update() {
    for (int i=0; i<camsInited; i++) {
        cams[i].update();
        if(cams[i].isFrameNew()) {
            smiles[i].update(cams[i]);
            bool faceFound = smiles[i].getFaceFound();
            if(faceFound) {
                float cur = smiles[i].getSmileAmount();
				//maxStatus[i] = max(maxStatus[i], cur);
                status[i] = ofLerp(status[i], cur/maxStatus[i], 0.2);
            }
            faceHysteresis[i].update(faceFound);
            if(faceHysteresis[i].wasTriggered()) {
                ofLogNotice("FaceInput") << "Face " << i << " entered.";
            }
            if(faceHysteresis[i].wasUntriggered()) {
                ofLogNotice("FaceInput") << "Face " << i << " exited.";
            }
        }
    }
}

void FaceInput::draw() {
    ofPushMatrix();
    ofScale(0.25, 0.25);
    for (int i=0; i<camsInited; i++) {
        cams[i].draw(0, 0);
        smiles[i].draw();
        ofTranslate(200, 0);
    }
    ofPopMatrix();
}

void FaceInput::reset() {
	for (int i=0; i<4; i++) {
		faceHysteresis[i].set(false);
	}
}

vector<bool> FaceInput::detectFaces() {
    vector<bool>detected = vector<bool>(4, false);
    for (int i=0; i<4; i++) {
        detected[i] = faceHysteresis[i].get();
    }
    return detected;
}