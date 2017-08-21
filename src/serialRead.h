//
//  serialRead.h
//
//  Created by Karina Jensen on 06/06/17.
//
//

#ifndef serialRead_h
#define serialRead_h


class SerialRead{
public:
    void setup(){

        //set up arduino
        serial.listDevices();
        vector <ofSerialDeviceInfo> deviceList = serial.getDeviceList();

        int baud = 9600;
        serial.setup(setupVal, baud); //open the first device
        
        setupVal++;
        setupVal=setupVal%deviceList.size();

    }
    
    void update(){
        // read from arduino if it is setup
        // store value
        if(serial.isInitialized()){
        
        int nRead  = 0;
            while( (nRead = serial.readByte()) > 0){
                value = nRead;
            }
        }else if(ofGetFrameNum()%500==0){
            setup();
        }
    }
    
    
    bool start(){
        //retrun true if value is set to 2
        bool res = value == 2 ? true : false;
        return res;
    }
    
    bool stop(){
        bool res = value == 3 ? true : false;
        return res;
    }
    
    bool interrupt(){
        bool res = value == 4 ? true : false;
        return res;
    }

    
    void writeByte(unsigned char w){
        // send bytes to the arduino to light up the lights
        if(serial.isInitialized())
            serial.writeByte(w);
    }
    
    
private:
    ofSerial	serial;
    int         value;
    int setupVal = 0;
};

#endif /* serialRead_h */
