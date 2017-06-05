#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){	 
	
	ofSetVerticalSync(true);
	ofSetCircleResolution(80);
	ofBackground(54, 54, 54);	
	
    recorder.setup();
    
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
  //  timeline.addSound("s/welcome", 0, 1); //
  //  timeline.addSound("s/intro", 1, 2);
    
    timeline.addString("01_INTRO.txt", 0 , 30); // welcome
    timeline.addSilence(5.0 , 30 , 31 ); // shutup
    timeline.addString("yay.txt", 31 , 1); // yay, you said something
    
    timeline.addString("02_INTRO.txt", 1 , 2); // intro
    
    timeline.addString("03_STREAM.txt", 2 , 20); // stream
    timeline.addString("Quotes.txt", 20 , 21); // stream
    timeline.addString("03_STREAM.txt", 21 , 3); // stream
    
    timeline.addString("04_QUESTION_coexist.txt", 3 , 4); // question
    
    timeline.addSilence(4.0 , 4 , 5 , 6); // detect 1
    
    timeline.addString("05_PROBE.txt" , 5 , 7);
    
    timeline.addSilence(4.0 , 7 , 8 , 6 ); // detect 1
    
    timeline.addString("01_GIVEUP.txt" , 8 , 10 );
    
    timeline.addSilence(0.5 , 6 , 6, 9); // listen to opinion now
    timeline.addString("06_THANKYOU.txt" , 9 , 10);
    
    timeline.addString("03_STREAM.txt", 10 , 10 , -1 , "stream");
    
    
    // distance sensor will force position to 11
    timeline.addString("07_GOODBUY.txt", 11 , 13 , 12 );
    timeline.addString("01_YOUARESTILLHERE.txt", 12 , 10 );
    
    
    // interruption.
    //timeline.addString("07_IMSORRY.txt", 14 , 15 , 12 );
    //timeline.addSilence(0.5 , 15 , 15 , 9); // say something
    
    
    timeline.defineEndPos(11);
}

//--------------------------------------------------------------
void ofApp::update(){
    
    
    timeline.update(erdiVol);
    twitter.update(twitterVol);
    idleMumbler.update(idleVol);
    
    recorder.update(timeline.isSilent());
    
    
    if(!timeline.isPlaying){
        float m = idleMaxVol;
        idleVol = CLAMP(idleVol+=0.02 ,0., m);
        
        
    }else{
        if(timeline.position == 4 || timeline.position == 7){ // detect
            if(recorder.sampleLength > 1.5f && recorder.recording)timeline.swithDirection();
        }
        
        idleVol = 0.;
    }
    
//    // interrupt
//    if(recorder.sampleLength > 2.f){
//        timeline.jumpToNext(14); // did you say somehitng??
//    }
//    
    if(recorder.doneRecording){
        //timeline.swithDirection();
        recorder.doneRecording = false;
        
        if(timeline.position == 6)
            timeline.swithDirection();
    }
    
}



//--------------------------------------------------------------
void ofApp::draw(){
    
	ofSetColor(225);

	ofNoFill();
    
    //ofPushMatrix();
    gui.draw();
    
    //ofTranslate(gui.getWidth() + 50, 0);
    timeline.draw(gui.getWidth() + 50, 0);
    
    //ofTranslate(gui.getWidth() + 50, 0);
    recorder.gui.draw();
    
    if(recorder.recording){
        ofSetColor(100,255,100);
        ofDrawBitmapString("Sample length: "+std::to_string(recorder.sampleLength), 10, 300);
    }
    else{
        ofSetColor(255,100,100);
        ofDrawBitmapString("volume: "+std::to_string(recorder.getVolume()), 10, 300);
    }
    ofDrawCircle(600, 300, recorder.getVolume());
}


//--------------------------------------------------------------
void ofApp::keyPressed  (int key){ 

    
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

