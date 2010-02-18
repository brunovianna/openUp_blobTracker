#pragma once

#include "baseTracker.h"


class MaskedTracker : public BaseTracker{

	public:

		MaskedTracker();
		
		void setup(int width, int height);
		void update(ofxCvColorImage & colorImgFromCam);
		void draw(float x, float y);
		
		void setupGui();
		void updateGui();
		
		void flip(bool bHorizontal, bool bVertical);
	
		ofxCvColorImage				colorImg;
		ofxCvGrayscaleImage			grayImgPreModification;
		ofxCvGrayscaleImage			grayBgImg;
		ofxCvGrayscaleAdvanced		grayImgPreWarp;
		ofxCvGrayscaleAdvanced		grayImg;
		ofxCvGrayscaleAdvanced		threshImg;
	
		ofxCvContourFinder			contourFinder;
	
		void						calculateEdgePixels();
		ofPoint						edgeMaskStartPos;
		float						edgeMaskInnerRadius;			// < innerRadius = white
		float						edgeMaskOuterRadius;			// > inner radius and < outer radius = fade to black
		unsigned char				* edgeMaskPixels;				// > outer radius = black.....
		ofxCvGrayscaleImage			edgeMask;
		ofxCvGrayscaleImage			edgeMaskInverted;				// for display, easier to see what's going on. 
	
	
		int minSize, maxSize;
	
		bool		bBgCapture;
		
		// filtering / options:
		bool		bUseGamma;
		float		gamma;
		
		bool		bUseContrast;
		float		contrast;
		float		brightness;
		
		bool		bUseDilate;
		int			nDilations;

		bool		flipX, flipY;
		
		float		w, h;
		float		threshold;
		

};

