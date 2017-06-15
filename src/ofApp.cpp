#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){	 
	
	ofSetVerticalSync(true);
	ofSetCircleResolution(80);
	ofBackground(54, 54, 54);	
	
    recorder.setup();
    
    ofxXmlSettings xml;
    xml.load("config.xml");
    float erdiLow = xml.getValue("config:erdiLow", 0.0);
    float erdiHigh = xml.getValue("config:erdiHigh", 0.0);
    float volLow = xml.getValue("config:volLow", 0.0);
    timeline.setup(volLow, erdiHigh, erdiLow);
    cout << erdiHigh << endl;
    cout << "erdiHigh" << endl;
    setupTimeline();
    
    twitter.setup("s/quotes",3);
    idleMumbler.setup("s/opinions",2);
    
    //timeline.start(); // dist sensor triggers this!
    
    gui.setup();
    gui.add(idleMaxVol.set("idle max vol", 0. , 0., 1.));
    //gui.add(erdiVol.set("erdi vol", 0. , 0., 1.));
    gui.add(twitterMaxVol.set("twitter vol", 0. , 0., 1.));
    
    gui.add(sampleDetectionLength.set("sampDetLength",1.3,0.,4.));
    
    gui.add(recorder.recGroup);
    
    gui.loadFromFile("settings.xml");
    
   // gui.setFillColor(ofColor::lightCoral);
   // gui.setHeaderBackgroundColor(ofColor::lightCoral);
    
    
    

    //gui.setWidthElements(180);
    serial.setup();
    ofShowCursor();
    
    serial.writeByte(5);
    timeline.start();
}


void ofApp::setupTimeline(){
    
        //timeline.addSound("opinions", 0 , 60, -1 , ""); // stream
      timeline.addSound("01_welcome_old", 0 , 2,  -1, "" );
     // timeline.addSound("01_intro_new", 50 , 2,  -1, "" );
    
      timeline.addSound("opinions", 2 , 2, -1 , "opinion"); // stream
    
    
        timeline.addSound("10_areyoustillthere", 20 , 3 , -1, "");
        timeline.addSound("05_question", 3 , 4 ,  -1, "question" ); // question
        timeline.addSilence(-2 , 4 , 5 , 6 , "detect"); // detect 1
        timeline.addSound("06_probe" , 5 , 7);
        timeline.addSilence(-2 , 7 , 8 , 6 , "detect"); // detect 1
        timeline.addSound("07_giveup" , 8 , 2 );
    
        timeline.addSilence(0.5 , 6 , 6 , 9 , "listen"); // listen to opinion now
        timeline.addSound("08_thankyou" , 9 , 2, -1, "thankyou");
        timeline.addSound("03_ohno" , 19 , 2, -1, "interruption");
    
    
        timeline.addSound("twitter_intro", 30 , 31, -1 , "twitter"); // stream
        timeline.addSound("twitter", 31 , 2, -1 , "twitter"); // stream
    
        timeline.addSound("quote_intro", 40 , 41, -1 , "quote"); // stream
        timeline.addSound("quotes", 41 , 2, -1 , "quote"); // stream
    
        timeline.addSound("10_goodbuy", 60 , 61 , -1, "goodBuy");
        timeline.addSilence(-2 , 61 , 2 , 12, "goodBuy"); // detect 1
        timeline.addSound("11_youarestillhere", 12 , 2 , -1 , "goodBuy");
    
//    timeline.addSound("01_welcome", 0 , 552,  -1, "welcome" ); // welcome
//    timeline.addSilence(2. , 552 , 551 , -1 , "welcome"); // shutup
//    
//    timeline.addSound("02_intro", 551 , 52, -1 , "intro"); // intro // could be before.
//    timeline.addSound("02_knock", 52 , 30,  -1, "knockknockSpeak" ); // welcome
//    timeline.addSilence(-2 , 30 , 32 , 1 , "knockknock"); // shutup
//    
//    timeline.addSound("03_ohnoKnock", 32 , 1 , -1, "ohnoKnock" ); // nope
//    
//    timeline.addSound("04_iwilltellyou", 1 , 2); // intro
//    timeline.addSound("opinions", 2 , 21, -1 , ""); // stream
//    timeline.addSound("quote_intro", 21 , 441, -1 , "quote"); // stream
//    timeline.addSound("quotes", 441 , 3); // stream
//    timeline.addSound("05_question", 3 , 4 ,  -1, "question" ); // question
//    timeline.addSilence(-2 , 4 , 5 , 6 , "detect"); // detect 1
//    timeline.addSound("06_probe" , 5 , 7);
//    timeline.addSilence(-2 , 7 , 8 , 6 , "detect"); // detect 1
//    timeline.addSound("07_giveup" , 8 , 10 );
//    
//    timeline.addSilence(0.5 , 6 , 6, 9 , "listen"); // listen to opinion now
//    timeline.addSound("08_thankyou" , 9 , 18, -1, "interruption");
//    
//    timeline.addSound("04_iwilltellyou", 18 , 10); // intro
//    
//    timeline.addSound("opinions", 10 , 10, -1 , "opinion"); // stream
//    
//    timeline.addSound("twitter_intro", 40 , 41, -1 , "twitter"); // stream
//    timeline.addSound("twitter", 41 , 10, -1 , "twitter"); // stream
//    
//    timeline.addSound("quote_intro", 400 , 401, -1 , "quote"); // stream
//    timeline.addSound("quotes", 401 , 10, -1 , "quote"); // stream
//    
//    //goodbuy
//    timeline.addSound("10_goodbuy", 11 , 13 , -1, "goodBuy");
//    timeline.addSilence(-2 , 13 , 100 , 12, "goodBuy"); // detect 1
//    timeline.addSound("11_youarestillhere", 12 , -2 , -1 , "goodBuy");
//
//    // interruption.
//    timeline.addSound("07_imsorry", 14 , 17 , -1 , "interruption");
//    timeline.addSilence(-2 , 17 , 106 , 15, "detect"); // detect 1
//    timeline.addSound("03_speaklouder" , 106 , -2, -1, "interruption");
//    
//    timeline.addSilence(0.5 , 15 ,  15 , 16, "listen"); // listen to opinion now
//    timeline.addSound("08_thankyou" , 16 , -2, -1, "interruption");
    
    
    timeline.defineEndPos(100);
    
}

//--------------------------------------------------------------
void ofApp::update(){
   //if(ofGetFrameNum() == 100)gui.loadFromFile("settings.xml"); // temporary
    

    // goodbuy can be interrupted buy a start 
    if( timeline.getName() == "goodBuy" )
       if( serial.start() || recorder.recording )
           timeline.swithDirection();
    
//    
//    if(!timeline.isPlaying){
//        float i = idleMaxVol;
//        float t = twitterMaxVol;
//        idleVol = CLAMP(idleVol+=0.02 ,0., i);
//        twitterVol = CLAMP(twitterVol+=0.02 ,0., t);
//        
//        
//
//    }
    
    if(serial.start() && startTimeline){
        //timeline.start();
        startTimeline = false;
        timeline.jumpToNext(0);
    }
    if(serial.stop() && !startTimeline){
        startTimeline = true;
        //timeline.start();
        timeline.jumpToNext(60);
        timeline.stop();
    }
    
    //PLAYING
//    else
//    {
//        bool recordInterruptionPos = false;
//        if(timeline.getName()!="interruption" &&
//           timeline.getName()!="goodbuy" &&
//           timeline.position!=17 &&
//           timeline.position!=15)recordInterruptionPos=true;
//        
//        if(serial.stop() && timeline.getName()!="goodBuy"){
//            if(recordInterruptionPos)
//                timeline.interruptionPos = timeline.position;
//            timeline.stop();
//        }
//        
//        // interruption
//        else if(serial.interrupt() || recorder.recording){
//            
//            if(timeline.getName() == "knockknock")
//                timeline.swithDirection();
//            
//            if(timeline.getName() == "knockknockSpeak" || timeline.getName() == "ohnoKnock"
//               || timeline.getName() == "knockknock")
//                timeline.interruptionPos = 1;
//            
//            else if(recordInterruptionPos){
//                
//                timeline.interruptionPos = timeline.position;
//            }
//            timeline.jumpToNext(14);
//        }
        
        
        if(serial.interrupt()){
            timeline.jumpToNext(6);
        }
        else if(recorder.getVolume()){
            recordingTimer+=ofGetLastFrameTime();
            if(recordingTimer>.25){
                timeline.jumpToNext(6);
             //   cout << recordingTimer << endl;
            }
        }else{
           // cout << recordingTimer << endl;
            recordingTimer = 0.;
        }
        
        
        if(timeline.getName() == "detect" && recorder.recording){ // detect
            timeline.swithDirection();
        }
        if(timeline.getName() == "listen" && !recorder.recording ){
            
            if(!recorder.save)
                timeline.jumpToNext(19);
            else
                timeline.swithDirection();
        }
        

        
        
        
//        if(timeline.getName() == "opinion" || timeline.getName() == "quote" ){
//            float i = idleMaxVol;
//            idleVol = CLAMP(idleVol+=0.02 ,0., i);
//        }
//        else
        
        idleVol = 0.01;
        twitterVol = 0.01;
        
   // }
    
    
    timeline.update(sampleDetectionLength);
    //recorder.update(timeline.isSilent());
    recorder.update(true);
    
    twitter.update(twitterVol, timeline.isPlaying);
    idleMumbler.update(idleVol, timeline.isPlaying);
    
    if(recorder.getVolume())serial.writeByte(1);
    
    serial.update();
    
#ifdef TARGET_OSX
    //cout<< "i am osx"<< endl;
#else
   
    //cout<< "i am not"<< endl;
#endif
    
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

    
    if(key == 'q'){
        timeline.interruptionPos = timeline.position;
        timeline.jumpToNext(14);
    }
    if(key == 'b'){
        // interruption
        if(
           timeline.position != 14)
        {
            if(timeline.getName() == "knockknock")timeline.swithDirection();
            
            timeline.interruptionPos = timeline.position;
            timeline.jumpToNext(14);
            
        }
    
    
    }
    if(key == 's' && !timeline.isPlaying){
        //setupTimeline();
        timeline.start();
        timeline.jumpToNext(0);
    }
    if(key == 's'){
        //setupTimeline();
        //timeline.start();
        timeline.jumpToNext(0);
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

