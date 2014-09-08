#pragma once

#include "ofMain.h"

class StackedPlot {
private:
    list<vector<float> > history;
    vector<ofColor> colors;
public:
    int historyLength;
    
    StackedPlot();
    void clear();
    void addData(const vector<float>& data);
    void addColor(ofColor color);
    int size() const;
    int dimensions() const;
    void draw(float width, float height) const;

	static float sum(const vector<float>& x);
	static vector<float> divide(const vector<float>& x, float y);
	static vector<float> normalize(const vector<float>& x);
	static vector<float> accumulate(const vector<float>& x);
};