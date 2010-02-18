#include "simpleTracker.h"


//--------------------------------------------------------------
SimpleTracker::SimpleTracker(){

	myName = "simpleTracker";
}




//--------------------------------------------------------------
void SimpleTracker::setup(int width, int height){

	minSize = 10;
	maxSize = width*height*.75;


	w = width;
	h = height;

	colorImg.allocate(w, h);
	grayImgPreModification.allocate(w, h);
	grayImg.allocate(w, h);
	threshImg.allocate(w, h);
	grayBgImg.allocate(w, h);

	threshold			= 60;


	setupGui();

//	masker.setup(w,h,10,10,2);
}


//--------------------------------------------------------------
void SimpleTracker::update(ofxCvColorImage & colorImgFromCam){

	// update vars from control panel
	updateGui();

	// maybe not needed, slow?
	colorImg = colorImgFromCam;

	// preserve original
	grayImgPreModification = colorImgFromCam;

	grayImg = grayImgPreModification;
//	grayImg -= masker.mask;

	// bg  cap
	if(panel.getValueB("BG_CAPTURE") ){
		grayBgImg = grayImg;
		panel.setValueB("BG_CAPTURE", false);
	}

	threshImg.absDiff(grayImg,grayBgImg);
	threshImg.threshold(threshold, false);

	int num = contourFinder.findContours(threshImg, minSize, maxSize, 100, false, false);


//	masker.makeMask();
}


//--------------------------------------------------------------
void SimpleTracker::draw(float x, float y){


	ofSetColor(255,255,255);

	// live video or movie file
	colorImg.draw(10,10,320,240);

	grayImgPreModification.draw(10,10,320,240);
	grayImg.draw(320+20, 10, 320,240);
	grayBgImg.draw(10,20+240,320,240);

	ofSetColor(255,255,255);
	ofFill();
	ofRect( 320+20,240+20,320, 240);

	ofSetColor(255,255,255, 255);
	threshImg.draw(10 + 10 + 320, 10+10 + 240,320,240);
	contourFinder.draw(10 + 10 + 320,10+10+240,320,240);

	ofSetColor(255,255,255);

	grayImg.draw(10, 10, 320,240);
	threshImg.draw(10 + 10 + 320,10,320,240);
	contourFinder.draw(10 + 10 + 320,10,320,240);

	panel.draw();

	//glPushMatrix();
		//glTranslatef(10,10,0);
//		masker.draw();
	//glPopMatrix();
	ofSetColor(0xffffff);
//	masker.mask.draw( 10, 10 + 10 + 240, 320,240);
}

void SimpleTracker::setupGui()
{

	// create the gui panel
	panel.setup("cv panel", 700, 20, 300, 550);

	// add some tabs
	panel.addPanel("tracking", 1, false);


	//---- set which tab we are on and add some controls
	panel.setWhichPanel("tracking");

	panel.addToggle("capture bg ", "BG_CAPTURE", false);
	panel.addSlider("threshold ", "THRESHOLD_GAZE", threshold, 0, 255, true);
	panel.addSlider("min blob","MIN_BLOB",10,0,5000,true);
	panel.addSlider("max blob","MAX_BLOB",100*100,0,50500,true);


	// load our saved settings
	panel.loadSettings("settings/"+myName+"Settings.xml");


}



void SimpleTracker::updateGui(){

	panel.update();

	minSize = panel.getValueI("MIN_BLOB");
	maxSize = panel.getValueI("MAX_BLOB");

	threshold				= panel.getValueI("THRESHOLD_GAZE");


}

