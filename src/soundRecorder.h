//
//  soundRecorder.h
//  soundRecorder
//
//  Created by Karina Jensen on 05/06/17.
//
//

#ifndef soundRecorder_h
#define soundRecorder_h

#include "ofMain.h"
#include "ofxLibsndFileRecorder.h"
#include "ofxGui.h"
#include "ofxXmlSettings.h"

#define NUM_CHANNELS 1
#define SAMPLE_RATE 44100
#define BUFFER_SIZE 1024


class SoundRecorder : public ofBaseApp{

public:
    double vol;
    double sampleLength;
    bool doneRecording;
    bool recording;
    
    ofxPanel gui;
    ofParameterGroup recGroup;
    
    string pt;
    
    ofParameter<float>threshold;
    ofParameter<float>minSampleLength;
    ofParameter<float>wait;
    
    void setup(){
        
        //setup gui for the recorder
        recGroup.setName("recorder gui");
        recGroup.add(threshold.set("volume threshold",0.26,6,60.));
        recGroup.add(wait.set("allowed pauses",0.80,0,10));
        recGroup.add(minSampleLength.set("Min Sample Length",3.,0,20));
        
        // load values from xml file
        int soundDevice;
        
        ofxXmlSettings xml;
        if(xml.load("config.xml")){
            filePath = xml.getValue("config:path", "");
            soundDevice = xml.getValue("config:soundDevice", 0);
        }
        else filePath = "Users/annelieberner/Dropbox/sounds/outro/";
        
        
        // sound setup:
        recording=false;
        soundStream.printDeviceList();
//set the device id to 0 if on mac and 1 if on raspberry
#ifdef TARGET_OSX
        soundStream.setDeviceID(0);
#else
        soundStream.setDeviceID(1);
#endif
        
        vol     = 0.0;
        soundStream.setup(this, 0, NUM_CHANNELS, SAMPLE_RATE, BUFFER_SIZE, 4);
        
        
        //seach in directory to find the name of the of the last recorded sample so we start saving files from there
        ofDirectory dir;
        string path =filePath;
        dir.allowExt("wav");
        dir.listDir(path);
        dir.sort();
        
        if(dir.size()>0){
            cout << dir.getName(dir.size()-1)<< endl;
            vector<string> name = ofSplitString(dir.getName(dir.size()-1), ".");
            audioCount = ofToInt(name.front())+1;
        }else{
            audioCount=0;
        }
        cout << audioCount << endl;
        doneRecording = false;
    }
    
    void update(bool rec){
        
        double dt = ofGetLastFrameTime();
        
        if(recording)sampleLength+=dt;
        if(getVolume()) silentSec = 0.f;
        
        //if the volume is higher than threshold start recording
        if(getVolume() && rec && !recording){
            pt = filePath+ofToString(audioCount, 5 ,'0')+".wav";
            silentSec = 0.f;
            startSpeak +=ofGetLastFrameTime();
            
            if(startSpeak>0.15){
                sampleLength=0;
                cout<<"Start recording\n";
                cout << pt<<"----\n";
                
                audioRecorder.setup(pt);
                audioRecorder.setFormat(SF_FORMAT_WAV | SF_FORMAT_PCM_16);
                recording=true;
                
            }
        }
        else if(recording){
            silentSec+=dt;
            // if it is silent for too long, stop recording
            if(silentSec>wait){

                cout<<"Stop recording\n";
                startSpeak = 0;
                
                // check if file is longer than min sample length
                save = sampleLength > minSampleLength;
                
                // cut of the last silent part of the sample
                if(save)
                    audioRecorder.recordingSize -= (silentSampleSize);
                
                // finalize file and remove the file if it is too short
                audioRecorder.finalize();
                if(!save){
                    ofFile::removeFile(pt, true);
                    cout <<"removed file "+pt<< endl;
                }else{
                    audioCount ++;
                    cout <<"kept file "+pt<< endl;
                }
                recording=false;
            }
        }
    }
    
    // debugging curve
    void drawCurve(){
        ofPolyline p;
        for(int i = 0; i<hist.size();i++)
            p.addVertex(i,hist[i],1);
        
        p.draw();
    }
    
    bool getVolume(){
        return vol>threshold;
    }

    //--------------------------------------------------------------
    void audioReceived(float * input, int bufferSize, int nChannels){
        
        
        if (recording){
            audioRecorder.addSamples(input, bufferSize * nChannels);
            if(vol<=threshold)silentSampleSize+=bufferSize * nChannels;
            else silentSampleSize = 0;
        }
        
        float curVol = accumulate(input, input + bufferSize * nChannels, 0., []( float a, float b) {
            return abs(a)+abs(b);
        });
        
        vol = curVol*0.8 + vol * 0.2;
        hist.push_back(300-vol);
        
        if  (hist.size()>ofGetWidth())
            hist.erase(hist.begin());
    }
    
    
    bool save;
    

    ofxLibsndFileRecorder audioRecorder;
    
    double silentSec, startSpeak;
    int audioCount;
    int silentSampleSize = 0;
    string filePath;
    
    private:
    ofSoundStream soundStream;
    
    vector<float>hist;
};

#endif /* soundRecorder_h */
