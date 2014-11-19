/******************************************************************/
/**
* @file	ofApp.cpp
* @brief	Proyecto Final de Interaccion y Sonido
* @author	HMBE
* @date	Nov 2, 2014
*/
/******************************************************************/

#include "ofApp.h"
#include "ofxKinectNuiDraw.h"
#include "ofMath.h"

//--------------------------------------------------------------
void ofApp::setup() {
	ofSetLogLevel(OF_LOG_VERBOSE);
	showHelp = false;
	
	// Setup OSC host
	sender.setup(HOST, PORT);


	leftHand.set(0, 0, 0);
	rightHand.set(0, 0, 0);


	//Basics parameters for ofxKinectNui in the app;
	ofxKinectNui::InitSetting initSetting;
	initSetting.grabVideo = false;
	initSetting.grabDepth = false;
	initSetting.grabAudio = false;
	initSetting.grabLabel = false;
	initSetting.grabSkeleton = true;
	initSetting.grabCalibratedVideo = false;
	initSetting.grabLabelCv = false;
	initSetting.videoResolution = NUI_IMAGE_RESOLUTION_640x480;
	initSetting.depthResolution = NUI_IMAGE_RESOLUTION_640x480;

	kinect.init(initSetting);
	kinect.open();
	kinect.addKinectListener(this, &ofApp::kinectPlugged, &ofApp::kinectUnplugged);

	kinectSource = &kinect;
	angle = kinect.getCurrentAngle();
	bPlugged = kinect.isConnected();
	nearClipping = kinect.getNearClippingDistance();
	farClipping = kinect.getFarClippingDistance();

	bDrawHandsOnly = true;
	bDrawCalibratedTexture = false;
	bDrawVideo = false;
	bDrawDepthLabel = false;
	bDrawSkeleton = false;
	
	
	//OSC Data

	maxDistance = 2;
	minDistance = 1;

	//Video parameters
	glEnable(GL_DEPTH_TEST);
	ofSetFrameRate(30);
	ofSetVerticalSync(false);

	initMesh();

}

//--------------------------------------------------------------
void ofApp::update() {
	kinectSource->update();
	if (kinect.isFoundSkeleton())
	{
		ofVec3f sumOfAllPoints(0,0,0);
		for(unsigned int i = 0; i < leftHPoints.size(); i++){
			leftHPoints[i].z -= 4;
			sumOfAllPoints += leftHPoints[i];
		}       

		for(unsigned int i = 0; i < rightHPoints.size(); i++){
			rightHPoints[i].z -= 4;
			sumOfAllPoints += rightHPoints[i];
		}
		center = sumOfAllPoints / rightHPoints.size();

	}



}

//--------------------------------------------------------------

void ofApp::initMesh(){
	colorMesh.push_back(ofColor(0,0,0));
	leftMesh.setMode(OF_PRIMITIVE_TRIANGLE_STRIP);
	rightMesh.setMode(OF_PRIMITIVE_TRIANGLE_STRIP);
}


ofColor ofApp::prettyColors(ofColor baseColor){
	int varRang = 256;

	int red = (int)ofRandom(varRang);
	int green = (int)ofRandom(varRang);
	int blue = (int)ofRandom(varRang);

	// mix the color
	if (baseColor != NULL) {
		red = (red + baseColor.r) / 2;
		green = (green + baseColor.g) / 2;
		blue = (blue + baseColor.b) / 2;
	}

	ofColor color = ofColor(red, green, blue);
	return color;

}

ofColor ofApp::prettyColors(){
	return prettyColors(ofColor(0, 0, 0));
}

void ofApp::paintLines(){


	leftMesh.clearVertices();
	rightMesh.clearVertices();
	leftMesh.clearColors();
	rightMesh.clearColors();


	if (kinect.isFoundSkeleton())
	{

		for(unsigned int i = 1; i < leftHPoints.size(); i++){

			//find this point and the next point
			ofVec3f thisPoint = leftHPoints[i-1];
			ofVec3f nextPoint = leftHPoints[i];

			//get the direction from one to the next.
			//the ribbon should fan out from this direction
			ofVec3f direction = (nextPoint - thisPoint);

			//get the distance from one point to the next
			float distance = direction.length();

			//get the normalized direction. normalized vectors always have a length of one
			//and are really useful for representing directions as opposed to something with length
			ofVec3f unitDirection = direction.normalized();

			//find both directions to the left and to the right
			ofVec3f toTheLeft = unitDirection.getRotated(-90, ofVec3f(0,0,1));
			ofVec3f toTheRight = unitDirection.getRotated(90, ofVec3f(0,0,1));

			//use the map function to determine the distance.
			//the longer the distance, the narrower the line.
			//this makes it look a bit like brush strokes
			float thickness = ofMap(distance, 0, 60, 20, 2, true);

			//calculate the points to the left and to the right
			//by extending the current point in the direction of left/right by the length
			ofVec3f leftPoint = thisPoint+toTheLeft*thickness;
			ofVec3f rightPoint = thisPoint+toTheRight*thickness;

			//add these points to the triangle strip
			leftMesh.addVertex(ofVec3f(leftPoint.x, leftPoint.y, leftPoint.z));
			leftMesh.addVertex(ofVec3f(rightPoint.x, rightPoint.y, rightPoint.z));
			leftMesh.addColor(colorMesh.at(i));
			leftMesh.addColor(colorMesh.at(i));
		}

		for(unsigned int i = 1; i < rightHPoints.size(); i++){

			//find this point and the next point
			ofVec3f thisPoint = rightHPoints[i-1];
			ofVec3f nextPoint = rightHPoints[i];

			//get the direction from one to the next.
			//the ribbon should fan out from this direction
			ofVec3f direction = (nextPoint - thisPoint);

			//get the distance from one point to the next
			float distance = 50;

			//get the normalized direction. normalized vectors always have a length of one
			//and are really useful for representing directions as opposed to something with length
			ofVec3f unitDirection = direction.normalized();

			//find both directions to the left and to the right
			ofVec3f toTheLeft = unitDirection.getRotated(-90, ofVec3f(0,0,1));
			ofVec3f toTheRight = unitDirection.getRotated(90, ofVec3f(0,0,1));

			//use the map function to determine the distance.
			//the longer the distance, the narrower the line.
			//this makes it look a bit like brush strokes
			float thickness = ofMap(distance, 0, 60, 20, 2, true);

			//calculate the points to the left and to the right
			//by extending the current point in the direction of left/right by the length
			ofVec3f leftPoint = thisPoint+toTheLeft*thickness;
			ofVec3f rightPoint = thisPoint+toTheRight*thickness;

			//add these points to the triangle strip
			rightMesh.addVertex(ofVec3f(leftPoint.x, leftPoint.y, leftPoint.z));
			rightMesh.addVertex(ofVec3f(rightPoint.x, rightPoint.y, rightPoint.z));
			rightMesh.addColor(colorMesh.at(i));
			rightMesh.addColor(colorMesh.at(i));
		}

		leftMesh.draw();
		rightMesh.draw();

	}

}

void ofApp::draw() {
	ofBackground(0, 0, 0);
	// Draw video only
	if(bDrawVideo){
		// draw video images from kinect camera
		kinect.drawVideo(0, 0, 1024, 768);
		// Draw depth + users label only
	}else if(bDrawDepthLabel){
		ofEnableAlphaBlending();

		// draw depth images from kinect depth sensor
		kinect.drawDepth(0, 0, 1024, 768);
		// draw players' label images on video images
		kinect.drawLabel(0, 0, 1024, 768);
		ofDisableAlphaBlending();
		// Draw skeleton only
	}else if(bDrawSkeleton){

		kinect.drawSkeleton(0, 0, 640, 480);


	}else if(bDrawHandsOnly){

		fabricData();
		drawHands();
		paintLines();

	}else if(bDrawCalibratedTexture){
		ofPushMatrix();
		drawCalibratedTexture();	// draw calibrated images coodinates to depth images
		ofPopMatrix();
	}else{
		if(true){
			// draw video images from kinect camera
			kinect.drawVideo(20, 20, 400, 300);
			ofEnableAlphaBlending();
			// draw depth images from kinect depth sensor
			kinect.drawDepth(20, 340, 400, 300);
			// draw players' label images on video images
			kinect.drawLabel(20, 340, 400, 300);
			ofDisableAlphaBlending();
			// draw skeleton images on video images
			kinect.drawSkeleton(20, 20, 400, 300);


		}else{
			kinectPlayer.draw(20, 20, 400, 300);
			ofEnableAlphaBlending();
			kinectPlayer.drawDepth(20, 340, 400, 300);
			kinectPlayer.drawLabel(20, 340, 400, 300);

			ofDisableAlphaBlending();
			kinectPlayer.drawSkeleton(20, 20, 400, 300);
		}
	}

	ofPushMatrix();
	ofTranslate(35, 35);
	ofFill();
	ofPopMatrix();

	stringstream kinectReport;
	if(bPlugged && !kinect.isOpened()){
		ofSetColor(0, 255, 0);
		kinectReport << "Kinect is plugged..." << endl;
		ofDrawBitmapString(kinectReport.str(), 200, 300);
	}else if(!bPlugged){
		ofSetColor(255, 0, 0);
		kinectReport << "Kinect is unplugged..." << endl;
		ofDrawBitmapString(kinectReport.str(), 200, 300);
	}

	ofSetColor(0, 0, 0);
	stringstream reportStream;

	// TODO show and hide help

	//reportStream << "TODO: Organice help";
	if (showHelp){
		reportStream << "fps: " << ofGetFrameRate() << endl
		<< "Press 'c' to close the stream and 'o' to open it again, stream is: " << kinect.isOpened() << "." << endl
		<< "Press UP and DOWN to change the tilt angle: " << angle << " degrees." << endl
		<< "Press H to toogle this help."<< endl
		<< "Press F to toogle fullscreen."<< endl;
		ofDrawBitmapString(reportStream.str(), 20, 50);
	}
}

//--------------------------------------------------------------
void ofApp::drawCalibratedTexture(){
	ofPixels calibpix = kinect.getCalibratedVideoPixels();
	ofMesh mesh;
	mesh.setMode(OF_PRIMITIVE_TRIANGLES);
	int w = kinect.getDepthResolutionWidth();
	int h = kinect.getDepthResolutionHeight();
	ofShortPixels dis = kinect.getDistancePixels();
	for(int y = 0; y < h; y++){
		for(int x = 0; x < w; x++){
			int dIndex = y * w + x;
			short distance = dis[dIndex];
			if(distance > 500 && distance < 3000){
				ofFloatColor c = ofFloatColor(calibpix[3 * dIndex] / 255.f, calibpix[3 * dIndex + 1] / 255.f, calibpix[3 * dIndex + 2] / 255.f);
				mesh.addColor(c);

				// z-position is just proper value
				mesh.addVertex(ofPoint(x, y, (1500 - distance) * 0.5f));
				mesh.addIndex(y * w + x);
			}else{
				ofFloatColor c = ofFloatColor(calibpix[3 * dIndex] / 255.f, calibpix[3 * dIndex + 1] / 255.f, calibpix[3 * dIndex + 2] / 255.f, 0.f);
				mesh.addColor(c);
				mesh.addVertex(ofPoint(x, y, 0));
				mesh.addIndex(y * w + x);
			}
		}
	}

	for(int y = 0; y < h - 1; y += 2){
		for(int x = 0; x < w - 1; x += 2){
			int pos1 = w * y + x;
			int pos2 = w * y + x + 1;
			int pos3 = w * (y + 1) + x;
			int pos4 = w * (y + 1) + x + 1;
			mesh.addTriangle(pos1, pos2, pos3);
			mesh.addTriangle(pos2, pos3, pos4);
		}
	}

	ofRotateY(mRotationY);
	ofRotateX(mRotationX);
	ofEnableAlphaBlending();
	mesh.drawVertices();

	ofDisableAlphaBlending();
}

void ofApp::drawHands(){
	stringstream permaPoint;
	ofSetColor(255, 255, 255);
	if(leftHand.length()>1){		
		permaPoint.str("L");
		ofDrawBitmapString(permaPoint.str(),leftHand.x,leftHand.y);
	}
	if(rightHand.length()>1){		
		permaPoint.str("R");
		ofDrawBitmapString(permaPoint.str(),rightHand.x,rightHand.y);
	}

	//Control volumen with distance betwen hands

	volumeControl();
	paintLines();
}

void ofApp::fabricData(){
	int maxDrawn= 10000;
	ofPoint* skeletonPoints[ofxKinectNui::SKELETON_COUNT];
	kinect.getSkeletonPoints(skeletonPoints);	

	if(leftHPoints.size()>maxDrawn){
		leftHPoints.erase (leftHPoints.begin());
	}

	if(rightHPoints.size()>maxDrawn){
		rightHPoints.erase (rightHPoints.begin());
	}

	if(colorMesh.size()>maxDrawn){
		colorMesh.erase (colorMesh.begin());
	}

	if(kinect.isFoundSkeleton()){
		for(int i = 0; i < ofxKinectNui::SKELETON_COUNT; i++){
			if(kinect.isTrackedSkeleton(i)){
				// 7 and 11 left and right hand respectively
				leftHand = skeletonPoints[i][7];
				leftHand.z= 0;
				leftHPoints.push_back(leftHand);
				rightHand = skeletonPoints[i][11];
				rightHand.z= 0;
				rightHPoints.push_back(rightHand);
				if(colorMesh.size()%10==0){
					colorMesh.erase (colorMesh.begin());
					colorMesh.push_back(prettyColors(colorMesh.back()));
					colorMesh.push_back(prettyColors(colorMesh.back()));
				}else{
					colorMesh.erase (colorMesh.begin());
					colorMesh.push_back(colorMesh.back());
					colorMesh.push_back(colorMesh.back());
				}				
			}
		}
	}
}



void ofApp::drawSkeletonPoints(){
	ofPoint* skeletonPoints[ofxKinectNui::SKELETON_COUNT];
	kinect.getSkeletonPoints(skeletonPoints);
	stringstream permaPoint;
	if(kinect.isFoundSkeleton()){
		for(int i = 0; i < ofxKinectNui::SKELETON_COUNT; i++){
			if(kinect.isTrackedSkeleton(i)){
				for(int j = 0; j < ofxKinectNui::SKELETON_POSITION_COUNT; j++){

					ofPoint joint = skeletonPoints[i][j];
					//pointStr << j <<": X:" <<joint.x<<"Y:" <<joint.y<< "Z:"<<joint.z<<endl;
					permaPoint.str("");
					permaPoint<<j;
					ofDrawBitmapString(permaPoint.str(),joint.x,joint.y);
					//ofDrawBitmapString(pointStr.str(),20,20);
				}
				//ofDrawBitmapString(pointStr.str(),20,20);
			}
		}
	}

}

//--------------------------------------------------------------
void ofApp::exit() {
	if(calibratedTexture.bAllocated()){
		calibratedTexture.clear();
	}

	/*if(videoDraw_) {
	videoDraw_->destroy();
	videoDraw_ = NULL;
	}
	if(depthDraw_) {
	depthDraw_->destroy();
	depthDraw_ = NULL;
	}
	if(labelDraw_) {
	labelDraw_->destroy();
	labelDraw_ = NULL;
	}
	if(skeletonDraw_) {
	delete skeletonDraw_;
	skeletonDraw_ = NULL;
	}*/
	kinect.setAngle(0);
	kinect.close();
	kinect.removeKinectListener(this);
}



//--------------------------------------------------------------
void ofApp::keyPressed (int key) {
	switch(key){
	case 'f':
	case 'F':
		ofToggleFullscreen();
		break;

	case 'h': // draw hands only
	case 'H':
		showHelp = !showHelp;
		break;
	case 'o': // open stream
	case 'O':
		kinect.open();
		break;
	case 'c': // close stream
	case 'C':
		kinect.close();
		break;
	case OF_KEY_UP: // up the kinect angle
		angle++;
		if(angle > 27){
			angle = 27;
		}
		kinect.setAngle(angle);
		break;
	case OF_KEY_DOWN: // down the kinect angle
		angle--;
		if(angle < -27){
			angle = -27;
		}
		kinect.setAngle(angle);
		break;
	}
}


//OscMesasge Sender test

void ofApp::sendOscMessage(string address){
	ofxOscMessage msg;
	msg.setAddress(address);

	msg.addIntArg(1);
	msg.addFloatArg(3.5f);
	msg.addFloatArg(ofGetElapsedTimef());
	sender.sendMessage(msg);
}


void ofApp::volumeControl(){
	
	maxDistance = maxDistance - 0.1f;
	minDistance = minDistance + 0.1f;
	
	float currentDistance = leftHand.distance(rightHand);
	
	if(currentDistance>maxDistance){
		maxDistance = currentDistance;
	}

	float newvol = currentDistance / maxDistance;

	oscVolumeControl(newvol);

}

void ofApp::oscVolumeControl(float volume){
	ofxOscMessage msg;
	msg.setAddress("/activelayer/audio/volume/values");
	msg.addFloatArg(volume);
	sender.sendMessage(msg);
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y) {
	mRotationY = (x - 512) / 5;
	mRotationX = (384 - y) / 5;
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
	
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){
}

//--------------------------------------------------------------
void ofApp::kinectPlugged(){
	bPlugged = true;
}

//--------------------------------------------------------------
void ofApp::kinectUnplugged(){
	bPlugged = false;
}

