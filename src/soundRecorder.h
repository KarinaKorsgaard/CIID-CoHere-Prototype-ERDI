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
    bool doneRecording;
    ofxPanel gui;
    double sampleLength;
    bool recording;
    ofParameterGroup recGroup;
    
    ofParameter<float>threshold;
    ofParameter<float>minSampleLength;
    ofParameter<float>wait;
    
    void setup(){
        
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
        //if you want to set a different device id
        soundStream.setDeviceID(soundDevice); //bear in mind the device id corresponds to all audio devices, including  input-only and output-only devices.

        vol     = 0.0;
        soundStream.setup(this, 0, NUM_CHANNELS, SAMPLE_RATE, BUFFER_SIZE, 4);
        
        
        // find audio indx, where to start recording..
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

        
            if(vol >= threshold && rec){
                silentSec = 0.f;
                
                if(!recording && rec){
                    cout<<"Start recording\n";
                    audioCount++;
                    string pt=filePath+ofToString(audioCount,0)+".wav";
                    
                    cout << pt<<"----\n";
                    audioRecorder.setup(pt);
                    audioRecorder.setFormat(SF_FORMAT_WAV | SF_FORMAT_PCM_16);
                    recording=true;
                    
                }
            }
            else if(recording){
                silentSec+=dt;
                
                if(silentSec>wait || !rec){
                    cout<<"Stop recording\n";
                    recording=false;
                    bool save = sampleLength > minSampleLength;
    
                    audioRecorder.finalize();
                    
                    if(!save){
                        ofFile::removeFile(filePath+ofToString(audioCount,0)+".wav", true);
                    }else{
                        audioCount ++;
                    }
                    sampleLength=0;
                }
            
        }
    }
    
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
        
        if (recording)
            audioRecorder.addSamples(input, bufferSize * nChannels);
        
        
        float curVol = accumulate(input, input + bufferSize * nChannels, 0., []( float a, float b) {
            return abs(a)+abs(b);
        });
        
        vol = curVol*0.8 + vol * 0.2;
        hist.push_back(300-vol);
        
        if  (hist.size()>ofGetWidth())
            hist.erase(hist.begin());
    }
    
    

    
private:
    ofxLibsndFileRecorder audioRecorder;
    
    double silentSec;
    int audioCount;
    
    string filePath;
    ofSoundStream soundStream;
    
    vector<float>hist;
};

#endif /* soundRecorder_h */
