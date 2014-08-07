#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup() {
    gui = new ofxUICanvas(0, 0, ofGetWidth(), ofGetHeight());           // create a canvas at (0,0) using the default width
    gui->setColorBack(ofColor(0, 0, 0, 0));
    
    float w = ofGetWidth()/4;
    dominanceSlider = new ofxUIRotarySlider(w,0.0,100,50.0,"DOMINANCE");
    sliders.push_back(dominanceSlider);
    interruptionSlider = new ofxUIRotarySlider(w,0.0,100,50.0,"INTERRUPTIONS");
    sliders.push_back(interruptionSlider);
    expressionSlider = new ofxUIRotarySlider(w,0.0,100,50.0,"EXPRESSION");
    sliders.push_back(expressionSlider);
    
    for (int i=0; i<sliders.size(); i++) {
        gui->addWidgetRight(sliders[i]);
        sliders[i]->setColorOutline(ofColor(255, 255, 255, 0));
        sliders[i]->setColorOutlineHighlight(ofColor(255, 255, 255, 0));
        sliders[i]->setColorFill(ofColor(255, 255, 255));
        sliders[i]->setColorBack(ofColor(100, 100, 100));
        sliders[i]->setPadding(20.0);
    }
    
    //gui->autoSizeToFitWidgets();
    gui->centerWidgetsOnCanvas();
    ofAddListener(gui->newGUIEvent, this, &ofApp::guiEvent);
    gui->loadSettings("settings.xml"); // pend, does anything need to be saved?
    
    ofShowCursor();
}

//--------------------------------------------------------------
void ofApp::update() {

}

//--------------------------------------------------------------
void ofApp::draw() {

}

//--------------------------------------------------------------
void ofApp::keyPressed(int key) {

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key) {

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ) {

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button) {

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button) {

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button) {

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h) {

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg) {

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo) { 

}

void ofApp::exit() {
    gui->saveSettings("settings.xml");
    delete gui;
}

void ofApp::guiEvent(ofxUIEventArgs &e) {
    if(e.getName() == "DOMINANCE") {
        ofxUISlider *slider = e.getSlider();
        ofBackground(slider->getScaledValue());
    }
//    } else if (e.getSlider() == interruptionSlider) {
//        
//    } else if (e.getSlider() == expressionSlider) {
//        
//    }
}
