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

        serial.listDevices();
        vector <ofSerialDeviceInfo> deviceList = serial.getDeviceList();

        int baud = 9600;
        serial.setup(setupVal, baud); //open the first device
        
        setupVal++;
        setupVal=setupVal%deviceList.size();
        
        //serial.setup("COM4", baud); // windows example
        //serial.setup("/dev/tty.usbserial-A4001JEC", baud); // mac osx example
        //    serial.setup("/dev/ttyUSB0", baud); //linux example
        //if(serial.isInitialized())
            //serial.flush();
    }
    
    void update(){
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
        if(serial.isInitialized())
            serial.writeByte(w);
    }
    
    
private:
    ofSerial	serial;
    int         value;
    int setupVal = 0;
};

#endif /* serialRead_h */
