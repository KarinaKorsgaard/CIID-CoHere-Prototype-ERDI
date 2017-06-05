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

#define NUM_CHANNELS 2
#define SAMPLE_RATE 44100
#define BUFFER_SIZE 1024


class SoundRecorder : public ofBaseApp{

public:
    
    bool doneRecording;
    ofxPanel gui;
    double sampleLength;
    bool recording;
    
    
    void setup(){
        
        // gui setup
        ofParameterGroup g;
        
        g.add(threshold.set("volume threshold",0.26,0,0.5));
        g.add(wait.set("allowed pauses",0.80,0,10));
        g.add(minSampleLength.set("Min Sample Length",3.,0,20));
        
        gui.setup(g, "recorder_settings.xml",600,10);
        gui.loadFromFile("recorder_settings.xml");
        
        
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
        
        left.assign(BUFFER_SIZE, 0.0);
        right.assign(BUFFER_SIZE, 0.0);

        bufferCounter	= 0;
        smoothedVol     = 0.0;
        scaledVol		= 0.0;
        soundStream.setup(this, 0, NUM_CHANNELS, SAMPLE_RATE, BUFFER_SIZE, 4);
        
        
        // find audio indx, where to start recording..
        ofDirectory dir;
        string path =filePath;
        dir.allowExt("wav");
       
        dir.listDir(path);
        dir.sort();
        
        message = dir.isDirectory()? "directory found, looking in: "+filePath:"error: directory not found. Change path in config.xml "+filePath;
        
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
        
        // scale volume to something that makes sense.
        scaledVol = ofMap(smoothedVol, 0.0, 0.17, 0.0, 1.0, true);
        
        double dt = ofGetLastFrameTime();
        
        if(recording)sampleLength+=dt;

        if(rec){
            if(scaledVol >= threshold){
                silentSec = 0.f;
                
                if(!recording){
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
                
                if(silentSec>wait){
                    cout<<"Stop recording\n";
                    recording=false;
                    doneRecording = true;
                    bool save = sampleLength > minSampleLength;
                    if(save){
                        audioRecorder.finalize();
                    }
                    else{
                        audioRecorder.initialized=false;
                        audioRecorder.recordingSize=0;
                        delete audioRecorder.outFile;
                        ofFile::removeFile(filePath+ofToString(audioCount,0)+".wav", false);
                        cout << filePath+ofToString(audioCount,0)+".wav" << endl;
                        //ofFile(filePath+ofToString(audioCount,0)+".wav", false).remove();
                        //audioCount--;
                    }
                    sampleLength=0;
                }
            }
        }
    }
    
    
    float getVolume(){
        return scaledVol;
    }
    
    //--------------------------------------------------------------
    void audioReceived(float * input, int bufferSize, int nChannels){
        
        if (recording)
            audioRecorder.addSamples(input, bufferSize * nChannels);
        
//        inVol = accumulate(input, input + bufferSize * nChannels, 0., []( float a, float b ) {
//            return std::max(a, b);
//        });
        
        float curVol = 0.0;
        
        // samples are "interleaved"
        int numCounted = 0;
        
        //lets go through each sample and calculate the root mean square which is a rough way to calculate volume
        for (int i = 0; i < bufferSize; i++){
            left[i]		= input[i*2]*0.5;
            right[i]	= input[i*2+1]*0.5;
            
            curVol += left[i] * left[i];
            curVol += right[i] * right[i];
            numCounted+=2;
        }
        
        //this is how we get the mean of rms :)
        curVol /= (float)numCounted;
        
        // this is how we get the root of rms :)
        curVol = sqrt( curVol );
        
        smoothedVol *= 0.93;
        smoothedVol += 0.07 * curVol;
        
        bufferCounter++;
    }
    
    
    
private:
    ofxLibsndFileRecorder audioRecorder;
    
    
    ofParameter<float>threshold;
    ofParameter<float>minSampleLength;
    ofParameter<float>wait;
    
    double silentSec;
    int audioCount;
    
    string filePath;
    string message;
    ofSoundStream soundStream;
    
    vector <float> left;
    vector <float> right;
    vector <float> volHistory;
    
    int 	bufferCounter;
    
    float smoothedVol;
    float scaledVol;
};

#endif /* soundRecorder_h */
