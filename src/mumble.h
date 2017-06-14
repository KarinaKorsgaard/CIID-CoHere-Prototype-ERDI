//
//  twitter.h
//  Erdi_V1
//
//  Created by Karina Jensen on 04/06/17.
//
//

#ifndef twitter_h
#define twitter_h

struct Player{
    int soundIndx;
    float age;
    ofSoundPlayer soundPlayer;
    float volume;
};

class Mumbler{
public:
    string path;
    vector<string>files;
    vector<Player>sp;
    vector<float>volumes;
    
    void setup(string _dir, int numMumblers){
        path = _dir;
        ofDirectory dir;
        dir.listDir(_dir);
        //dir.allowExt("mp3");
        
        for(int i = 0; i<dir.size();i++){
            ofSoundPlayer p;
            if(p.load(dir.getPath(i)))
                files.push_back( dir.getPath(i) );
        }
        
        sp.resize(numMumblers);
        
    }
    
    void update(float vol){
        
        if(ofGetFrameNum()%200==0)loadStrings();
        
        //
        for(int i = 0; i<sp.size();i++){
            Player * p = &sp[i];
            p->age++;
            
            //if(vol > 0){
            if(p->age>60*60)p->volume-=0.1;
            else if (p->age<30)p->volume+=0.01;
            else if (ofRandom(1)>.5)p->volume+=0.01;
            else p->volume-=0.01;
            
            if(p->volume<0. || (!p->soundPlayer.isPlaying() && p->age>10)){
                if(files.size()>0){
                    int sIndx = int(ofRandom(files.size()-1));
                    p->soundPlayer.unload();
                    p->soundPlayer.load(files[sIndx]);
                    p->age = 0;
                    p->volume = 0;
                    p->soundIndx = sIndx;
                    p->soundPlayer.play();
                }
            }

            p->volume=CLAMP(p->volume,0,1);
            p->soundPlayer.setVolume(p->volume * MAX(vol,0) );
        }
    }
    //  }
    
    //}
    
    void loadStrings(){
        
        files.clear();
        ofDirectory dir(path);
        
        dir.listDir();
        dir.sort();
        // dir.allowExt("mp3");
        dir.allowExt("wav");
        // dir.allowExt("ogg");
        
        for(int i = 0 ; i<dir.size();i++){
            files.push_back(dir.getPath(i));
        }
    }
};

#endif /* twitter_h */

