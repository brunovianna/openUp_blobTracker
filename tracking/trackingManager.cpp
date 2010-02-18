#include "trackingManager.h"
#include "openUpTracker.h"

trackingManager::trackingManager(){

}

trackingManager::~trackingManager(){

}

void trackingManager::setup(){


	IM.setup();		// setup input

	// create trackers
	//trackers.push_back( new SimpleTracker() );
	trackers.push_back( new OpenUpTracker() );

	tOn = 0;

	for( int i = 0; i < trackers.size(); i++)
		trackers[i]->setup(IM.width,IM.height);

	font.loadFont("fonts/verdana.ttf",24);
}

void trackingManager::update(){

	//--- update video/camera input
	IM.update();

	//---  tracking (on new frames)
	if (IM.bIsFrameNew){

		trackers[ tOn ]->update(IM.colorImg);

	}

}


void trackingManager::draw(){

	ofSetColor(255,255,255);

	trackers[tOn]->draw(0,0);

	ofSetColor(0,0,0);
	font.drawString(trackers[tOn]->myName, 20, ofGetHeight()-50);

	ofDrawBitmapString("Space: Change Tracker | f: toggle fullscreen", 20,  ofGetHeight()-25);
}


//--------------------------------------------------------------
void trackingManager::mouseDragged(int x, int y, int button){

	trackers[tOn]->panel.mouseDragged(x, y, button);

}

//--------------------------------------------------------------
void trackingManager::mousePressed(int x, int y, int button){

	trackers[tOn]->panel.mousePressed(x, y, button);
}

//--------------------------------------------------------------
void trackingManager::mouseReleased(){

	trackers[tOn]->panel.mouseReleased();

}

//--------------------------------------------------------------
void trackingManager::keyPressed(int key){

	if( key == ' ' )
	{
		tOn++;
		tOn %= trackers.size();
	}

	if( key == 'f') ofToggleFullscreen();
}
