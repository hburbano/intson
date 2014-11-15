/******************************************************************/
/**
 * @file	ofApp.cpp
 * @brief	Proyecto Final de Interaccion y Sonido
 * @author	HMBE
 * @date	Nov 2, 2014
 */
/******************************************************************/

#pragma once

#include "ofMain.h"
#include "ofxKinectNui.h"
#include "ofxKinectNuiPlayer.h"
#include "ofxKinectNuiRecorder.h"

class ofxKinectNuiDrawTexture;
class ofxKinectNuiDrawSkeleton;

class ofApp : public ofBaseApp {
	public:
		ofPoint leftHand;
		ofPoint rightHand;

		ofMesh leftMesh;
		ofMesh rightMesh;

		vector<ofVec3f> rightHPoints;
		vector<ofVec3f> leftHPoints;
		vector<ofColor> colorMesh;

		void setup();
		void update();
		void draw();
		void drawCalibratedTexture();
		void exit();
		void drawCircle3f(int n, int radius, ofVec3f cur);
		void paintLines();

		void initMesh();

		void drawHands();
		void drawSkeletonPoints();

		void keyPressed  (int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);
		void kinectPlugged();
		void kinectUnplugged();
		void fabricData();
		void startRecording();
		void stopRecording();
		void startPlayback();
		void stopPlayback();

		ofColor prettyColors(ofColor baseColor);
		ofColor prettyColors();

		ofxKinectNui kinect;

		ofxKinectNuiPlayer kinectPlayer;
		ofxKinectNuiRecorder kinectRecorder;

		ofxBase3DVideo* kinectSource;

		ofTexture calibratedTexture;

		bool bDrawHandsOnly;
		bool bPlayback;
		bool bDrawVideo;
		bool bDrawDepthLabel;
		bool bDrawSkeleton;
		bool bDrawCalibratedTexture;
		bool bPlugged;
		bool bUnplugged;
		
		unsigned short nearClipping;
		unsigned short farClipping;
		int angle;
		
		int mRotationX, mRotationY;

		ofVec3f center;
		ofCamera camera;

		// Please declare these texture pointer and initialize when you want to draw them
		ofxKinectNuiDrawTexture*	videoDraw_;
		ofxKinectNuiDrawTexture*	depthDraw_;
		ofxKinectNuiDrawTexture*	labelDraw_;
		ofxKinectNuiDrawSkeleton*	skeletonDraw_;

		//void keyReleased(int key);
		//void gotMessage(ofMessage msg);
		//void dragEvent(ofDragInfo dragInfo);

};
