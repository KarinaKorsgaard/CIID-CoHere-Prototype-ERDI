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
void Timeline::update(){
    isValid = entries.find(position)!=entries.end();
    
    if(isValid){
        time+=ofGetLastFrameTime();
        
        if(time > entries[position].duration){
            if(entries[position].swithDirection)position = entries[position].optionNext;
            else position = entries[position].next;
            time = 0.;
            
            if(entries[position].name == "steam"){
                int i = ofRandom(2)>0.5;
                //if(i)addSound("03_STEAM", position, entries[position].next,entries[position].optionNext,entries[position].name);
                addString("03_STREAM.txt", 11 , 11 , 12 , -1 , "steam");
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
    }
}

void Timeline::draw(){
    ofFill();
    ofSetColor(255);
    ofDrawRectangle(0,0,300,ofGetHeight());
    
    ofSetColor(0);
    
    if(isValid){
        ofPushMatrix();
        ofDrawBitmapString( entries[position].name , 10, 20);
        ofTranslate(20, 10);
        ofDrawBitmapString( st(position) + entries[position].file , 10, 20);
        ofDrawBitmapString( st(entries[position].duration) + "current duration" , 10, 30);
        ofDrawBitmapString(st(time) + "current time", 10, 40);
        int next =entries[position].swithDirection ? entries[position].next : entries[position].optionNext;
        ofDrawBitmapString(st(next)+ "next position", 10, 50);
        
        for(int i = 0; i<messages.size();i++){
            ofDrawBitmapString(messages[i], 10, 30 + 60 + 20*i);
        }
        ofPopMatrix();
    }
    else
        ofDrawBitmapString("you suck!", 10, 20);
}


//--------------------------------------------------------------
void Timeline::start(){
    
    isPlaying = true;
    time = 0.f;
    position = 0;
    
    isValid = entries.find(position)!=entries.end();
    
    if(!isValid)messages.push_back("no start entry found!, abort");

    map<int, entry>::iterator it;
    for (it = entries.begin(); it != entries.end(); it++)
    {
        it->second.isPlayed = false;
        it->second.swithDirection = false;
    }
    
    if(entries[position].isSound){
        sound.load(entries[position].file);
        sound.play();
    }
    else if(entries[position].isString){
        say(entries[position].file);
    }
    else{
        // shut up!
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

//--------------------------------------------------------------
void Timeline::addSound(string file, int position, int next, int optionNext, string name){
    string filePath = path + file;
    checkEntries(position);
    
    
    if(ofFile::doesFileExist(filePath)){
        
        
        entry e = *new entry;
        e.file = filePath;
        
        ofSoundPlayer p;
        if(p.load(filePath)){
            
            p.setPosition(0.5f);
            double sampleLength = 2.f * p.getPositionMS(); // wink
            
            e.isSound = true;
            e.duration = sampleLength;
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
    else{
        cout<<("ERROR, could not find file")<<endl;
    }
}

//--------------------------------------------------------------
void Timeline::addString(string file, int position, int next, int optionNext, int line, string name){
    
    string filePath = file;
    checkEntries(position);
    
    if(ofFile::doesFileExist("text/"+filePath)){
        
        entry e = *new entry;
        e.name = name;
        e.file = getLine(filePath, line);
        e.isString = true;
        e.duration = float(e.file.length()) * 0.1;
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
        messages.push_back(string("the map entry already exists and is overwritten %d", find));
    }
    return entries.find(find)!=entries.end();
}


//#endif /* Header_h */
