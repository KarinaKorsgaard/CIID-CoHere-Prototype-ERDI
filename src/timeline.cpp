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
void Timeline::update(float vol){
    isValid = entries.find(position)!=entries.end();
    
    if(p_vol != vol){
        sound.setVolume(vol);
        p_vol = vol;
    }
    
    if(isValid && isPlaying){
        time+=ofGetLastFrameTime();
        
        if(time > entries[position].duration){
            loadNewEntry();
        }
    }
}


void Timeline::loadNewEntry(){
    //ofLogNotice("new entry!");

    if(entries[position].swithDirection)position = entries[position].optionNext;
    else position = entries[position].next;
    time = 0.;
    sound.stop();
    
    if(entries[position].name == "stream"){
        addSound("09_stream", 10 , 40 , -1 , "stream");
    }
    if(entries[position].name == "opinion"){
        if(ofRandom(1)>0.2)
            addSound("opinions", 40 , 10, -1 , "opinion"); // stream
        else
            addSound("quotes", 40 , 10, -1 , "opinion"); // stream
    }
    
    if(entries.find(position)!=entries.end()){
        
        if(entries[position].isSound){
            sound.load(entries[position].file);
            sound.play();
        }
        else{
            say(entries[position].file);
        }
    }else{
        messages.push_back("timeline ended");
        isPlaying = false;
        if(looping)start();
    }
}



void Timeline::draw(int x, int y){
    ofPushMatrix();
    ofTranslate(x, y);
    ofFill();
        
    if(isValid){
        
        ofDrawBitmapString( entries[position].name , 10, 10);
        ofDrawBitmapString( st(position) + entries[position].file , 10, 20);
        ofDrawBitmapString( st(entries[position].duration) + "current duration" , 10, 30);
        ofDrawBitmapString(st(time) + "current time", 10, 40);
        int next =entries[position].swithDirection ? entries[position].next : entries[position].optionNext;
        ofDrawBitmapString(st(next)+ "next position", 10, 50);
        
        for(int i = 0; i<messages.size();i++){
            ofDrawBitmapString(messages[i], 10, 60 + 10*i);
        }
       
    }
    ofPopMatrix();
}


//--------------------------------------------------------------
void Timeline::start(){
    if(!isPlaying){
    	isPlaying = true;
    	time = 0.f;
    	position = 0;
    
    	cout<<"start"<<endl;
    
    	isValid = entries.find(0)!=entries.end();
    
    	if(!isValid)messages.push_back("no start entry found!, abort");

    	map<int, entry>::iterator it;
    	for (it = entries.begin(); it != entries.end(); it++)
    	{
        	it->second.isPlayed = false;
        	it->second.swithDirection = false;
    	}
    
    	if(entries.find(position)!=entries.end()){
        
        	if(entries[position].isSound){
            		sound.load(entries[position].file);
            		sound.play();
        	}
        	else{
         	   	say(entries[position].file);
       	 	}
    	}else{
        	messages.push_back("timeline ended");
        	isPlaying = false;
        
    	}
	}
}
void Timeline::stop(){
    if(isPlaying){
	cout << "stop" << endl;
    	isPlaying = false;
    	position = endPos;
    	time = 0.;
    	sound.stop();
    
    	if(entries.find(position)!=entries.end()){
        
       	 	if(entries[position].isSound){
          	  	sound.load(entries[position].file, true);
           	 	sound.play();
        	}
        	else{
        	    say(entries[position].file);
        	}
    	}
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
    if(entries.find(position)!=entries.end()){
        
        if(entries[position].isSound){
            sound.load(entries[position].file);
            sound.play();
        }
        else{
            say(entries[position].file);
        }
    }else{
        messages.push_back("timeline ended");
        isPlaying = false;
        if(looping)start();
    }
    
}


void Timeline::addSilence(float duration, int position, int next, int optionNext, string name){
    
    checkEntries(position);
    
    entry e = *new entry;
    e.name = name;
    e.next = next;
    e.optionNext = optionNext == -1 ? next : optionNext;
    e.duration = duration;
    e.isSilence = true;
    e.isPlayed = false;
    
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
    
    if(dir.size()>0){
        int indx = floor(ofRandom(dir.size()));
        string filpath = dir.getPath(indx);
        
        //if(ofFile::doesFileExist(filePath)){
        
        
        entry e = *new entry;
        e.file = filpath;
        
        ofSoundPlayer p;
        
        if(p.load(filpath)){
            
            
            p.play();
            p.setPosition(0.9999999f);
            int ms = p.getPositionMS();
            p.setPosition(0);
            p.stop();
            p.unload();
            //printf("SOUND LENGTH: %i\n\n", ms);
            
            e.isSound = true;
            e.duration = float(ms)/1000;
            e.isPlayed = false;
            
            e.name = name;
            e.next = next;
            e.optionNext = optionNext == -1 ? next : optionNext;
            
            
            entries[position] = e;
        }
        else{
            cout<<("ERROR, it is a file, but not a soundfile")<<endl;
        }
    }
   // }
    else{
        cout<<("ERROR, could not find file")<<endl;
    }
}

//--------------------------------------------------------------
void Timeline::addString(string file, int position, int next, int optionNext, string name){
    
    string filePath = file;
    checkEntries(position);
    
    if(ofFile::doesFileExist("text/"+filePath)){
        
        entry e = *new entry;
        e.name = name;
        e.file = getLine(filePath);
        e.isString = true;
        e.duration = float(e.file.length()) * 0.07;
        if(e.duration < 0.1)messages.push_back(string("short file added %d", e.duration));
        e.isPlayed = false;
        e.optionNext = optionNext == -1 ? next : optionNext;
        e.next = next;
        e.position = position;
        
        entries[position] = e;
        
    }
    else{
        cout<<("ERROR, could not find file")<<endl;
    }
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
