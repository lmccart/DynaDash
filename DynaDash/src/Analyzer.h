//
//  Analyzer.h
//  DynaDash
//
//  Created by Lauren McCarthy on 8/17/14.
//
//

#include "ofMain.h"
#include "ofxESCPOS.h"
#include "Feedback.h"
#include "DebugFeedback.h"
#include "AudioInput.h"
#include "ExpressionInput.h"

class Analyzer {
    
public:
    
    void setup();
    void update();
    void draw();
    void reset();
    void setMode(int mode);
    
    // remote control
    void setDominance(float d0, float d1, float d2, float d3);
    void setInterruption(int i);
    void setExpression(float expression);
    
    enum {
        TRAINING = 0,
        RECORDING = 1,
        REMOTE_CONTROL = 2
    };
    int curMode;
    
    AudioInput audioInput;
    ExpressionInput expressionInput;
    Feedback feedback;
	DebugFeedback debugFeedback;
	bool showDebug;
	float talkHistoryMinutes;

private:
    float lastUpdate;
    vector<ofx::ESCPOS::DefaultSerialPrinter> printers;
    
    // floating history
	list<float> talkHistoryTime;
	vector< list<float> > talkHistory;
    vector<float> talkTime;
    vector<float> talkRatio;
    
    // session history
    float smileThresh;
    vector< vector<int> > interruptions; // [person][giving, receiving]
    vector<float> totalTalkTime;
    vector< vector<float> > totalSmileTime; // [person][person]
    
    void beginRecording();
    void endRecording();
};


