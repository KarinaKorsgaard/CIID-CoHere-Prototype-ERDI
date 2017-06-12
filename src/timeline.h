//
//  Header.h
//  Erdi_V1
//
//  Created by Karina Jensen on 02/06/17.
//
//

#pragma once

#include "ofMain.h"

#include "Poco/TextConverter.h"
#include "Poco/TextEncoding.h"
#include "Poco/Unicode.h"
#include "Poco/UTF8String.h"
#include "Poco/String.h"

//using namespace std;

struct entry{
    vector<string> file;
    vector<double> duration;
    int indx;
    
    bool isPlayed;
   
    bool isSound = false;
    bool isString = false;
    bool isSilence = false;
    
    bool swithDirection;
    
    int position;
    int next;
    int optionNext;
    
    string name;
   
};

//--------------------------------------------------------------
class Timeline{
public:

    void draw(int x, int y);
    void setup(bool loop);
    void update(float vol, float sampleDetectionLength);
    
    void defineEndPos(int p);
    
    void start();
    void stop();
    
    void addSound(string _dir, int position, int next, int optionNext = -1 , string name = "");
    void addString(string file, int position, int next, int optionNext = -1, string name = "");
    void addSilence(float duration, int position, int next, int optionNext = -1 , string name = "");
    void swithDirection();
    void loadNewEntry();
    void jumpToNext(int p = -1);
    void setNextPosition(int pos, int next);
    
    string getName();

    bool isPlaying;
    int position;
    double time;
    double interruptionTime;
    
    bool isSilent(){
        if(isValid && isPlaying)return entries[position].isSilence;
        else return false;
    }
    int interruptionPos;
    
private:
    int curIndx = 0;
    float p_vol;
    int endPos;
    string path;
    vector<string>messages;
    map<int, entry>entries;
    ofSoundPlayer sound;
    ofSoundPlayer secondSound;
    bool looping;
    bool isValid;
    
    string getLine(string file, int optionLine = -1);
    void say(string line);
    bool checkEntries(int find);
    
    string encode(string input, const string& inputEncoding, const string& outputEncoding) {
        
        Poco::TextEncoding::Ptr ie = Poco::TextEncoding::find(inputEncoding);
        Poco::TextEncoding::Ptr oe = Poco::TextEncoding::find(outputEncoding);
        Poco::TextConverter converter(*ie, *oe);
        string output;
        converter.convert(input, output);
        return output;
    }
    
    string ReplaceString(std::string subject, const std::string& search,
                              const std::string& replace) {
        size_t pos = 0;
        while ((pos = subject.find(search, pos)) != std::string::npos) {
            subject.replace(pos, search.length(), replace);
            pos += replace.length();
        }
        return subject;
    }
    
    string st(float s){
        return " "+ ofToString(s, 1) + " ";
    }
};

//#endif /* Header_h */
