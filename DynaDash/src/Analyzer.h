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
#include "FaceInput.h"
#include "SerialCom.h"

class Analyzer {
    
public:
    
    void setup(string comPort);
    void update();
    void draw();
    void reset();
    void setMode(int mode);
    
    // remote control
    void setDominance(float d0, float d1, float d2, float d3);
    void setInterruption(int i);
    void setExpression(float expression);
    
    enum {
        OFF = 0,
        PRACTICE = 1,
		DETECT = 2,
        ANALYSIS = 3,
        REMOTE_CONTROL = 4
    };
    int curMode;
    
    vector< vector<int> > stats;
    vector<bool> participantStatus;
    
    AudioInput audioInput;
    FaceInput faceInput;
    Feedback feedback;
	DebugFeedback debugFeedback;
	bool showDebug;
	float talkHistoryMinutes;
    

private:
	float modeStart;
    float lastUpdate;
	float detectDuration;

    ofx::ESCPOS::DefaultSerialPrinter printer;
    SerialCom serialCom;
	float lastSerialStatsTime;
    
    // floating history
	list<float> talkHistoryTime;
	vector< list<float> > talkHistory;
    vector<float> talkTime;
    vector<float> talkRatio;
    
    // session history
    float smileThresh;
    vector< vector<int> > interruptions; // [person][giving, receiving]
    vector<float> totalTalkTime;
    vector< vector<float> > personSmileTime; // [person][person]
	vector<float> totalSmileTime;
    
    void endAnalysisSession();
	string Analyzer::getTimes(int val);
    void handleSerialMessage(int msg);
	int detectParticipants(int mode);

	bool printerAttached;
};


