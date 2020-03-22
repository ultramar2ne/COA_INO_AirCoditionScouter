#include <SoftwareSerial.h>

SoftwareSerial mySerial(7,6); // Arudino Uno port RX, TX

void setup() {
  // for debuging 
  Serial.begin(9600);  
  
  // Use software serial
  mySerial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  static int CheckFirst=0;
  static int pm_add[3][5]={0,};
  static int pm_old[3]={0,};
  int chksum=0,res=0;;
  unsigned char pms[32]={0,};
  
  
  if(mySerial.available()>=32){

    for(int j=0; j<32 ; j++){
      pms[j]=mySerial.read();
      if(j<30)
        chksum+=pms[j];
    }

    if(pms[30] != (unsigned char)(chksum>>8) 
        || pms[31]!= (unsigned char)(chksum) ){

      return res;
    }
    if(pms[0]!=0x42 || pms[1]!=0x4d )
      return res;

  Serial.print("Dust raw data debugging :  ");
  Serial.print("1.0ug/m3:");
  Serial.print(pms[10]);
  Serial.print(pms[11]);
  Serial.print("  ");
  Serial.print("2.5ug/m3:");
  Serial.print(pms[12]);
  Serial.print(pms[13]);
  Serial.print("  ");
  Serial.print("2.5ug/m3:");
  Serial.print(pms[14]);
  Serial.println(pms[15]);
  } 
}
