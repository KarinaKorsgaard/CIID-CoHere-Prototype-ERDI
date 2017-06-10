#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){	 
	
	ofSetVerticalSync(true);
	ofSetCircleResolution(80);
	ofBackground(54, 54, 54);	
	
    recorder.setup();
    
    timeline.setup(false);
    
    setupTimeline();
    
    twitter.setup("s/quotes",3);
    idleMumbler.setup("s/opinions",2);
    
    //timeline.start(); // dist sensor triggers this!
    
    gui.setup();
    gui.add(idleMaxVol.set("idle max vol", 0. , 0., 1.));
    gui.add(erdiVol.set("erdi vol", 0. , 0., 1.));
    gui.add(twitterMaxVol.set("twitter vol", 0. , 0., 1.));
    
    gui.add(sampleDetectionLength.set("sampDetLength",1.3,0.,3.));
    
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
    
    
    timeline.addSound("01_welcome", 0 , 30,  -1, "noInterrupt" ); // welcome
    timeline.addSilence(sampleDetectionLength , 30 , 32 , 31 , "detect"); // shutup
    
    timeline.addSound("03_yay", 31 , 1 , -1, "noInterrupt" ); // yay, you said something
    timeline.addSound("03_ohno", 32 , 1 , -1, "noInterrupt" ); // nope
    
    timeline.addSound("02_intro", 1 , 51); // intro
    timeline.addSound("04_intro2", 51 , 2); // intro
    
    //timeline.addSound("09_stream", 2 , 20, -1 , "stream"); // stream
    timeline.addSound("opinions", 2 , 21, -1 , ""); // stream
    
    // add im going to read aloud from twitter now
    timeline.addSound("quotes", 21 , 3); // stream
    
    timeline.addSound("05_question", 3 , 4 ,  -1, "noInterrupt" ); // question
    
    timeline.addSilence(sampleDetectionLength , 4 , 5 , 6 , "detect"); // detect 1
    
    timeline.addSound("06_probe" , 5 , 7);
    timeline.addSilence(0.5 , 6 , 6, 9 , "listen"); // listen to opinion now
    
    timeline.addSilence(sampleDetectionLength , 7 , 8 , 6 , "detect"); // detect 1
    
    timeline.addSound("07_giveup" , 8 , 10 );
    
   
    timeline.addSound("08_thankyou" , 9 , 10);
    
    //timeline.addSound("09_stream", 10 , 40 , -1 , "stream");
    timeline.addSound("quotes", 40 , 10, -1 , "quote"); // stream
    timeline.addSound("opinions", 10 , 10, -1 , "opinion"); // stream
    //timeline.addString("03_STREAM.txt", 10 , 10 , -1 , "stream");
    
    // distance sensor will force position to 11
    timeline.addSound("10_goodbuy", 11 , 13 );
    timeline.addSilence(6 , 13 , 100 , 12, "goodBuy"); // detect 1
    timeline.addSound("11_youarestillhere", 12 , 10 );
    
    
    // interruption.
    timeline.addSound("07_imsorry", 14 , 17 );
    timeline.addSilence(sampleDetectionLength , 17 , interruptionPos , 15, "detect"); // detect 1
    timeline.addSilence(0.5 , 15 ,  15 , 16, "listen"); // listen to opinion now
    timeline.addSound("08_thankyou" , 16 , interruptionPos, -1, "interruption");
    
    timeline.defineEndPos(11);
}

//--------------------------------------------------------------
void ofApp::update(){
   if(ofGetFrameNum() == 100)gui.loadFromFile("settings.xml"); // temporary
    
    timeline.update(erdiVol);
    twitter.update(twitterVol);
    idleMumbler.update(idleVol);
    recorder.update(timeline.isSilent());
    
    // goodbuy can be interrupted buy a start 
    if( timeline.getName() == "goodBuy" )
       if( serial.start() || recorder.recording )
           timeline.swithDirection();
    
    if(serial.start() && !timeline.isPlaying){
        //setupTimeline();
        timeline.start();
    }
    if(serial.stop() && timeline.isPlaying && timeline.position != 11 && timeline.position != 13)timeline.stop();
    
    // NOT PLAYING
    if(!timeline.isPlaying){
        float i = idleMaxVol;
        float t = twitterMaxVol;
        idleVol = CLAMP(idleVol+=0.02 ,0., i);
        twitterVol = CLAMP(twitterVol+=0.02 ,0., t);
    }
    
    //PLAYING
    else
    {
        if(timeline.getName() == "detect" && recorder.recording){ // detect
            timeline.swithDirection();
        }
        if(!recorder.recording && timeline.getName() == "listen"){
            timeline.swithDirection();
        }
        
        // interruption
        if(serial.interrupt() &&
           timeline.position != 14 &&
           !timeline.isSilent() &&
           timeline.getName() != "detect" &&
           timeline.getName() != "noInterrupt")
        {
            interruptionTimer += ofGetLastFrameTime();
            if(interruptionTimer>0.2){
                interruptionPos = timeline.position;
                timeline.setNextPosition(16, interruptionPos);
                timeline.setNextPosition(17, interruptionPos);
                timeline.jumpToNext(14);
                serial.writeByte(1);
                interruptionTimer = 0;
            }
        }
        
        
        if(timeline.getName() == "opinion" || timeline.getName() == "quote" ){
            float i = idleMaxVol;
            idleVol = CLAMP(idleVol+=0.02 ,0., i);
        }
        else
            idleVol = 0.;
        
        twitterVol = 0.;
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
        //setupTimeline();
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

