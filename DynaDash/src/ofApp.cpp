#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup() {

    ofShowCursor();

    ofSetVerticalSync(true);
    ofSetFrameRate(120);
	ofEnableSmoothing();
    
    analyzer.setup();
    
    // listen on the given port
    cout << "listening for osc messages on port " << PORT << "\n";
    receiver.setup(PORT);
    
    gui = new ofxUISuperCanvas("DEBUG VIEW");
    //gui->addLabelToggle("RECORDING", false);
    remoteControlLabel = gui->addLabel("REMOTE CONTROL OFF");
	gui->setWidth(215);
    
    gui->addLabelToggle("RECORDING", false);
    gui->autoSizeToFitWidgets();
    ofAddListener(gui->newGUIEvent, this, &ofApp::guiEvent);
    
	// audio stuff
	gui->addSlider("AUDIO_SMOOTH_AMT", 0, 1.0, 0.2);
	gui->addSlider("AUDIO_THRESH", 0, 1.0, 0.5);
	gui->addSlider("AUDIO_HISTORY", 1.0, 10.0, 3.0);

	gui->autoSizeToFitWidgets();
    ofAddListener(gui->newGUIEvent,this,&ofApp::guiEvent);
    
    initSettings();
}

void ofApp::initSettings() {
	gui->loadSettings("guiSettings.xml");
	analyzer.audioInput.setSmoothing(((ofxUISlider *)gui->getWidget("AUDIO_SMOOTH_AMT"))->getValue());
	analyzer.audioInput.speakingNormalizedThresh = ((ofxUISlider *)gui->getWidget("AUDIO_THRESH"))->getValue();
	analyzer.talkHistoryMinutes = ((ofxUISlider *)gui->getWidget("AUDIO_HISTORY"))->getValue();
}

//--------------------------------------------------------------
void ofApp::update() {
    analyzer.update();
    
    
    // check for waiting messages
    while(receiver.hasWaitingMessages()){
        
        // get the next message
        ofxOscMessage m;
        receiver.getNextMessage(&m);
        
        if(m.getAddress() == "/control_toggle"){
            int toggle = m.getArgAsInt32(0);
            ofLogNotice() << "received toggle " << toggle;
            if (toggle) {
                remoteControlLabel->setLabel("REMOTE CONTROL ON");
                analyzer.setMode(Analyzer::REMOTE_CONTROL);
            } else {
                remoteControlLabel->setLabel("REMOTE CONTROL OFF");
                analyzer.setMode(Analyzer::TRAINING);
            }
        }
        else if(m.getAddress() == "/expression"){
            float val = m.getArgAsFloat(0);
            ofLogNotice() << "received expression " << val;
            analyzer.setExpression(val);
        }
        else if(m.getAddress() == "/dominance"){
            float d0 = m.getArgAsFloat(0);
            float d1 = m.getArgAsFloat(1);
            float d2 = m.getArgAsFloat(2);
            float d3 = m.getArgAsFloat(3);
            ofLogNotice() << "received dominance " << d0 << " " << d1 << " " << d2 << " " << d3;
            analyzer.setDominance(d0, d1, d2, d3);
        }
        else if(m.getAddress() == "/interruption"){
            float i = m.getArgAsInt32(0);
            ofLogNotice() << "received interruption " << i;
            analyzer.setInterruption(i);
        }
        
    }
}

//--------------------------------------------------------------
void ofApp::draw() {
    analyzer.draw();

}

//--------------------------------------------------------------
void ofApp::keyPressed(int key) {
    if (key == ' ') {
        //analyzer.setMode(!analyzer.curMode);
		analyzer.showDebug = !analyzer.showDebug;
		ofLogNotice() << "debug view: " << analyzer.showDebug;

		gui->setVisible(analyzer.showDebug);

    }
	ofLogNotice() << "key pressed: " << key;
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
	
    gui->saveSettings("guiSettings.xml"); 
    delete gui;
}

void ofApp::guiEvent(ofxUIEventArgs &e) {
    if(e.getName() == "RECORDING") {
        ofxUILabelToggle *toggle = (ofxUILabelToggle *) e.widget;
        analyzer.setMode(toggle->getValue());
    } else if (e.getName() == "AUDIO_SMOOTH_AMT") {
		ofxUISlider *slider = (ofxUISlider *)e.widget;
		analyzer.audioInput.setSmoothing(slider->getValue());
		ofLogNotice() << "audio smooth amt set to: " << slider->getValue();
	} else if (e.getName() == "AUDIO_THRESH") {
		ofxUISlider *slider = (ofxUISlider *)e.widget;
		analyzer.audioInput.speakingNormalizedThresh = slider->getValue();
		ofLogNotice() << "audio speaking norm thresh set to: " << analyzer.audioInput.speakingNormalizedThresh;
	} else if (e.getName() == "AUDIO_HISTORY") {
		ofxUISlider *slider = (ofxUISlider *)e.widget;
		analyzer.talkHistoryMinutes = slider->getValue();
		ofLogNotice() << "audio history minutes set to: " << analyzer.talkHistoryMinutes;
	}
    //    } else if (e.getSlider() == interruptionSlider) {
    //
    //    } else if (e.getSlider() == expressionSlider) {
    //
    //    }
}
