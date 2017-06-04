#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){	 
	
	ofSetVerticalSync(true);
	ofSetCircleResolution(80);
	ofBackground(54, 54, 54);	
	
	// 0 output channels, 
	// 2 input channels
	// 44100 samples per second
	// 256 samples per buffer
	// 4 num buffers (latency)
	
	soundStream.printDeviceList();
	
	//if you want to set a different device id 
	soundStream.setDeviceID(0); //bear in mind the device id corresponds to all audio devices, including  input-only and output-only devices.
	
	int bufferSize = 256;
	
	
	left.assign(bufferSize, 0.0);
	right.assign(bufferSize, 0.0);
	//volHistory.assign(400, 0.0);
	
	bufferCounter	= 0;
	drawCounter		= 0;
	smoothedVol     = 0.0;
	scaledVol		= 0.0;

	soundStream.setup(this, 0, 2, 44100, bufferSize, 4);
    
    timeline.setup(false);
    
    setupTimeline();
    
    twitter.setup("twitter",3);
    idleMumbler.setup("s/opinions",4);
    
    //timeline.start(); // dist sensor triggers this!
    
    gui.setup();
    gui.add(idleMaxVol.set("idle max vol", 0. , 0., 10.));
    gui.add(erdiVol.set("erdi vol", 0. , 0., 10.));
    gui.add(twitterVol.set("twitter vol", 0. , 0., 10.));
    gui.loadFromFile("settings.xml");
    
}


void ofApp::setupTimeline(){
    timeline.addSound("s/welcome", 0, 1);
    timeline.addSound("s/intro", 1, 2);
    
  //  timeline.addString("01_INTRO.txt", 0 , 1); // welcome
  //  timeline.addString("02_INTRO.txt", 1 , 2); // intro
    timeline.addString("03_STREAM.txt", 2 , 3); // stream
    timeline.addString("04_QUESTION.txt", 3 , 4); // question
    
    timeline.addSilence(3.0 , 4 , 5, 6); // detect 1
    
    timeline.addString("05_PROBE.txt" , 5 , 7);
    timeline.addSilence(3.0 , 7 , 8 , 6 ); // detect 1
    
    timeline.addString("01_GIVEUP.txt" , 8 , 10 );
    
    timeline.addSilence(10.0 , 6 , 9); // listen to opinion now
    timeline.addString("06_THANKYOU.txt" , 9 , 10);
    

    timeline.addString("03_STREAM.txt", 10 , 10 , -1 , "stream");
    
    
    // distance sensor will force position to 11
    timeline.addString("07_GOODBUY.txt", 11 , 13 , 12 );
    timeline.addString("01_YOUARESTILLHERE.txt", 12 , 10 );
    
    timeline.defineEndPos(11);
}

//--------------------------------------------------------------
void ofApp::update(){
    
    if(!timeline.isPlaying){
        float m = idleMaxVol;
        idleVol = CLAMP(idleVol+=0.02 ,0., m);
    }else{
        idleVol = 0.;
    }
    timeline.update(erdiVol);
	//lets scale the vol up to a 0-1 range 
	scaledVol = ofMap(smoothedVol, 0.0, 0.17, 0.0, 1.0, true);

	//lets record the volume into an array
	volHistory.push_back( scaledVol );
	
	//if we are bigger the the size we want to record - lets drop the oldest value
	if( volHistory.size() >= 10 ){
		volHistory.erase(volHistory.begin(), volHistory.begin()+1);
        step++;
	}
    
    twitter.update(twitterVol);
    idleMumbler.update(idleVol);
}



//--------------------------------------------------------------
void ofApp::draw(){
    
	ofSetColor(225);
	ofDrawBitmapString("AUDIO INPUT EXAMPLE", 32, 32);

	ofNoFill();
	
		
	// draw the average volume:
	ofPushStyle();
		ofPushMatrix();
		ofTranslate(565, 170, 0);
			
		ofSetColor(225);
		ofDrawBitmapString("Scaled average vol (0-100): " + ofToString(scaledVol * 100.0, 0), 4, 18);
		ofDrawRectangle(0, 0, 400, 400);
		
		ofSetColor(245, 58, 135);
		ofFill();		
		ofDrawCircle(200, 200, scaledVol * 190.0f);
		
		//lets draw the volume history as a graph
		ofBeginShape();
		for (unsigned int i = 0; i < volHistory.size(); i++){
			if( i == 0 ) ofVertex(i, 400);

			ofVertex(i, 400 - volHistory[i] * 70);
			
			if( i == volHistory.size() -1 ) ofVertex(i, 400);
		}
		ofEndShape(false);		
			
		ofPopMatrix();
	ofPopStyle();
	
	drawCounter++;
	
	ofSetColor(225);
	string reportString = "buffers received: "+ofToString(bufferCounter)+"\ndraw routines called: "+ofToString(drawCounter)+"\nticks: " + ofToString(soundStream.getTickCount());
	ofDrawBitmapString(reportString, 32, 589);
		
    
    
    gui.draw();
    
    ofTranslate(0, gui.getHeight()+50);
    timeline.draw();
}

//--------------------------------------------------------------
void ofApp::audioIn(float * input, int bufferSize, int nChannels){	
	
	float curVol = 0.0;
	
	// samples are "interleaved"
	int numCounted = 0;	

	//lets go through each sample and calculate the root mean square which is a rough way to calculate volume	
	for (int i = 0; i < bufferSize; i++){
		left[i]		= input[i*2]*0.5;
		right[i]	= input[i*2+1]*0.5;

		curVol += left[i] * left[i];
		curVol += right[i] * right[i];
		numCounted+=2;
	}
	
	//this is how we get the mean of rms :) 
	curVol /= (float)numCounted;
	
	// this is how we get the root of rms :) 
	curVol = sqrt( curVol );
	
	smoothedVol *= 0.93;
	smoothedVol += 0.07 * curVol;
	
	bufferCounter++;
	
}

//--------------------------------------------------------------
void ofApp::keyPressed  (int key){ 
//	if( key == 's' ){
//		soundStream.start();
//	}
//	
//	if( key == 'e' ){
//		soundStream.stop();
//	}
    
    if(key == 'q')timeline.swithDirection();
    
    if(key == 's'){
        setupTimeline();
        timeline.start();
    }
    
    if(key == 'S')timeline.stop();
    
    if(key == 't')twitterVol+=0.1;
    if(key == 'T')twitterVol-=0.1;
}



//--------------------------------------------------------------






//--------------------------------------------------------------
void ofApp::keyReleased(int key){
	
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){
	
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
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}

