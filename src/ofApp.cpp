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
    

    gui.setup();
    gui.add(idleMaxVol.set("idle max vol", 0. , 0., 1.));
    gui.add(twitterMaxVol.set("twitter vol", 0. , 0., 1.));
    gui.add(sampleDetectionLength.set("sampDetLength",1.3,0.,4.));
    gui.add(recorder.recGroup);
    
    gui.loadFromFile("settings.xml");
    serial.setup();
    ofShowCursor();
    timeline.start();

}


void ofApp::setupTimeline(){
    
//setup timeline.
    //addSound(DIRECTORY NAME, position, next position, alternative next, name of entry)
    timeline.addSound("01_welcome_old", 0 , 2,  -1, "" );
    timeline.addSound("opinions", 2 , 2, -1 , "opinion"); // stream
    timeline.addSound("05_question", 20 , 4 ,  -1, "question" ); // question
    timeline.addSilence(-2 , 4 , 5 , 6 , "detect"); // detect 1
    timeline.addSound("06_probe" , 5 , 7);
    timeline.addSilence(-2 , 7 , 8 , 6 , "detect"); // detect 1
    timeline.addSound("07_giveup" , 8 , 2 );
    
    timeline.addSilence(0.5 , 6 , 6 , 9 , "listen"); // listen to opinion now
    timeline.addSound("03_ohno" , 19 , 119, -1, "interruption");
    timeline.addSilence(2. , 119 , 2, -1, "interruption");
    
    timeline.addSound("03_sense", 80 , 2, -1 , "sense"); // stream
    
    timeline.addSound("twitter_intro", 30 , 31, -1 , "twitter"); // stream
    timeline.addSound("twitter", 31 , 2, -1 , "twitter"); // stream
    
    timeline.addSound("quote_intro", 40 , 41, -1 , "quote"); // stream
    timeline.addSound("quotes", 41 , 2, -1 , "quote"); // stream
    
    timeline.addSound("10_goodbuy", 60 , 61 , -1, "goodBuy");
    timeline.addSilence(8. , 61 , 2 , 12, "goodBuy"); // detect 1
    timeline.addSound("11_youarestillhere", 12 , 2 , -1 , "goodBuy");
    
    timeline.addSound("whatdoyouthink", 90 , 2, -1 , "what"); // stream

    timeline.addSound("whatIheardyousay", 9 , 901, -1 , "what"); // stream
    timeline.addSound("opinions", 901 , 902, -1 , "repeat"); // stream
    timeline.addSound("someoneelsesaid", 902 , 2, -1 , ""); // stream
    
    timeline.defineEndPos(100);
    
}

//--------------------------------------------------------------
void ofApp::update(){

    // goodbuy can be interrupted buy a start 
    if( timeline.getName() == "goodBuy" && startTimeline){
        if( serial.start() || recorder.recording ){
           timeline.jumpToNext(12);
           startTimeline = false;
        }
    }
    
    // send data to arduino to turn on the mic LED\s
    if( (timeline.getName() == "listen" ||
         timeline.getName() == "detect" ||
         timeline.getName() == "question")&&
         sendbyte
       ){
        serial.writeByte(6);
        sendbyte = false;
        cout << "send 6"<< endl;
        
    }// else send data to turn of leds
    else if (!sendbyte &&
             timeline.getName() != "listen" &&
             timeline.getName() != "detect"&&
             timeline.getName() != "question"
             ){
        serial.writeByte(7);
        sendbyte = true;
        cout << "send 7"<< endl;
    }
    
    //if we get a start from arduino, start the timeline
    if(serial.start() && startTimeline ){
        startTimeline = false;
        if( timeline.getName() !="goodBuy" ){
            startTimeline = false;
            timeline.jumpToNext(0);
        }
    }
    // stop from arduiono starts the Goodbuy sequence
    if(serial.stop() && !startTimeline){
        startTimeline = true;
        timeline.jumpToNext(60);
        timeline.stop();
    }
    
    // if the interrupts -- i dont really remeber where this comes from...
    if(serial.interrupt()){
        timeline.jumpToNext(6);
    }
    //also jumo to silence sequence if the mic detects a sounds
    else if(recorder.getVolume() && timeline.getName()!="question"){
        recordingTimer+=ofGetLastFrameTime();
        if(recordingTimer>.15){
            timeline.jumpToNext(6);
        }
    }else{
        recordingTimer = 0.;
    }
    
    // switch direction so she starts lisetening, if whe hears a sound while detectivng sound
    if(timeline.getName() == "detect" && recorder.recording){ // detect
        timeline.swithDirection();
    }
    
    // when the recorder stops recording jump to either .that was a bit short. or .thank you!!/
    if(timeline.getName() == "listen" && !recorder.recording ){
        if(!recorder.save)
            timeline.jumpToNext(19);
        else
            timeline.jumpToNext(9);
    }

    
    idleVol = 0.01;
    twitterVol = 0.01;
    

    //update timeline
    timeline.update(sampleDetectionLength);
    recorder.update(true);
    
    twitter.update(twitterVol, timeline.isPlaying);
    idleMumbler.update(idleVol, timeline.isPlaying);
    
    if(recorder.getVolume())serial.writeByte(1);
    
    serial.update();

}



//--------------------------------------------------------------
void ofApp::draw(){
    
    ofBackground(0,0,100);
    
	ofSetColor(225,0,0);

	ofNoFill();
    gui.draw();
    timeline.draw(200, 0);
    recorder.gui.draw();
    
    if(recorder.getVolume()){
        ofSetColor(0,0,255);
        ofDrawBitmapString("Sample length: "+std::to_string(recorder.sampleLength), 10, 300);
    }
    ofDrawBitmapString("volume: "+std::to_string(recorder.vol), 200, 320);


    recorder.drawCurve();

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

