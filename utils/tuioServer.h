#ifndef TUIOSERVER_H_INCLUDED
#define TUIOSERVER_H_INCLUDED


#include "ofMain.h"
#include "ofxXmlSettings.h"
#include "ofxOsc.h"
#include "ofxCvBlobTracker.h"
#include "ofxCvBlob.h"


enum {

    NOT_CONNECTED, CONNECTED

};


class tuioServer {

	public:

		void setup(float _w, float _h);
		void update(vector<ofxCvTrackedBlob>& blobs);


		string                  host;
		int                     port;
		bool                    mode;
		float                   w, h;


    private:

		ofxOscSender sender;
		int findOrder(int id, vector<ofxCvTrackedBlob>& blobs);


};


#endif // TUIOSERVER_H_INCLUDED
