#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
	TM.setup();
}

//--------------------------------------------------------------
void testApp::update(){
	TM.update();
}


//--------------------------------------------------------------
void testApp::draw(){
	TM.draw();
}

//--------------------------------------------------------------
void testApp::keyPressed  (int key){
	TM.keyPressed(key);
}

//--------------------------------------------------------------
void testApp::keyReleased(int key){

}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){
}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){
	TM.mouseDragged(x, y, button);
}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){
	TM.mousePressed(x, y, button);
}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){
	TM.mouseReleased();
}

//--------------------------------------------------------------
void testApp::resized(int w, int h){

}

