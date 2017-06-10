#pragma once

#include "ofMain.h"
#include "timeline.h"
#include "mumble.h"
#include "serialRead.h"
#include "ofxGui.h"
#include "soundRecorder.h"

class ofApp : public ofBaseApp{
	
	public:
		
		void setup();
		void update();
		void draw();
		
		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void mouseEntered(int x, int y);
		void mouseExited(int x, int y);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
				
    SoundRecorder recorder;
  
    Timeline timeline;
    void setupTimeline();
    
    
    Mumbler twitter;
    SerialRead serial;

    
    double interruptionTimer;
    int interruptionPos;
    
    Mumbler idleMumbler;
    float idleVol = 0.;
    
    ofxPanel gui;
    ofParameter<float>idleMaxVol, twitterVol, erdiVol,sampleDetectionLength;


};
