#ifndef _TRACKSCENE_H
#define _TRACKSCENE_H


#include "ofMain.h"
#include "inputManager.h"
#include "baseTracker.h"
#include "maskedTracker.h"
#include "simpleTracker.h"
#include "tuioServer.h"

class trackingManager {

	public:
		trackingManager();
		~trackingManager();

		void setup();
		void update();
		void draw();


		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased();
		void keyPressed(int key);

		//----- video sources... what are we tracking!
		inputManager		IM;

		//----- trackers
		vector<BaseTracker*> trackers;
		int tOn;

		ofTrueTypeFont	font;

};

#endif
