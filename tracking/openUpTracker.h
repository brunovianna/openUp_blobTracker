#pragma once

#include "baseTracker.h"
#include "tuioServer.h"
//#include "maskMaker.h"
#include "ofxCvBlobTracker.h"
#include "ofxCvTrackedBlob.h"
#include "ofxCvConstants_Track.h"


#define MAX_USERS_HARDLIMIT	25

class OpenUpTracker : public BaseTracker{

	public:

		OpenUpTracker();

		void setup(int width, int height);
		void update(ofxCvColorImage & colorImgFromCam);
		void draw(float x, float y);

		void setupGui();
		void updateGui();

		void sendOsc(){};
		void receiveOsc(){};

		// blob tracking
		void blobOn( int x, int y, int id, int order );
		void blobMoved( int x, int y, int id, int order );
		void blobOff( int x, int y, int id, int order );

		//--- blob tracking
		int topLeftX;
		int topLeftY;
		int lowRightX;
		int lowRightY;
		int userOffsetX;
		int userOffsetY;
		int usersTracking[MAX_USERS_HARDLIMIT];
	//--

//		MaskMaker					masker;

		ofxCvColorImage				colorImg;
		ofxCvGrayscaleImage			grayImgPreModification;
		ofxCvGrayscaleImage			grayBgImg,grayBgImgMasked;
		ofxCvGrayscaleImage			grayImg;
		ofxCvGrayscaleImage			threshImg;

		ofxCvContourFinder			contourFinder;
		ofxCvBlobTracker			blobTracker;

		int minSize, maxSize;

		//tuio server
		tuioServer          TS;

		float		w, h;
		float		threshold;


};

