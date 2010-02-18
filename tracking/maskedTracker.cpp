#include "maskedTracker.h"


//--------------------------------------------------------------
MaskedTracker::MaskedTracker(){

	myName = "maskedTracker";
}


		

//--------------------------------------------------------------
void MaskedTracker::setup(int width, int height){

	minSize = 10; 
	maxSize = width*height*.75;
	
	bBgCapture = true;
	
	w = width;
	h = height;
	
	colorImg.allocate(w, h);
	grayImgPreWarp.allocate(w, h);
	grayImgPreModification.allocate(w, h);
	grayImg.allocate(w, h);
	threshImg.allocate(w, h);
	grayBgImg.allocate(w, h);
	edgeMask.allocate(w, h);
	edgeMaskInverted.allocate(w,h);

	threshold			= 60;
	
	bUseGamma			= false;
	gamma				= 1.0f;
	
	bUseContrast		= false;
	contrast			= 0.2;
	brightness			= 0.0;
	
	bUseDilate			= false;
	nDilations			= 1;

	flipX = false;
	flipY = false;	
	
	edgeMaskStartPos.set(w/2, h/2);
	edgeMaskInnerRadius = 250;
	edgeMaskOuterRadius = 350;
	edgeMaskPixels = new unsigned char [ (int)(w * h) ];
	
	calculateEdgePixels();
	
	setupGui();
}


//--------------------------------------------------------------
void MaskedTracker::flip(bool bHorizontal, bool bVertical){
	flipX = bHorizontal; 
	flipY = bVertical;
}


//--------------------------------------------------------------
void MaskedTracker::calculateEdgePixels(){
	
	for (int i = 0; i < w; i++){
		for (int j = 0; j < h; j++){
			float dx = i - edgeMaskStartPos.x;
			float dy = j - edgeMaskStartPos.y;
			float dist = sqrt(dx*dx + dy*dy);
			float pct = (dist - edgeMaskInnerRadius) / (edgeMaskOuterRadius - edgeMaskInnerRadius);
			if (pct < 0) pct = 0;
			if (pct > 1) pct = 1;
			edgeMaskPixels[(int)(j * w + i)] = pct * 255;
		}
	}	
	edgeMask.setFromPixels(edgeMaskPixels, w,h);
	
	edgeMaskInverted = edgeMask;
	edgeMaskInverted.invert();
}


//--------------------------------------------------------------
void MaskedTracker::update(ofxCvColorImage & colorImgFromCam){
	
	// update vars from control panel
	updateGui();
	
	// flip if we need to
	flip(  panel.getValueB("B_RIGHT_FLIP_X"),  panel.getValueB("B_RIGHT_FLIP_Y") );

	// maybe not needed, slow?
	colorImg = colorImgFromCam;
	
	// preserve original
	grayImgPreModification = colorImgFromCam;
	
	grayImgPreWarp.setFromPixels(grayImgPreModification.getPixels(), grayImgPreModification.width, grayImgPreModification.height);		// TODO: there's maybe an unnecessary grayscale image (and copy) here...
	
	if( flipX || flipY ){
		grayImgPreWarp.mirror(flipY, flipX);
	}

	grayImg = grayImgPreWarp;
	
	grayImgPreModification = grayImg;
	
	// grayImg.blur(5);
		
	if (bUseContrast == true){
		grayImg.applyBrightnessContrast(brightness,contrast);
	}

	if (bUseGamma == true){
		grayImg.applyMinMaxGamma(gamma);
	}
	
	// add mask
	grayImg += edgeMask;
	
	// bg  cap
	if(panel.getValueB("BG_CAPTURE") ){
		grayBgImg = grayImg;
		panel.setValueB("BG_CAPTURE", false);
	}
	
	//threshImg = grayImg;	
	threshImg.absDiff(grayImg,grayBgImg);
	threshImg.threshold(threshold, false);
	
	//threshImg.contrastStretch();
	//threshImg.threshold(threshold, true);

	int num = contourFinder.findContours(threshImg, minSize, maxSize, 100, false, true);
	

}


//--------------------------------------------------------------
void MaskedTracker::draw(float x, float y){


	/*ofPushMatrix();
		ofTranslate(x, y, 0);
		//ofScale(totalWidth/(w*2), totalHeight/h, 1);
		
		ofSetColor(255, 255, 255);
		//grayImg.draw(0,0);
		//threshImg.draw(threshImg.width, 0);
		//contourFinder.draw(threshImg.width, 0);
		
	ofPopMatrix();
	
	ofSetColor(0xffffff);
	ofDrawBitmapString("thresh is "+ofToString(threshold, 0), x, y + 10);
	*/
	
	//---------------------------------------------------------------- edge
	if (panel.getSelectedPanelName() == "image adjustment" ){
		
		ofSetColor(255,255,255);
		
		// live video or movie file
		colorImg.draw(10,10,320,240);
		
		grayImgPreModification.draw(10,10,320,240);
		grayImg.draw(320+20, 10, 320,240);
		
		
		grayBgImg.draw(10,20+240,320,240);
		
		ofSetColor(255,255,255);
		ofFill();
		ofRect( 320+20,240+20,320, 240);
		
		ofEnableAlphaBlending();
		ofSetColor(255,255,255, 255);
		threshImg.draw(10 + 10 + 320, 10+10 + 240,320,240);
		contourFinder.draw(10 + 10 + 320,10+10+240,320,240);
		
	}
	
	if (panel.getSelectedPanelName() == "edge fixer"){
		ofSetColor(255,255,255);
		edgeMaskInverted.draw(10,10, 320, 240);
		grayImg.draw(320+20, 10, 320,240);
	}
	
	
	if (panel.getSelectedPanelName() == "blob detection"){
		ofSetColor(255,255,255);
		
		grayImg.draw(10, 10, 320,240);
		
		threshImg.draw(10 + 10 + 320,10,320,240);
		contourFinder.draw(10 + 10 + 320,10,320,240);
	}
	
	panel.draw();


}

void MaskedTracker::setupGui()
{
	
	// create the gui panel
	panel.setup("cv panel", 700, 20, 300, 550);

	// add some tabs
	panel.addPanel("image adjustment", 1, false);
	panel.addPanel("edge fixer", 1, false);
	panel.addPanel("blob detection", 1, false);


	//---- set which tab we are on and add some controls
	panel.setWhichPanel("image adjustment");

	panel.addToggle("capture bg ", "BG_CAPTURE", false);
	panel.addToggle("flip horizontal ", "B_RIGHT_FLIP_X", false);
	panel.addToggle("flip vertical ", "B_RIGHT_FLIP_Y", false);

	panel.addToggle("use contrast / bri", "B_USE_CONTRAST", true);
	panel.addSlider("contrast ", "CONTRAST", 0.28f, 0.0, 1.0f, false);
	panel.addSlider("brightness ", "BRIGHTNESS", -0.02f, -1.0, 3.0f, false);

	panel.addToggle("use gamma ", "B_USE_GAMMA", true);
	panel.addSlider("gamma ", "GAMMA", 0.57f, 0.01, 3.0f, false);

	panel.addSlider("threshold ", "THRESHOLD_GAZE", threshold, 0, 255, true);

	//---- blob detection
	panel.setWhichPanel("blob detection");
	panel.addToggle("use dilate", "B_USE_DILATE", true);
	panel.addSlider("dilate num ", "N_DILATIONS", 0, 0, 10, true);
	panel.addSlider("min blob","MIN_BLOB",10*10,0,5000,true);
	panel.addSlider("max blob","MAX_BLOB",100*100,0,50500,true);

	//---- tracker edges
	panel.setWhichPanel("edge fixer");
	panel.setWhichColumn(0);
	panel.addSlider("x position ", "EDGE_MASK_X", 320, 0, 640, true);
	panel.addSlider("y position ", "EDGE_MASK_Y", 240, 0, 640, true);
	panel.addSlider("inner radius ", "EDGE_MASK_INNER_RADIUS", 250, 0, 500, true);
	panel.addSlider("outer radius ", "EDGE_MASK_OUTER_RADIUS", 350, 0, 600, true);



	// load our saved settings
	panel.loadSettings("settings/"+myName+"Settings.xml");


}



void MaskedTracker::updateGui(){
	
	panel.update();
	
	minSize = panel.getValueI("MIN_BLOB");
	maxSize = panel.getValueI("MAX_BLOB");
	
	threshold				= panel.getValueI("THRESHOLD_GAZE");
	
	gamma			= panel.getValueF("GAMMA");
	bUseGamma		= panel.getValueB("B_USE_GAMMA");
	
	contrast		= panel.getValueF("CONTRAST");
	brightness		= panel.getValueF("BRIGHTNESS");
	bUseContrast	= panel.getValueB("B_USE_CONTRAST");
	
	nDilations		= panel.getValueI("N_DILATIONS");
	bUseDilate		= panel.getValueB("B_USE_DILATE");
	
	int oldx				= edgeMaskStartPos.x;
	int oldy				= edgeMaskStartPos.y;
	int oldir				= edgeMaskInnerRadius;
	int oldor				= edgeMaskOuterRadius;
	
	edgeMaskStartPos.x		= panel.getValueI("EDGE_MASK_X");
	edgeMaskStartPos.y		= panel.getValueI("EDGE_MASK_Y");
	edgeMaskInnerRadius		= panel.getValueI("EDGE_MASK_INNER_RADIUS");
	edgeMaskOuterRadius		= panel.getValueI("EDGE_MASK_OUTER_RADIUS");
	
	
	if (	oldx	!= edgeMaskStartPos.x  ||
	 oldy	!= edgeMaskStartPos.y  ||
	 oldir	!= edgeMaskInnerRadius ||
	 oldor	!= edgeMaskOuterRadius	){		
	 
	 calculateEdgePixels();
	 
	 }
	 

}

/*
 
 
*/ 
