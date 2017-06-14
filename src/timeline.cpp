//
//  timeline.cpp
//  Erdi_V1
//
//  Created by Karina Jensen on 02/06/17.
//
//

#include "timeline.h"


//--------------------------------------------------------------
void Timeline::setup(bool loop){
    looping = loop;
    path = "text/";
}

//--------------------------------------------------------------

//--------------------------------------------------------------
void Timeline::update(float vol, float sampleDetectionLength){
    

    
    
    if(p_vol != vol){
        sound.setVolume(vol*volume);
        p_vol = vol;
    }
    
    if(isValid && isPlaying){
        
        time+=ofGetLastFrameTime();
        float duration = entries[position].duration[entries[position].indx];
        if(duration == -2)duration = sampleDetectionLength;
        
        if(time > duration){
            loadNewEntry();
        }
    }
}


void Timeline::loadNewEntry(){
    //ofLogNotice("new entry!");

    indxJump();
    
    position = entries[position].swithDirection ? entries[position].optionNext : entries[position].next;
    if(position == -2){
        position = interruptionPos;
    }
    isValid = position != endPos;
    
    entries[position].swithDirection = false;
    time = 0.;
    

    
    
    
    
    
    if(entries[position].name == "opinion"){
        if(ofRandom(1)>0.7)
            position = ofRandom(1)>0.5 ? 40 : 400;
    }
    volume = getName() == "opinion" ? 1. : .3f;
    
    sound.stop();
    playSound();
}



void Timeline::draw(int x, int y){
    ofPushMatrix();
    ofTranslate(x, y);
    ofFill();
    


    if(isValid){
        
        ofDrawBitmapString( entries[position].name , 10, 10);
        ofDrawBitmapString( st(position) + entries[position].file[entries[position].indx] , 10, 20);
        ofDrawBitmapString( st(entries[position].duration[entries[position].indx]) + "current duration" , 10, 30);
        ofDrawBitmapString(st(time) + "current time", 10, 40);
        int next =entries[position].swithDirection ? entries[position].next : entries[position].optionNext;
        ofDrawBitmapString(st(next)+ "next position", 10, 50);
        
        for(int i = 0; i<messages.size();i++){
            ofDrawBitmapString(messages[i], 10, 60 + 10*i);
        }
       
    }else isPlaying = false;
    ofPopMatrix();
}


//--------------------------------------------------------------
void Timeline::start(){
    
    if(!isPlaying){
        
        
        interruptionPos = 0;
        
    	isPlaying = true;
    	time = 0.f;
    	position = 0;
        isValid = true;
        volume = 0.3;
        
    	cout<<"start"<<endl;
    
    	//isValid = entries.find(0)!=entries.end();
    
    	if(!isValid)messages.push_back("no start entry found!, abort");

    	map<int, entry>::iterator it;
    	for (it = entries.begin(); it != entries.end(); it++)
    	{
        	it->second.isPlayed = false;
        	it->second.swithDirection = false;
    	}
    
    	playSound();
	}
}

void Timeline::defineEndPos(int p){
    endPos = p;
}
void Timeline::setNextPosition(int pos, int next){
    entries.find(pos)->second.next = next;
}

void Timeline::jumpToNext(int p){

    position = p == -1 ? position : p;
    time = 0.f;
    sound.stop();
    
    indxJump();
    
    playSound();
    
}


void Timeline::addSilence(float duration, int position, int next, int optionNext, string name){
    
    entry e = *new entry;
    e.name = name;
    e.next = next;
    e.optionNext = optionNext == -1 ? next : optionNext;
    e.file.resize(1);
    e.duration.resize(1);
    e.duration[0] = duration;
    e.isSilence = true;
    e.isPlayed = false;
    e.indx = 0;
    
    entries[position] = e;
}

string Timeline::getName(){
    return entries[position].name;
}

//--------------------------------------------------------------
void Timeline::addSound(string _dir, int position, int next, int optionNext, string name){

    checkEntries(position);

    
    ofDirectory dir;
    dir.listDir("s/"+_dir);
    
   // dir.allowExt("mp3");
    dir.allowExt("wav");
   // dir.allowExt("ogg");
    
    entry e = *new entry;
    e.duration.resize(dir.size());
    e.file.resize(dir.size());
    
    cout << e.file.size() << endl;
    
    for (int i = 0; i<dir.size() ; i++){
        
        string filpath = dir.getPath(i);
    
        
        e.file[i] = filpath;
        
        ofSoundPlayer p;
        
        bool cut = false;
        
        if(p.load(filpath))cut = true;
        else p.load("defaultSound.wav");
        
        p.play();
        p.setPosition(0.9999999f);
        int ms = p.getPositionMS();
        p.setPosition(0);
        p.stop();
        p.unload();
        float duration = float(ms)/1000;
        
        //printf("SOUND LENGTH: %i\n\n", ms);
        
        e.isSound = true;
        e.duration[i] = duration;
        e.isPlayed = false;
        
        e.name = name;
        e.next = next;
        e.optionNext = optionNext == -1 ? next : optionNext;
        e.indx = floor(ofRandom(dir.size()));
        
        entries[position] = e;
        

    }

}

//--------------------------------------------------------------
void Timeline::addString(string file, int position, int next, int optionNext, string name){
    
//    string filePath = file;
//    checkEntries(position);
//    
//    if(ofFile::doesFileExist("text/"+filePath)){
//        
//        entry e = *new entry;
//        e.name = name;
//        e.file = getLine(filePath);
//        e.isString = true;
//        e.duration = float(e.file.length()) * 0.07;
//        if(e.duration < 0.1)messages.push_back(string("short file added %d", e.duration));
//        e.isPlayed = false;
//        e.optionNext = optionNext == -1 ? next : optionNext;
//        e.next = next;
//        e.position = position;
//        
//        entries[position] = e;
//        
//    }
//    else{
//        cout<<("ERROR, could not find file")<<endl;
//    }
}

//--------------------------------------------------------------
void Timeline::swithDirection(){
    entries[position].swithDirection = true;
}

//--------------------------------------------------------------
string Timeline::getLine(string file, int optionLine){
    ofBuffer buffer = ofBufferFromFile("text/"+file);
    
    int indx = 0;
    
    vector<string>place;
    for (auto line : buffer.getLines()) {
        string str = line;
        
#ifdef TARGET_WIN32
        str = encode(str, "UTF-8", "Windows-1252");
#endif
        place.push_back(str);
        //cout << str <<endl;
        indx++;
    }
    
    int intLine =int(floor(ofRandom( indx )));
    if(optionLine!=-1)intLine = optionLine;
    
    string result = place[intLine];
    //cout << result << endl;
    if(file !=  "JOKES.txt" && file != "OPINIONS.txt"){
        result = ReplaceString(result, "JOKE", getLine("JOKES.txt"));
        result = ReplaceString(result, "OPINION", getLine("OPINIONS.txt"));
    }
    
   
    return result;
    
}

//--------------------------------------------------------------
void Timeline::say(string line){
    //cout << line << endl;
    
#ifdef TARGET_OSX
    string cmd = "say "+line+" &";   // create the command
    system(cmd.c_str());
#endif
#ifdef TARGET_WIN32
    string cmd = "data\\SayStatic.exe "+line;   // create the command
    system(cmd.c_str());
#endif
    
}

//--------------------------------------------------------------
bool Timeline::checkEntries(int find){
    if(entries.find(find)!=entries.end()){
        //messages.push_back(string("the map entry already exists and is overwritten %d", find));
    }
    return entries.find(find)!=entries.end();
}


//#endif /* Header_h */
