#include "openUpTracker.h"

// Blob Tracking

#define B_DEAD 0
#define B_ALIVE_IN_AREA 1
#define B_ALIVE_OUT_AREA 2

#define USER_JOINED 0
#define USER_MOVED 1
#define USER_LEFT 2
#define USER_MOOD 3
#define USER_PTBLOB_TEST 4
#define SCENE_MOOD_USER 99

//--------------------------------------------------------------
OpenUpTracker::OpenUpTracker(){

	myName = "openUpTracker";
}




//--------------------------------------------------------------
void OpenUpTracker::setup(int width, int height){

	minSize = 10;
	maxSize = width*height*.75;


	w = width;
	h = height;

	colorImg.allocate(w, h);
	grayImgPreModification.allocate(w, h);
	grayImg.allocate(w, h);
	threshImg.allocate(w, h);
	grayBgImg.allocate(w, h);
	grayBgImgMasked.allocate(w, h);

	threshold			= 60;


	setupGui();

//	masker.setup(w,h,10,10,2);
//	masker.setConstraints( ofRectangle(10,10,w,h) );


	// blob tracking
	topLeftX = 1;
	topLeftY = 1;
	lowRightX = w;
	lowRightY = h;
	userOffsetX = 0;
	userOffsetY = 0;

	// All blobs are marked as dead initially
	for (int i = 0; i < MAX_USERS_HARDLIMIT; i++)
	{
		usersTracking[i] = B_DEAD;
	}

	//blobTracker.setListener( this );
	TS.setup(w, h);     //start tuio server, if configured



}


//--------------------------------------------------------------
void OpenUpTracker::update(ofxCvColorImage & colorImgFromCam){

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
	threshImg.erode();
	threshImg.dilate();

	int blurVal = panel.getValueI( "BLUR" );
	if(  blurVal > 0 )
	{
		threshImg.blur(blurVal);
		threshImg.threshold(11, false);
	}

	int num = contourFinder.findContours(threshImg, minSize, maxSize, MAX_USERS_HARDLIMIT, false, false);

	if( num > 0 ) blobTracker.trackBlobs( contourFinder.blobs );


    if (TS.mode == CONNECTED) {

        TS.update(blobTracker.blobs);

    }
	// set mask if changed (could move this maybe? to mouse dragged??)
    //	masker.makeMask();
}


//--------------------------------------------------------------
void OpenUpTracker::draw(float x, float y){


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
	//contourFinder.draw(10 + 10 + 320,10,320,240);
	blobTracker.draw( 10 + 10 + 320,10,320/640.f,320/640.f );  // draws all blobs
	panel.draw();

	//glPushMatrix();
		//glTranslatef(10,10,0);
//		masker.draw();
	//glPopMatrix();
	ofSetColor(0xffffff);
//	masker.mask.draw( 10, 10 + 10 + 240, 320,240);




}

void OpenUpTracker::setupGui()
{

	// create the gui panel
	panel.setup("cv panel", 700, 20, 300, 550);

	// add some tabs
	panel.addPanel("tracking", 1, false);


	//---- set which tab we are on and add some controls
	panel.setWhichPanel("tracking");

	panel.addToggle("capture bg ", "BG_CAPTURE", false);
	panel.addSlider("threshold ", "THRESHOLD_GAZE", threshold, 0, 255, true);
	panel.addSlider("min blob","MIN_BLOB",10,0,500,true);
	panel.addSlider("max blob","MAX_BLOB",100*100,0,50500,true);
	panel.addSlider("blur","BLUR",11,0,11,true);

	// load our saved settings
	panel.loadSettings("settings/"+myName+"Settings.xml");


}



void OpenUpTracker::updateGui(){

	panel.update();

	minSize = panel.getValueI("MIN_BLOB");
	maxSize = panel.getValueI("MAX_BLOB");

	threshold				= panel.getValueI("THRESHOLD_GAZE");


}

void OpenUpTracker::blobOn( int x, int y, int id, int order )
{
	char updateMSG[100];

	int insideArea = 1;
    //cout << "blobOn() - id:" << id << " order:" << order << endl;
    float scaledX = (float) ((userOffsetX+x)-topLeftX) / (float)(lowRightX-topLeftX);
    float scaledY = (float) ((userOffsetY+y)-topLeftY) / (float)(lowRightY-topLeftY);


	if (scaledX < 0){ scaledX = 0; insideArea = 0;}
	if (scaledY < 0){ scaledY = 0; insideArea = 0;}
	if (scaledX > 1){ scaledX = 1; insideArea = 0;}
	if (scaledY > 1){ scaledY = 1; insideArea = 0;}
	if (insideArea)
	{
		// A new user has appeared inside the area, therefore he's joined.
		sprintf(updateMSG, "%d|%d|%f|%f|%f|%f\n", USER_JOINED, order, scaledX, scaledY,
				0.0, 0.0);
		printf(updateMSG);
		//tcps.sendToAll(updateMSG);

		usersTracking[order] = B_ALIVE_IN_AREA;
	} else
	{
		// A new user has appeared outside the play area. S/He's not reported as joining, but
		// taken into account when s/he walks inside the play area.
		usersTracking[order] = B_ALIVE_OUT_AREA;
	}

}
void OpenUpTracker::blobMoved( int x, int y, int id, int order)
{
	char updateMSG[100];
	int insideArea = 1;
    //cout << "blobMoved() - id:" << id << " order:" << order << endl;

    // full access to blob object ( get a reference)
    //ofxCvTrackedBlob blob = blobTracker.getById( id );
    //cout << "area: " << blob.area << endl;
	float scaledX = (float) ((userOffsetX+x)-topLeftX) / (float)(lowRightX-topLeftX);
    float scaledY = (float) ((userOffsetY+y)-topLeftY) / (float)(lowRightY-topLeftY);



	if (scaledX < 0){ scaledX = 0; insideArea = 0;}
	if (scaledY < 0){ scaledY = 0; insideArea = 0;}
	if (scaledX > 1){ scaledX = 1; insideArea = 0;}
	if (scaledY > 1){ scaledY = 1; insideArea = 0;}

	if (insideArea)
	{
		if (usersTracking[order] == B_ALIVE_IN_AREA)
		{
			// The user is moving inside the area
			sprintf(updateMSG, "%d|%d|%f|%f|%f|%f\n", USER_MOVED, order, scaledX, scaledY,
					0.0, 0.0);
			printf(updateMSG);
			//tcps.sendToAll(updateMSG);
		} else {
			// The user was outside the play area, but now came inside and therefore joined.
			usersTracking[order] = B_ALIVE_IN_AREA;
			sprintf(updateMSG, "%d|%d|%f|%f|%f|%f\n", USER_JOINED, order, scaledX, scaledY,
					0.0, 0.0);
			printf(updateMSG);
			//tcps.sendToAll(updateMSG);

		}

	} else {
		if (usersTracking[order] == B_ALIVE_IN_AREA)
		{
			// The user was inside the play area but has left
			sprintf(updateMSG,"%d|%d|%f|%f|%f|%f\n", USER_LEFT, order, scaledX, scaledY,
					0.0, 0.0);
			printf(updateMSG);
			//tcps.sendToAll(updateMSG);
		}
		usersTracking[order] = B_ALIVE_OUT_AREA;
	}
}

void OpenUpTracker::blobOff( int x, int y, int id, int order )
{
	char updateMSG[100];
    //cout << "blobOff() - id:" << id << " order:" << order << endl;
    float scaledX = (float) ((userOffsetX+x)-topLeftX) / (float)(lowRightX-topLeftX);
    float scaledY = (float) ((userOffsetY+y)-topLeftY) / (float)(lowRightY-topLeftY);

	// Is this blob alive and inside the play area?
	// Then send the notification. Do not send notification otherwise..
	if (usersTracking[order] == B_ALIVE_IN_AREA)
	{
		if (scaledX < 0) scaledX = 0;
		if (scaledY < 0) scaledY = 0;
		if (scaledX > 1) scaledX = 1;
		if (scaledY > 1) scaledY = 1;
		sprintf(updateMSG, "%d|%d|%f|%f|%f|%f\n", USER_LEFT, order, scaledX, scaledY,
				0.0, 0.0);
		printf(updateMSG);
		//tcps.sendToAll(updateMSG);
	}
	// mark blob as dead
	usersTracking[order] = B_DEAD;
}
