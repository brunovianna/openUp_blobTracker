#pragma once


#include "ofMain.h"
#include "ofxOpenCv.h"
#include "ofxCvGrayscaleAdvanced.h"
#include "ofxVectorMath.h"
#include "ofxControlPanel.h"



class BaseTracker{

	public:

		BaseTracker();

		virtual void setup(int width, int height){};
		virtual void update(ofxCvColorImage & colorImgFromCam){};
		virtual void draw(float x, float y){};


		virtual void sendOsc(){};
		virtual void receiveOsc(){};

		int w, h;
		int myId;
		string myName;

		ofxControlPanel panel;

};

