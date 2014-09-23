//
//  SmileDetector.cpp
//  DynaDash
//
//  single-person smile strength detector
//  not robust against rotation

#include "SmileDetector.h"

using namespace cv;
using namespace ofxCv;

void SmileDetector::setup() {
    faceFinder.setup("haarcascade_frontalface_default.xml");
    faceFinder.setPreset(ofxCv::ObjectFinder::Fast);
    faceFinder.setFindBiggestObject(true);
    smileFinder.setup("smiled_05.xml");
    smileFinder.setPreset(ofxCv::ObjectFinder::Sensitive);
    smileFinder.setMinNeighbors(0);
}

void SmileDetector::update(ofVideoGrabber& img) {
    update(ofxCv::toCv(img));
}

void SmileDetector::update(const cv::Mat& mat) {
    faceFinder.update(mat);
    if(faceFinder.size()) {
        roi = faceFinder.getObject(0);
        float lowerRatio = .35;
        roi.y += roi.height * (1 - lowerRatio);
        roi.height *= lowerRatio;
        cv::Mat faceMat(mat, ofxCv::toCv(roi));
        smileFinder.update(faceMat);
    }
}
void SmileDetector::draw() const {
    faceFinder.draw();
    if(faceFinder.size()) {
        ofPushMatrix();
        ofTranslate(roi.position);
        smileFinder.draw();
        ofPopMatrix();
    }
}
bool SmileDetector::getFaceFound() const {
    return faceFinder.size();
}
ofRectangle SmileDetector::getFace() const {
    return faceFinder.getObject(0);
}
int SmileDetector::getSmileAmount() const {
    if(faceFinder.size()) {
        return smileFinder.size();
    } else {
        return 0;
    }
}
