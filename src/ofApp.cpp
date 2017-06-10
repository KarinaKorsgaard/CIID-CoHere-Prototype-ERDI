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
    gui.add(idleMaxVol.set("idle max vol", 0. , 0., 1.));
    gui.add(erdiVol.set("erdi vol", 0. , 0., 1.));
    gui.add(twitterVol.set("twitter vol", 0. , 0., 1.));
    
    gui.add(sampleDetectionLength.set("sampleDetectionLength",1.3,0.,3.));
    
    gui.add(recorder.recGroup);
    
    gui.loadFromFile("settings.xml");
    
   // gui.setFillColor(ofColor::lightCoral);
   // gui.setHeaderBackgroundColor(ofColor::lightCoral);
    
    //gui.setWidthElements(180);
    serial.setup();
    ofShowCursor();
    
}


void ofApp::setupTimeline(){
  //  timeline.addSound("s/welcome", 0, 1); //
  //  timeline.addSound("s/intro", 1, 2);
    
//    timeline.addSound("01_welcome", 0 , 30); // welcome
//    timeline.addSilence(5.0 , 30 , 31 ); // shutup
//    timeline.addSound("03_yay", 31 , 1 ); // yay, you said something
//    
//    timeline.addSound("02_intro", 1 , 51); // intro
//    timeline.addSound("04_intro2", 51 , 2); // intro
//    
//    timeline.addSound("09_stream", 2 , 20); // stream
//    timeline.addSound("opinions", 20 , 21); // stream
//    timeline.addSound("quotes", 21 , 22); // stream
//    timeline.addSound("09_stream", 22 , 23); // stream
//    timeline.addSound("opinions", 23 , 3); // stream
//    
//    timeline.addSound("05_question", 3 , 4); // question
//    
//    timeline.addSilence(4.0 , 4 , 5 , 6); // detect 1
//    
//    timeline.addSound("06_probe" , 5 , 7);
//    
//    timeline.addSilence(4.0 , 7 , 8 , 6 ); // detect 1
//    
//    timeline.addSound("07_giveup" , 8 , 10 );
//    
//    timeline.addSilence(0.5 , 6 , 6, 9); // listen to opinion now
//    timeline.addSound("08_thankyou" , 9 , 10);
//    
//    timeline.addSound("09_stream", 10 , 40 , -1 , "stream");
//    timeline.addSound("opinions", 40 , 10, -1 , "opinion"); // stream
//    //timeline.addString("03_STREAM.txt", 10 , 10 , -1 , "stream");
//    
//    // distance sensor will force position to 11
//    timeline.addSound("10_goodbuy", 11 , 13 , 12 );
//    timeline.addSound("11_youarestillhere", 12 , 10 );
//    
    
    
    timeline.addSound("01_welcome", 0 , 30); // welcome
    timeline.addSilence(4.0 , 30 , 32 , 31 ); // shutup
    timeline.addSound("03_yay", 31 , 1  ); // yay, you said something
    timeline.addSound("03_ohno", 32 , 1  ); // nope
    
    timeline.addSound("02_intro", 1 , 51); // intro
    timeline.addSound("04_intro2", 51 , 2); // intro
    
    timeline.addSound("09_stream", 2 , 20); // stream
    timeline.addSound("opinions", 20 , 21); // stream
    timeline.addSound("quotes", 21 , 3); // stream
   // timeline.addString("09_stream.txt", 22 , 23); // stream
   // timeline.addString("opinions.txt", 23 , 3); // stream
    
    timeline.addSound("05_question", 3 , 4); // question
    
    timeline.addSilence(4.0 , 4 , 5 , 6); // detect 1
    
    timeline.addSound("06_probe" , 5 , 7);
    
    timeline.addSilence(4.0 , 7 , 8 , 6 ); // detect 1
    
    timeline.addSound("07_giveup" , 8 , 10 );
    
    timeline.addSilence(0.5 , 6 , 6, 9); // listen to opinion now
    timeline.addSound("08_thankyou" , 9 , 10);
    
    timeline.addSound("09_stream", 10 , 40 , -1 , "stream");
    timeline.addSound("opinions", 40 , 10, -1 , "opinion"); // stream
    //timeline.addString("03_STREAM.txt", 10 , 10 , -1 , "stream");
    
    // distance sensor will force position to 11
    timeline.addSound("10_goodbuy", 11 , 13 , 12 );
    timeline.addSound("11_youarestillhere", 12 , 10 );
    
    
    // interruption.
    timeline.addSound("07_imsorry", 14 , 6 );

    
    
    timeline.defineEndPos(11);
}

//--------------------------------------------------------------
void ofApp::update(){
    
    
    timeline.update(erdiVol);
    twitter.update(twitterVol);
    idleMumbler.update(idleVol);
    
    if(serial.start()){
        setupTimeline();
        timeline.start();
    }
    
    bool record;
    int t = timeline.position;

    recorder.update(timeline.isSilent());

    if(recorder.getVolume()){
        timeline.jumpToNext(14);
        serial.writeByte(1);
    }
    
    if(!recorder.getVolume() && timeline.position == 15){
        timeline.swithDirection();
    }
    
    if(serial.stop())timeline.stop();
    
    if(!timeline.isPlaying){
        float m = idleMaxVol;
        idleVol = CLAMP(idleVol+=0.02 ,0., m);
        
    }else{
        if(timeline.position == 4 || timeline.position == 7 || timeline.position == 30){ // detect
            
            if(recorder.recording)timeline.swithDirection();
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
    
    serial.update();
}



//--------------------------------------------------------------
void ofApp::draw(){
    
    ofBackground(0,0,100);
    
	ofSetColor(225,0,0);

	ofNoFill();
    gui.draw();
    
    //ofTranslate(gui.getWidth() + 50, 0);
    timeline.draw(200, 0);
    
    //ofTranslate(gui.getWidth() + 50, 0);
    recorder.gui.draw();
    
    if(recorder.getVolume()){
        ofSetColor(0,0,255);
        ofDrawBitmapString("Sample length: "+std::to_string(recorder.sampleLength), 10, 300);
    }
    ofDrawBitmapString("volume: "+std::to_string(recorder.vol), 200, 320);

    
    
    recorder.drawCurve();
   // ofTranslate(0,400);
   // recorder.drawCurve();
    ofTranslate(0, 200);
    ofDrawBitmapString("q - jump to question", 10, 20*1);
    ofDrawBitmapString("t T twitter vol", 10, 20*2);
    ofDrawBitmapString("s S start stop", 10, 20*3);
    ofDrawBitmapString("e E erdi vol", 10, 20*4);
    ofDrawBitmapString("r R threshold", 10, 20*5);
    ofDrawBitmapString("w W wait", 10, 20*6);
    ofDrawBitmapString("m M min sample lenght", 10, 20*7);

    
    
}


//--------------------------------------------------------------
void ofApp::keyPressed  (int key){ 

    
    if(key == 'q')timeline.jumpToNext(3);
    
    if(key == 's'){
        setupTimeline();
        timeline.start();
    }
    if(key == 'S')timeline.stop();
    
    if(key == 't')twitterVol+=0.1;
    if(key == 'T')twitterVol-=0.1;
    
    if(key == 'e')erdiVol+=0.1;
    if(key == 'E')erdiVol-=0.1;
    
    if(key == 'r')recorder.threshold+=0.1;
    if(key == 'R')recorder.threshold-=0.1;
    
    if(key == 'w')recorder.wait+=0.1;
    if(key == 'W')recorder.wait-=0.1;
    
    if(key == 'm')recorder.minSampleLength+=0.1;
    if(key == 'M')recorder.minSampleLength-=0.1;
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

