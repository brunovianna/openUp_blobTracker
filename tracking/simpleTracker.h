#pragma once

#include "baseTracker.h"
//#include "maskMaker.h"

class SimpleTracker : public BaseTracker{

	public:

		SimpleTracker();

		void setup(int width, int height);
		void update(ofxCvColorImage & colorImgFromCam);
		void draw(float x, float y);

		void setupGui();
		void updateGui();


//		MaskMaker					masker;

		ofxCvColorImage				colorImg;
		ofxCvGrayscaleImage			grayImgPreModification;
		ofxCvGrayscaleImage			grayBgImg;
		ofxCvGrayscaleImage			grayImg;
		ofxCvGrayscaleImage			threshImg;

		ofxCvContourFinder			contourFinder;


		int minSize, maxSize;


		float		w, h;
		float		threshold;


};

