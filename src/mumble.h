
//  Erdi_V1
//
//  Created by Karina Jensen on 04/06/17.
//
//

#ifndef twitter_h
#define twitter_h

struct Player{
    // tiny struct to hold the player information
    int soundIndx;
    float age;
    float volume;
    ofSoundPlayer soundPlayer;
};

class Mumbler{
public:
    string path;
    vector<string>files;
    vector<Player>soundPlayers;
    vector<float>volumes;
    
    // set up the background noice with an amount of voices and a directory folder name
    void setup(string _dir, int numMumblers){
        path = _dir;
        ofDirectory dir;
        dir.listDir(_dir);

        for(int i = 0; i<dir.size();i++){
            ofSoundPlayer p;
            if(p.load(dir.getPath(i)))
                files.push_back( dir.getPath(i) );
        }
        
        soundPlayers.resize(numMumblers);
        
    }
    
    void update(float vol, bool stop){
        // check for new files
        if(ofGetFrameNum()%200==0)loadStrings();

        // turn the voulme up or down on the samples and load a new one if one finished
        for(int i = 0; i<soundPlayers.size();i++){
            Player * p = &soundPlayers[i];
            p->age++;
            
            if(!stop){
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
            }
            p->volume=CLAMP(p->volume,0,1);
            p->soundPlayer.setVolume(p->volume * MAX(vol,0) );
        }
    }

    
    void loadStrings(){
        files.clear();
        ofDirectory dir(path);
        
        dir.listDir();
        dir.sort();
        dir.allowExt("wav");

        for(int i = 0 ; i<dir.size();i++){
            files.push_back(dir.getPath(i));
        }
    }
};

#endif /* twitter_h */

