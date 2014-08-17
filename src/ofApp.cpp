#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup() {

    ofShowCursor();
//    gui = new ofxUICanvas(0, 0, ofGetWidth(), ofGetHeight());           // create a canvas at (0,0) using the default width
//    gui->addLabelToggle("RECORDING", false);
//    gui->autoSizeToFitWidgets();
//    ofAddListener(gui->newGUIEvent, this, &ofApp::guiEvent);

	ofSetVerticalSync(true);
	ofEnableSmoothing();
	
    
    gui = new ofxUISuperCanvas("CONTROLS");
    gui->addLabelToggle("RECORDING", false);
    gui->autoSizeToFitWidgets();
    ofAddListener(gui->newGUIEvent,this,&ofApp::guiEvent);
    gui->loadSettings("guiSettings.xml");
}

//--------------------------------------------------------------
void ofApp::update() {
    analyzer.update();
}

//--------------------------------------------------------------
void ofApp::draw() {
    analyzer.draw();

}

//--------------------------------------------------------------
void ofApp::keyPressed(int key) {
    if (key == 32) {
        analyzer.setMode(!analyzer.curMode);
    }
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
    delete gui;
}

void ofApp::guiEvent(ofxUIEventArgs &e) {
    if(e.getName() == "RECORDING") {
        ofxUILabelToggle *toggle = (ofxUILabelToggle *) e.widget;
        analyzer.setMode(toggle->getValue());
    }
    //    } else if (e.getSlider() == interruptionSlider) {
    //
    //    } else if (e.getSlider() == expressionSlider) {
    //
    //    }
}
