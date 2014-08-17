//
//  Feedback.h
//  DynaDash
//
//  Created by Lauren McCarthy on 8/17/14.
//
//

#include "ofMain.h"
#include "ofxUI.h"


class Feedback {
    
public:
    Feedback();
    ~Feedback();
    void draw(float expression[4], float dominance[4]);
    
private:
    void drawArc(float amt);
};