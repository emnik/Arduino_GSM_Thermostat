#include "SIM900.h"
#include <SoftwareSerial.h>
#include "sms.h"
// include the library code for the LCD screen:
#include <Wire.h>
#include <Adafruit_RGBLCDShield.h>
#include <utility/Adafruit_MCP23017.h>


#define RELAY1 51

Adafruit_RGBLCDShield lcd = Adafruit_RGBLCDShield();

SMSGSM sms;
char number[]="+306985703883";
char message[180];
char pos;
char del;
char *p;
char *s;
char *e;
char *i;
int state;


void setup()
{
  pinMode(RELAY1, OUTPUT); 
  digitalWrite(RELAY1,LOW);
  
  //lcd.setBacklight(WHITE);
  
  Serial.begin(9600);
  lcd.begin(16, 2);
  lcd.print("GSM Connecting");
  lcd.setCursor(0, 1);
  lcd.print("Please wait...");
  if (gsm.begin(2400)){
    Serial.println("\nstatus=READY");
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("GSM Ready!");
  }
  else
  { 
    Serial.println("\nstatus=IDLE");
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("GSM IDLE!");
  }
};
 
void loop()
{
 pos=sms.IsSMSPresent(SMS_ALL);
// Serial.println((int)pos);
 if((int)pos>0){
   Serial.print("NEW MESSAGE, POS=");
   Serial.println((int)pos);
   message[0]='\0';
   sms.GetSMS(pos,number,message,180);
   p=strstr(message,"testpwd");
   s=strstr(message,"START");
   e=strstr(message,"STOP");
   i=strstr(message,"STATUS");
//   Serial.println("-------------------------DEBUG------------------------");
//   Serial.println(p);
//   Serial.println(s);
//   Serial.println(e);
//   Serial.println(i);
//   Serial.println("-------------------------DEBUG------------------------");
   if(p!=NULL){
     Serial.println("PSWD OK");
     if(s!=NULL && e==NULL && i==NULL){
        Serial.println("THERMOSTAT STARTED");
        digitalWrite(RELAY1,HIGH);
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("THERMOSTAT STARTED!");
     }
     else if(s==NULL && e!=NULL && i==NULL){
        Serial.println("THERMOSTAT STOPPED");
        digitalWrite(RELAY1,LOW);
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("THERMOSTAT STOPPED!");        
     }
     else if(s==NULL && e==NULL && i!=NULL){
        Serial.print("THERMOSTAT's STATUS is: ");
        state=digitalRead(RELAY1);
        if(state==1){
          Serial.println("ON");
          lcd.clear();
          lcd.setCursor(0,0);
          lcd.print("THERMOSTAT STATUS: ON");              
        }
        else{
          Serial.println("OFF");
          lcd.clear();
          lcd.setCursor(0,0);
          lcd.print("THERMOSTAT STATUS: OFF");          
        }
     }
   }
   else
   {
    Serial.println("PSWD WRONG!");
   }
   //resetting pointers to NULL values
   *p=NULL;
   *s=NULL;
   *e=NULL;
   *i=NULL;
   
   //Delete the message in (int)pos
   //sms.DeleteSMS((int)pos);

   //Delete ALL the messages
   for(int i=1;i<=(int)pos;i++){
      del = sms.DeleteSMS(i);
      Serial.print("msg in pos: ");
      Serial.print(i);
      if(del==1) {
          Serial.println(" deleted successfuly!");
        }
        else
        {
          Serial.println(" was not deleted - ERROR!");
        }  
    }
  }
 delay(5000);
};

