//
//  Analyzer.cpp
//  DynaDash
//
//  Created by Lauren McCarthy on 8/17/14.
//
//

#include "Analyzer.h"

using namespace ofx;
using namespace IO;
using namespace ESCPOS;



void Analyzer::setup(string comPort) {

    
    curMode = OFF;
    smileThresh = 0.5; // pend, might need to be pp
    
    stats = vector<vector<int> >(4, vector<int>(3, 0)); // [person][dominance, interruptions, expression]
    
    
    // floating history
    talkHistoryMinutes = 3;
	talkTime = vector<float>(4, 0);
	talkRatio = vector<float>(4, 0);
	talkHistory = vector<list<float> >(4, list<float>());

    // session history
    interruptions = vector<vector<int> >(4, vector<int>(2, 0)); // [person][giving, receiving]
    totalTalkTime = vector<float>(4,0);
    totalSmileTime = vector<vector<float> >(4, vector<float>(4, 0)); // [person][person]
    
    
    // setup things
    audioInput.setup();
    expressionInput.setup();
    feedback.setup();
	debugFeedback.setup(215);
    
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
    
    // setup serial
    cout << "listening for serial data on port " << comPort << "\n";
    serial.setup(comPort.c_str(), 57600, 15, 0xFE, 0xFF);
    
    showDebug = true;
    reset();
}

void Analyzer::update() {
    
    handleSerialMessage(serial.update());

    
    if (curMode == PRACTICE || curMode == ANALYSIS) {
        float curUpdate = ofGetElapsedTimef();
        float elapsed = curUpdate - lastUpdate;
        lastUpdate = curUpdate;
        
        // update inputs
        audioInput.update();
        expressionInput.update();
        feedback.update(audioInput.normalizedVolume);
        debugFeedback.update(audioInput.normalizedVolume, expressionInput.status);
        
        // update talk history arrays and current time
        // update smile time
        talkHistoryTime.push_back(curUpdate);
        for(int i = 0; i < 4; i++) {
            float audioDuration = audioInput.curSpeaker == i ? elapsed : 0.0;
            talkHistory[i].push_back(audioDuration);
            talkTime[i] += audioDuration;
            
            // session history stats
            totalTalkTime[i] += audioDuration;
            if (expressionInput.status[i] > smileThresh) {
                float ind = audioInput.curSpeaker == -1 ? i : audioInput.curSpeaker; // stick it in self if no cur speaker
                totalSmileTime[i][ind] += elapsed;
            } else {
            }
            
            // check for interrupting
            if (audioInput.interrupting[i]) {
                interruptions[i][0] += 1;
            }
            
            
            // check for interrupted
            if (i == audioInput.curSpeaker) {
                for (int j=0; j<4; j++) {
                    if (j != i) {
                        int val = audioInput.interrupting[j] ? 1 : 0;
                        interruptions[i][1] += val;
                    }
                }
            }
        }

        // remove all time that is too old
        float talkHistorySeconds = talkHistoryMinutes * 60;
        while(curUpdate - talkHistoryTime.front() > talkHistorySeconds) {
            talkHistoryTime.pop_front();
            for(int i = 0; i < 4; i++) {
                talkTime[i] -= talkHistory[i].front();
                talkHistory[i].pop_front();
            }
        }

        // track talk time ratios
        float totalTalkTime = 0;
        for (int i=0; i<4; i++) {
            totalTalkTime += talkTime[i];
        }
        for (int i=0; i<talkTime.size(); i++) {
            talkRatio[i] = talkTime[i]/totalTalkTime;
        }
        
        // send stats
        for (int i=0; i<4; i++) {
            stats[i][0] = talkRatio[i];
            stats[i][1] = interruptions[i][0];
            stats[i][2] = expressionInput.status[i] > smileThresh ? 1 : 0;
        }
        serial.sendStats(stats);
    }
}

void Analyzer::handleSerialMessage(int msg) {
    if (msg != -1) {
        switch (msg) {
            case 0:
                ofLogNotice() << "Received message: begin practice session";
                setMode(PRACTICE);
                break;
            case 1:
                ofLogNotice() << "Received message: begin analysis session";
                setMode(ANALYSIS);
                break;
            case 4:
                ofLogNotice() << "Received message: end practice session";
                setMode(OFF);
                break;
            case 5:
                ofLogNotice() << "Received message: end analysis session";
                setMode(OFF);
                break;
            default:
                ofLogNotice() << "Received unknown message: " << msg;
                break;
        }
        
    }
}

void Analyzer::draw() {
    feedback.draw(expressionInput.status, audioInput.interrupting, talkRatio);
    
    // reset interruptions after drawn
    if (curMode == REMOTE_CONTROL) {
        for (int i=0; i<4; i++) {
            audioInput.interrupting[i] = 0;
        }
    }
    
	if (showDebug) {
		debugFeedback.draw(audioInput.normalizedVolume, audioInput.speaking);
	}
    //expressionInput.draw();
}

void Analyzer::reset() {

    for (int i=0; i<4; i++) {
        talkTime[i] = 0;
        talkRatio[i] = 0;
        talkHistory[i].clear();
    }
    
    // clear session history
    for (int i=0; i<4; i++) {
        interruptions[i][0] = 0;
        interruptions[i][1] = 0;
        totalTalkTime[i] = 0;
        for (int j=0; j<4; j++) {
            totalSmileTime[i][j] = 0;
        }
    }
    
    lastUpdate = ofGetElapsedTimef();
}

void Analyzer::setMode(int mode) {
    if (curMode == ANALYSIS && mode != ANALYSIS) {
        endRecording();
    }
    
    curMode = mode;
    reset();
    ofLogNotice() << "mode set to " << curMode;
    
}

void Analyzer::endRecording() {
    
    ofLogNotice() << "Processing end of conversation";
    
    // figure out most and least
    int mostInterrupting = -1;
    int mostInterruptingVal = 0;
    int leastInterrupting = -1;
    int leastInterruptingVal = 0;
    
    int mostInterrupted = -1;
    int mostInterruptedVal = 0;
    int leastInterrupted = -1;
    int leastInterruptedVal = 0;
    
    int mostSpeaking = -1;
    float mostSpeakingVal = 0;
    int leastSpeaking = -1;
    float leastSpeakingVal = 0;
    
    int mostSmiling = -1;
    float mostSmilingVal = 0;
    int leastSmiling = -1;
    float leastSmilingVal = 0;
    
    vector<float> individualTotalSmile = vector<float>(4, 0);
    float groupTotalTalk = 0;
    float groupTotalSmile = 0;
    
    for (int i=0; i<4; i++) {
        
        // interrupting
        if (interruptions[i][0] > mostInterruptingVal) {
            mostInterrupting = i;
            mostInterruptingVal = interruptions[i][0];
        }
        else if (interruptions[i][0] < leastInterruptingVal || leastInterrupting == -1) {
            leastInterrupting = i;
            leastInterruptingVal = interruptions[i][0];
        }
        
        // interrupted
        if (interruptions[i][1] > mostInterruptedVal) {
            mostInterrupted = i;
            mostInterruptedVal = interruptions[i][1];
        }
        else if (interruptions[i][1] < leastInterruptedVal || leastInterrupted == -1) {
            leastInterrupted = i;
            leastInterruptedVal = interruptions[i][1];
        }
        
        // talking
        if (totalTalkTime[i] > mostSpeakingVal) {
            mostSpeaking = i;
            mostSpeakingVal = totalTalkTime[i];
        }
        else if (totalTalkTime[i] < leastSpeakingVal || leastSpeaking == -1) {
            leastSpeaking = i;
            leastSpeakingVal = totalTalkTime[i];
        }
        groupTotalTalk += totalTalkTime[i];
        
        
        // smiling
        for (int j=0; j<4; j++) {
            individualTotalSmile[i] += totalSmileTime[i][j];
        }
        
        if (individualTotalSmile[i] > mostSmilingVal) {
            mostSmiling = i;
            mostSmilingVal = individualTotalSmile[i];
        }
        else if (individualTotalSmile[i] < leastSmilingVal || leastSmiling == -1) {
            leastSmiling = i;
            leastSmilingVal = individualTotalSmile[i];
        }
        
        groupTotalSmile += individualTotalSmile[i];
        
    }
    
    vector<string> labels = vector<string>(4, "");
    
    for (int i=0; i<1; i++) { //4; i++) {
        
        labels[0] = "PERSON A";
        labels[1] = "PERSON B";
        labels[2] = "PERSON C";
        labels[3] = "PERSON D";
        labels[i] = "YOU";
        
        ofLogNotice() << "You interrupted people " << interruptions[i][0] << " times";
        if (mostInterrupting != -1)
            ofLogNotice() << labels[mostInterrupting] << " interrupted the most (" << mostInterruptingVal << " times)";
        if (leastInterrupting != -1)
            ofLogNotice() << labels[leastInterrupting] << " interrupted the least (" << leastInterruptingVal << " times)";
        
        ofLogNotice() << "You were interrupted " << interruptions[i][1] << " times";
        if (mostInterrupted != -1)
            ofLogNotice() << labels[mostInterrupted] << " was interrupted the most (" << mostInterruptedVal << " times)";
        if (leastInterrupted != -1)
            ofLogNotice() << labels[leastInterrupted] << " was interrupted the least (" << leastInterruptedVal << " times)";
        
        if (groupTotalTalk > 0) {
            ofLogNotice() << "You spoke " << int(100*totalTalkTime[i]/groupTotalTalk) << "% of the time.";
            if (mostSpeaking != -1)
                ofLogNotice() << labels[mostSpeaking] << " talked the most (" << int(100*mostSpeakingVal/groupTotalTalk) << " % of the time)";
            if (leastSpeaking != -1)
                ofLogNotice() << labels[leastSpeaking] << " talked the least (" << int(100*leastSpeakingVal/groupTotalTalk) << " % of the time)";
        }
        
        if (groupTotalSmile > 0) {
            ofLogNotice() << "You were smiling " << int(100*individualTotalSmile[i]/groupTotalSmile) << "% of the time.";
            if (mostSmiling != -1)
                ofLogNotice() << labels[mostSmiling] << " smiled the most (" << int(100*mostSmilingVal/groupTotalSmile) << " % of the time)";
            if (leastSmiling != -1)
                ofLogNotice() << labels[leastSmiling] << " smiled the least (" << int(100*leastSmilingVal/groupTotalSmile) << " % of the time)";
        }
    }
    
    
//    for (int i=0; i<printers.size(); i++) {
//        
//        
//        printers[i].setInvert(true);
//        printers[i].println("Inverted");
//        printers[i].setInvert(false);
//        
//        printers[i].setUnderline(ofx::ESCPOS::BaseCodes::UNDERLINE_NORMAL);
//        printers[i].println("Normal Underline");
//        printers[i].setUnderline(ofx::ESCPOS::BaseCodes::UNDERLINE_THICK);
//        printers[i].println("Thick Underline");
//        printers[i].setUnderline(ofx::ESCPOS::BaseCodes::UNDERLINE_OFF);
//        
//        printers[i].setEmphasis(true);
//        printers[i].println("Emphasis");
//        printers[i].setEmphasis(false);
//        
//        printers[i].setDoubleStrike(true);
//        printers[i].println("Double Strike");
//        printers[i].setDoubleStrike(true);
//        
//        printers[i].setFont(ofx::ESCPOS::BaseCodes::FONT_A);
//        printers[i].println("FONT_A");
//        printers[i].setFont(ofx::ESCPOS::BaseCodes::FONT_B);
//        printers[i].println("FONT_B");
//        printers[i].setFont(ofx::ESCPOS::BaseCodes::FONT_C);
//        printers[i].println("FONT_C");
//        printers[i].setFont(ofx::ESCPOS::BaseCodes::FONT_A);
//        
//        printers[i].setColor(ofx::ESCPOS::BaseCodes::COLOR_1);
//        printers[i].println("COLOR_1");
//        printers[i].setColor(ofx::ESCPOS::BaseCodes::COLOR_2);
//        printers[i].println("COLOR_2");
//        printers[i].setColor(ofx::ESCPOS::BaseCodes::COLOR_1);
//        
//        printers[i].setUpsideDown(true);
//        printers[i].println("UPSIDE DOWN");
//        printers[i].setUpsideDown(false);
//        
//        printers[i].setCharacterSize(ofx::ESCPOS::BaseCodes::MAGNIFICATION_1X,
//                                     ofx::ESCPOS::BaseCodes::MAGNIFICATION_1X);
//        printers[i].println("MAGNIFICATION_1X");
//        printers[i].setCharacterSize(ofx::ESCPOS::BaseCodes::MAGNIFICATION_2X,
//                                     ofx::ESCPOS::BaseCodes::MAGNIFICATION_2X);
//        printers[i].println("MAGNIFICATION_2X");
//        printers[i].setCharacterSize(ofx::ESCPOS::BaseCodes::MAGNIFICATION_3X,
//                                     ofx::ESCPOS::BaseCodes::MAGNIFICATION_3X);
//        printers[i].println("MAGNIFICATION_3X");
//        printers[i].setCharacterSize(ofx::ESCPOS::BaseCodes::MAGNIFICATION_4X,
//                                     ofx::ESCPOS::BaseCodes::MAGNIFICATION_4X);
//        printers[i].println("MAGNIFICATION_4X");
//        printers[i].setCharacterSize(ofx::ESCPOS::BaseCodes::MAGNIFICATION_5X,
//                                     ofx::ESCPOS::BaseCodes::MAGNIFICATION_5X);
//        printers[i].println("MAGNIFICATION_5X");
//        printers[i].setCharacterSize(ofx::ESCPOS::BaseCodes::MAGNIFICATION_6X,
//                                     ofx::ESCPOS::BaseCodes::MAGNIFICATION_6X);
//        printers[i].println("MAGNIFICATION_6X");
//        printers[i].setCharacterSize(ofx::ESCPOS::BaseCodes::MAGNIFICATION_7X,
//                                     ofx::ESCPOS::BaseCodes::MAGNIFICATION_7X);
//        printers[i].println("MAGNIFICATION_7X");
//        printers[i].setCharacterSize(ofx::ESCPOS::BaseCodes::MAGNIFICATION_1X,
//                                     ofx::ESCPOS::BaseCodes::MAGNIFICATION_1X);
//        printers[i].println("MAGNIFICATION_1X");
//        
//        printers[i].setCharacterSmoothing(true);
//        printers[i].println("Smooth");
//        printers[i].setCharacterSmoothing(false);
//        
//        printers[i].setRotation(ofx::ESCPOS::BaseCodes::ROTATE_90_CW);
//        printers[i].println("ROTATE_90_CW");
//        printers[i].setRotation(ofx::ESCPOS::BaseCodes::ROTATE_OFF);
//        printers[i].println("ROTATE_OFF");
//        
//        printers[i].setAlign(ofx::ESCPOS::BaseCodes::TextAlignment(2));
//        printers[i].println("right_align");
//        printers[i].setAlign(ofx::ESCPOS::BaseCodes::TextAlignment(1));
//        printers[i].println("center_align");
//        printers[i].setAlign(ofx::ESCPOS::BaseCodes::TextAlignment(0));
//        printers[i].println("left_align");
//        
//        printers[i].setLineSpacing(0);
//        printers[i].println("Tight line spacing.");
//        printers[i].println("Tight line spacing.");
//        printers[i].println("Tight line spacing.");
//        printers[i].setDefaultLineSpacing();
//        
//        printers[i].println("Normal line spacing.");
//        printers[i].println("Normal line spacing.");
//        printers[i].println("Normal line spacing.");
//        printers[i].println("Normal line spacing.println");
//        printers[i].println("-------");
//        
//        printers[i].cut(ofx::ESCPOS::BaseCodes::CUT_PARTIAL);
//        printers[i].println("Partial cut.");
//        printers[i].cut(ofx::ESCPOS::BaseCodes::CUT_FULL);
//        printers[i].println("Full cut with 90 dots fed.");
//    }
    
    reset();
}


// remote control
void Analyzer::setDominance(float d0, float d1, float d2, float d3) {
    talkRatio[0] = d0;
    talkRatio[1] = d1;
    talkRatio[2] = d2;
    talkRatio[3] = d3;
}

void Analyzer::setInterruption(int i) {
    audioInput.interrupting[i] = true;
}
void Analyzer::setExpression(float expression) {
    expressionInput.status[0] = expression;
}


