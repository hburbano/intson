#include "ofMain.h"
#include "ofApp.h"
#include "ofAppGlutWindow.h"

//========================================================================
int main( ){
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_CHECK_ALWAYS_DF | _CRTDBG_LEAK_CHECK_DF);
    ofAppGlutWindow window;
	ofSetupOpenGL(&window, 640,480, OF_WINDOW);	
	ofSetWindowPosition(50,50);
	ofRunApp( new ofApp());
}