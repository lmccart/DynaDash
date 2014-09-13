//
//  ExpressionInput.cpp
//  DynaDash
//
//  Created by Lauren McCarthy on 8/17/14.
//
//

#include "ExpressionInput.h"

void ExpressionInput::setup() {
	status = vector<float>(4, 0);
	status[0] = 0.3;
}

void ExpressionInput::update() {
    for (int i=0; i<status.size(); i++) {
        status[i] = ofClamp(status[i]+ofRandom(-0.01, 0.01), 0.05, 1.0);
    }
}
