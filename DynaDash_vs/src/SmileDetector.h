//
//  SmileDetector.h
//  DynaDash
//
//  Created by Lauren McCarthy on 9/22/14.
//
//

#pragma once

#include "ofMain.h"
#include "ofxCv.h"

class SmileDetector {
protected:
    ofRectangle roi;
    
public:
    ofxCv::ObjectFinder faceFinder, smileFinder;
    
    void setup();
    void update(ofVideoGrabber& img);
    void update(const cv::Mat& mat);
    void draw() const;
    bool getFaceFound() const;
    ofRectangle getFace() const;
    int getSmileAmount() const;

};
