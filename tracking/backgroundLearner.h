/*
 *  backgroundLearner.h
 *  fachada_tracking
 *
 *  Created by Chris on 2/13/10.
 *  Copyright 2010 csugrue. All rights reserved.
 *
 */

#pragma once

#include "ofMain.h"
#include "ofxCvMain.h"

class BackgroundLearner{
	
	public:
	BackgroundLearner();
	~BackgroundLearner(){};
	
	void setup(int w, int h);
	void learnBackground( ofxCvGrayscaleImage graySrc );
	
	ofxCvGrayscaleImage grayImg;
	ofxCvFloatImage		floatImg;
	
};
