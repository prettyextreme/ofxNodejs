#include "testApp.h"

#include "ofxNodejs.h"

ofImage mouse;

struct Client
{
	int x, y;
};

map<string, Client> clients;

using namespace ofxNodejs;

static v8::Handle<v8::Value> onLoggedIn(const v8::Arguments& args)
{
	string id = Value(args[0]).toString();
	
	printf("loggedin: %s\n", id.c_str());
	
	clients[id] = Client();
	
	return v8::Undefined();
}

static v8::Handle<v8::Value> onLoggedOut(const v8::Arguments& args)
{
	string id = Value(args[0]).toString();
	
	printf("loggedout: %s\n", id.c_str());
	
	clients.erase(id);
	
	return v8::Undefined();
}

static v8::Handle<v8::Value> onMouseMoved(const v8::Arguments& args)
{
	string id = Value(args[0]).toString();
	int x = Value(args[1]).toInt32();
	int y = Value(args[2]).toInt32();
	
	Client &c = clients[id];
	c.x = x;
	c.y = y;
	
	return v8::Undefined();
}

//--------------------------------------------------------------
void testApp::setup(){

	ofSetFrameRate(60);
	ofSetVerticalSync(true);
	
	ofBackground(255, 255, 255);
	
	mouse.setImageType(OF_IMAGE_COLOR_ALPHA);
	mouse.loadImage("mouse.png");

	
	// setup node.js
	ofxNodejs::addNodePath("node/libs");
	ofxNodejs::init("node/main.js");
	
	// add callback function
	ofxNodejs::setFunction("onLoggedIn", onLoggedIn);
	ofxNodejs::setFunction("onLoggedOut", onLoggedOut);
	ofxNodejs::setFunction("onMouseMoved", onMouseMoved);
	
	// run script
	$$("node/script.js");
}

//--------------------------------------------------------------
void testApp::update(){
}

//--------------------------------------------------------------
void testApp::draw(){
	ofEnableAlphaBlending();
	
	ofSetColor(0, 0, 0);
	ofDrawBitmapString("http://localhost:8001/index.html", 20, 20);
	
	ofSetColor(255, 255, 255);
	map<string, Client>::iterator it = clients.begin();
	while (it != clients.end())
	{
		Client &c = (*it).second;
		
		mouse.draw(c.x, c.y);
		
		it++;
	}
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
}

//--------------------------------------------------------------
void testApp::keyReleased(int key){

}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo){ 

}