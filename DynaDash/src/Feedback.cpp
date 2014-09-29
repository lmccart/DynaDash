//
//  Feedback.cpp
//  DynaDash
//
//  Created by Lauren McCarthy on 8/17/14.
//
//

#include "Feedback.h"

using namespace ofx;
using namespace IO;
using namespace ESCPOS;


void Feedback::setup() {
	ledColor = ofColor::yellow;
	ledWarnColor = ofColor::red;
	ledAlpha = vector<float>(4, 255);
	ledWarnAlpha = vector<float>(4, 255);
	ledCenters = vector<ofVec2f>(4);
	ledCenters[0] = ofVec2f(0, 300);
	ledCenters[1] = ofVec2f(-100, 200);
	ledCenters[2] = ofVec2f(0, 100);
	ledCenters[3] = ofVec2f(100, 200);
	easing = 0.1;

    audioPlot.historyLength = 100;
    audioPlot.addColor(ofColor::fromHex(0xF4C341));
    audioPlot.addColor(ofColor::fromHex(0x05A180));
    audioPlot.addColor(ofColor::fromHex(0xE25D48));
    audioPlot.addColor(ofColor::fromHex(0x055D48));
    
    // setup printers
    
    printers = vector<DefaultSerialPrinter>(4);
    vector<SerialDeviceInfo> printDevices = SerialDeviceUtils::listDevices();
    
    if (printDevices.size() < 4) {
        ofLogError() << "Detected " << printDevices.size() << " printers instead of 4.";
    }
    
    for (int i=0; i<printDevices.size(); i++) {
//        //printDevices[i].getDescription();
//        if (!printers[i].setup(printDevices[i].getPort(),
//                               38400,
//                               SerialDevice::DATA_BITS_EIGHT,
//                               SerialDevice::PAR_NONE,
//                               SerialDevice::STOP_ONE,
//                               SerialDevice::FLOW_CTRL_HARDWARE)) {
//            ofLogError("ofApp::setup") << "Unable to connect to: " << printDevices[i].getPort();
//        }
//        
//        
//        // Set up hardware flow control if needed.
//        printers[i].setDataTerminalReady();
//        printers[i].setRequestToSend();
//
//        // Initialize the printer.
//        printers[i].initialize();
        ofLogNotice() << printDevices[i].getDescription() << " " << printDevices[i].getHardwareId() << " " << printDevices[i].getPort();
    }
}

void Feedback::update(vector<float>volume) {
	audioPlot.addData(volume);
}

void Feedback::draw(vector<float> expression, vector<bool> interrupting, vector<float> dominance) {
    
    ofBackground(ofColor(40));

    ofPushMatrix();
	ofTranslate(425, 0);
	
    // facial expression dial
    drawArc(expression[0]);

	ofTranslate(375, 0);

    // DOMINANCE/SPEAKING LED DISPLAY
	ofPushStyle();
	ofSetLineWidth(2.5);
    for (int i=0; i<4; i++) {
        float nextAlpha = 255*dominance[i];
        ofSetColor(ledColor[0], ledColor[1], ledColor[2], 100);
        ofNoFill();
        ofCircle(ledCenters[i].x, ledCenters[i].y, 25);

        ofSetColor(ledColor[0], ledColor[1], ledColor[2], ofLerp(ledAlpha[i], nextAlpha, easing));
        ofFill();
        ledAlpha[i] = nextAlpha;
        ofCircle(ledCenters[i].x, ledCenters[i].y, 25);

		float warnAlpha = 0;
		if (interrupting[i]) {
			warnAlpha = 255.0;
		} else {
			warnAlpha = ofLerp(ledWarnAlpha[i], 0, easing);
		}
		ledWarnAlpha[i] = warnAlpha;
		ofSetColor(ledWarnColor[0], ledWarnColor[1], ledWarnColor[2], warnAlpha);
		ofFill();
		ofCircle(ledCenters[i].x, ledCenters[i].y, 25);
    }
	ofPopStyle();
	
    
    ofPopMatrix();

}

void Feedback::drawArc(float amt) {
	ofPushStyle();
    float unit = ofGetWidth()*0.15;
    float h = ofGetHeight()*0.5;
	ofPoint p(0, h);
	ofColor blue(70, 70, 200);
	ofColor gray(100);

    ofPath expressionCurveBack;
	expressionCurveBack.setCurveResolution(100);
	expressionCurveBack.arc(p, unit, unit, -180, 0);
	expressionCurveBack.setFillColor(gray);
	expressionCurveBack.setFilled(true);
	expressionCurveBack.draw();
	
    ofPath expressionCurve;
	expressionCurve.setCurveResolution(100);
	expressionCurve.arc(p, unit, unit, -180, ofClamp(180*amt-180, -178, 0));
	ofColor c(255, 0,0);
	expressionCurve.setFillColor(blue);
	expressionCurve.setFilled(true);
	expressionCurve.draw();

	ofPopStyle();
}

void Feedback::printResults() {
    for (int i=0; i<printers.size(); i++) {
        printers[i].setInvert(true);
        printers[i].println("Inverted");
        printers[i].setInvert(false);
        
        printers[i].setUnderline(ofx::ESCPOS::BaseCodes::UNDERLINE_NORMAL);
        printers[i].println("Normal Underline");
        printers[i].setUnderline(ofx::ESCPOS::BaseCodes::UNDERLINE_THICK);
        printers[i].println("Thick Underline");
        printers[i].setUnderline(ofx::ESCPOS::BaseCodes::UNDERLINE_OFF);
        
        printers[i].setEmphasis(true);
        printers[i].println("Emphasis");
        printers[i].setEmphasis(false);
        
        printers[i].setDoubleStrike(true);
        printers[i].println("Double Strike");
        printers[i].setDoubleStrike(true);
        
        printers[i].setFont(ofx::ESCPOS::BaseCodes::FONT_A);
        printers[i].println("FONT_A");
        printers[i].setFont(ofx::ESCPOS::BaseCodes::FONT_B);
        printers[i].println("FONT_B");
        printers[i].setFont(ofx::ESCPOS::BaseCodes::FONT_C);
        printers[i].println("FONT_C");
        printers[i].setFont(ofx::ESCPOS::BaseCodes::FONT_A);
        
        printers[i].setColor(ofx::ESCPOS::BaseCodes::COLOR_1);
        printers[i].println("COLOR_1");
        printers[i].setColor(ofx::ESCPOS::BaseCodes::COLOR_2);
        printers[i].println("COLOR_2");
        printers[i].setColor(ofx::ESCPOS::BaseCodes::COLOR_1);
        
        printers[i].setUpsideDown(true);
        printers[i].println("UPSIDE DOWN");
        printers[i].setUpsideDown(false);
        
        printers[i].setCharacterSize(ofx::ESCPOS::BaseCodes::MAGNIFICATION_1X,
                                 ofx::ESCPOS::BaseCodes::MAGNIFICATION_1X);
        printers[i].println("MAGNIFICATION_1X");
        printers[i].setCharacterSize(ofx::ESCPOS::BaseCodes::MAGNIFICATION_2X,
                                 ofx::ESCPOS::BaseCodes::MAGNIFICATION_2X);
        printers[i].println("MAGNIFICATION_2X");
        printers[i].setCharacterSize(ofx::ESCPOS::BaseCodes::MAGNIFICATION_3X,
                                 ofx::ESCPOS::BaseCodes::MAGNIFICATION_3X);
        printers[i].println("MAGNIFICATION_3X");
        printers[i].setCharacterSize(ofx::ESCPOS::BaseCodes::MAGNIFICATION_4X,
                                 ofx::ESCPOS::BaseCodes::MAGNIFICATION_4X);
        printers[i].println("MAGNIFICATION_4X");
        printers[i].setCharacterSize(ofx::ESCPOS::BaseCodes::MAGNIFICATION_5X,
                                 ofx::ESCPOS::BaseCodes::MAGNIFICATION_5X);
        printers[i].println("MAGNIFICATION_5X");
        printers[i].setCharacterSize(ofx::ESCPOS::BaseCodes::MAGNIFICATION_6X,
                                 ofx::ESCPOS::BaseCodes::MAGNIFICATION_6X);
        printers[i].println("MAGNIFICATION_6X");
        printers[i].setCharacterSize(ofx::ESCPOS::BaseCodes::MAGNIFICATION_7X,
                                 ofx::ESCPOS::BaseCodes::MAGNIFICATION_7X);
        printers[i].println("MAGNIFICATION_7X");
        printers[i].setCharacterSize(ofx::ESCPOS::BaseCodes::MAGNIFICATION_1X,
                                 ofx::ESCPOS::BaseCodes::MAGNIFICATION_1X);
        printers[i].println("MAGNIFICATION_1X");
        
        printers[i].setCharacterSmoothing(true);
        printers[i].println("Smooth");
        printers[i].setCharacterSmoothing(false);
        
        printers[i].setRotation(ofx::ESCPOS::BaseCodes::ROTATE_90_CW);
        printers[i].println("ROTATE_90_CW");
        printers[i].setRotation(ofx::ESCPOS::BaseCodes::ROTATE_OFF);
        printers[i].println("ROTATE_OFF");
        
        printers[i].setAlign(ofx::ESCPOS::BaseCodes::TextAlignment(2));
        printers[i].println("right_align");
        printers[i].setAlign(ofx::ESCPOS::BaseCodes::TextAlignment(1));
        printers[i].println("center_align");
        printers[i].setAlign(ofx::ESCPOS::BaseCodes::TextAlignment(0));
        printers[i].println("left_align");
        
        printers[i].setLineSpacing(0);
        printers[i].println("Tight line spacing.");
        printers[i].println("Tight line spacing.");
        printers[i].println("Tight line spacing.");
        printers[i].setDefaultLineSpacing();
        
        printers[i].println("Normal line spacing.");
        printers[i].println("Normal line spacing.");
        printers[i].println("Normal line spacing.");
        printers[i].println("Normal line spacing.println");
        printers[i].println("-------");
        
        printers[i].cut(ofx::ESCPOS::BaseCodes::CUT_PARTIAL);
        printers[i].println("Partial cut.");
        printers[i].cut(ofx::ESCPOS::BaseCodes::CUT_FULL);
        printers[i].println("Full cut with 90 dots fed.");
    }
}



