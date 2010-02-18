#include "tuioServer.h"

/*
<app>

    <mode>1</mode>
	<host>localhost</host>
	<port>3333</port>

</app>
*/

//--------------------------------------------------------------
void tuioServer::setup(float _w, float _h){

	ofxXmlSettings XML;
	XML.loadFile("settings/tuioSettings.xml");
	host = XML.getValue("app:host", "localhost");
	port = XML.getValue("app:port", 3333);
	mode = XML.getValue("app:mode", 0);
	w = _w;
	h = _h;

    if (mode == CONNECTED) {

        // open an outgoing connection to HOST:PORT
        sender.setup( host, port );

    }
}

void tuioServer::update(vector<ofxCvTrackedBlob>& blobs){

    ofxOscBundle b;
	ofxOscMessage alive, fseq;
    ofxOscMessage blobMsg[blobs.size()];

	ofPoint center;

//	printf ("%d", blobs.size());


    alive.setAddress( "/tuio/2Dobj" );
    alive.addStringArg( "alive" );


//    printf ("%d: ", frame);

    for (int i=0; i< blobs.size(); i++) {

//        printf ("%d, ", blobs[i].id);

        alive.addIntArg(blobs[i].id );

        blobMsg[i].setAddress( "/tuio/2Dobj" );
        blobMsg[i].addStringArg( "set" );
        blobMsg[i].addIntArg( blobs[i].id ); //session id
        blobMsg[i].addIntArg( blobs[i].id); //object id
//        blobMsg[i].addIntArg( findOrder(blobs[i].id, blobs )); //object id

        center = blobs[i].centroid;

        blobMsg[i].addFloatArg (center.x / w); //x 0...1
        blobMsg[i].addFloatArg (center.y / h); //y 0...1
        blobMsg[i].addFloatArg (0); //angle 0...2PI
        blobMsg[i].addFloatArg (0); //xspeed 0...1
        blobMsg[i].addFloatArg (0); //yspeed 0...1
        blobMsg[i].addFloatArg (0); //rspeed 0...2PI
        blobMsg[i].addFloatArg (0); //maccel 0...1
        blobMsg[i].addFloatArg (0); //raccel 0...2PI
        b.addMessage(blobMsg[i]);

    }

    b.addMessage(alive);
//    printf ("\n");


    if (blobs.size()>0) {
        fseq.setAddress( "/tuio/2Dobj" );
        fseq.addStringArg( "fseq" );
        fseq.addIntArg( -1 );
        b.addMessage(fseq);
    }

    sender.sendBundle(b);
}

 int tuioServer::findOrder(int id, vector<ofxCvTrackedBlob>& blobs) {
    int count = 0;
    for( int i=0; i<blobs.size(); i++ ) {
        if( blobs[i].id < id ) {
            count++;
        }
    }
    return count;
}
