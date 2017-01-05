/*   ARDUINO GSM LCD THERMOSTAT - A simple home thermostat with SMS control capabilities!
*    Copyright (C) 2016  Nikiforakis Manos
*
*    This program is free software: you can redistribute it and/or modify
*    it under the terms of the GNU General Public License as published by
*    the Free Software Foundation, either version 3 of the License, or
*    (at your option) any later version.
*
*    This program is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*    GNU General Public License for more details.
*
*    You should have received a copy of the GNU General Public License
*    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

//Using the TinySine GSM Shield (http://www.tinyosshop.com/index.php?route=product/product&product_id=464)
#include "SIM900.h"
#include <SoftwareSerial.h>
#include "sms.h"

//Using an ADAFRUIT I2C LCD SCREEN (https://www.adafruit.com/products/715)
// include the library code for the LCD screen:
#include <Wire.h>
#include <Adafruit_RGBLCDShield.h>
#include <utility/Adafruit_MCP23017.h>

// include the library code for the DS18b20 temprerature sensor:
#include <OneWire.h>
#include <DallasTemperature.h>

//Using an ARDUINO MEGA or compatible board
#define RELAY1 51 // Relay IN wire is plugged into pin 51 on the Arduino
#define ONE_WIRE_BUS 40 // Data wire is plugged into pin 40 on the Arduino
#define KNOB A15 // Knob for manual setting the target temp is plugged into pin 31 on the Arduino
#define BLUE_RGB 32 //common anode -> write LOW to light!
#define GREEN_RGB 30 //common anode -> write LOW to light!
#define RED_RGB 34 //common anode -> write LOW to light!

const float hysteresis = 0.5;
const float manual_control_threshold = 0.4; //threshold for manual override to avoid false overrides due to bouncing effects...
const int new_target_time_threshold = 5000; //secs to wait after setting to a target temp value
const int manual_min_temp = 10;
const int manual_max_temp = 30;
boolean GSM_Status;
boolean force_STOP = true; // used to START/STOP the thermostat via SMS
float target, cur_temp, last_temp;
unsigned long target_timer;
boolean set_new_target = true;
boolean manual_control = false; //true when the temp is manually set - false when is set via SMS!
char NumberToSendSMS[] = "69xxxxxxxx";
float celsius; // keep the temperature in this valiable
long checkSMSTimer = 0; //used to check the SMS every 10secs
boolean prevstate = 1; //at initialization, the thermostat if set at OFF status. When for the first time one sets a target between the setpoint-hysteresis and setpoint I want the thermostat to turn ON 

SMSGSM sms;
char number[] = "+3069xxxxxxxx";
char message[160];
char pos;
char del;
char *p; //password
char *s; //start
char *e; //stop
char *i; //status
char *t; //target
int status; //check the RELAY status to report by SMS

OneWire oneWire(ONE_WIRE_BUS); // Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
DallasTemperature sensors(&oneWire); // Pass our oneWire reference to Dallas Temperature.

Adafruit_RGBLCDShield lcd = Adafruit_RGBLCDShield();


void setup()
{
  pinMode(BLUE_RGB, OUTPUT);
  pinMode(GREEN_RGB, OUTPUT);
  pinMode(RED_RGB, OUTPUT);
  digitalWrite(BLUE_RGB, HIGH); //led off
  digitalWrite(GREEN_RGB, HIGH); //led off
  digitalWrite(RED_RGB, HIGH); //led off
  pinMode(RELAY1, OUTPUT);
  pinMode(KNOB, INPUT);
  digitalWrite(RELAY1, LOW);

  Serial.begin(9600);

  lcd.begin(16, 2);
  lcd.print("GSM Connecting");
  lcd.setCursor(0, 1);
  lcd.print("Please wait...");

  // Start up the dallas (ds18b20) library
  sensors.begin();

  if (gsm.begin(2400)) {
    Serial.println("status=READY");
    GSM_Status = 0;
  }
  else
  {
    Serial.println("status=IDLE");
    GSM_Status = 1;
  }

  lcd.clear();
  // Get the current temperature
  sensors.requestTemperatures(); // Send the command to get temperatures
  celsius = sensors.getTempCByIndex(0);
  // Show the temp to the screen
  lcd.setCursor(0, 0);
  lcd.print("Now:");
  lcd.print(celsius, 1); // ,1 is for showing the temperature with one decimal digit
  lcd.print("C");

  lcd.setCursor(9, 0);
  if (GSM_Status == 0) {
    lcd.print(" GSM ok");
  }
  else {
    lcd.print(" No GSM");
  }

  //Get the initial target from the KNOB
  target = last_temp = round((float)map(analogRead(KNOB), 0, 1023, manual_min_temp * 10, manual_max_temp * 10)) / 10.0;

  if (target<=manual_min_temp+manual_control_threshold){
    Serial.print("The status is initially set at: ");
    Serial.println("OFF");
    lcd.setCursor(0, 1);
    lcd.print("Status is:");
  }
  else {
    Serial.print("The target is initially set at: ");
    Serial.println(target);  
    lcd.setCursor(0, 1);
    lcd.print("Set:");
    lcd.print(target, 1);
    lcd.print("C");
  }
  
  lcd.setCursor(10, 1);
  lcd.print("   OFF");
};

void loop()
{
  //if(GSM_Status==0){
  //check for sms every 5sec but only if the temperature is remotely set or manually but not while changing!!!
  if (((millis() - checkSMSTimer) > 10000) and ((manual_control == true and set_new_target == false) or manual_control == false)) {
    pos = checkSMS();
    if ((int)pos > 0) {
      handleSMS(pos); //handle the SMS
      deleteSMS(pos); //delete SMS
    }
    checkSMSTimer = millis();
  }//end sms handling
  //}
  sensors.requestTemperatures(); // Send the command to get temperatures
  celsius = sensors.getTempCByIndex(0);
  lcd.setCursor(0, 0);
  lcd.print("Now:");
  lcd.print(celsius, 1);
  lcd.print("C");

  float val = analogRead(KNOB);
  cur_temp = round((float)map(val, 0, 1023, manual_min_temp * 10, manual_max_temp * 10)) / 10.0;
  if ((cur_temp < last_temp - manual_control_threshold or cur_temp > last_temp + manual_control_threshold) and manual_control == false) {
    manual_control = true;
    force_STOP = false;
    Serial.println("The temperature is now manually controlled!!!");
  }
  if (manual_control) {
    if (cur_temp != last_temp and cur_temp != target) {
      if (cur_temp <= manual_min_temp+manual_control_threshold){
        lcd.setCursor(0, 1);
        lcd.print("changed to : OFF");
      }
      else {
        Serial.print("changed to: ");
        Serial.println(cur_temp);
        lcd.setCursor(0, 1);
        lcd.print("changed to:");
        lcd.print(cur_temp, 1);
        lcd.print("C");        
      }

      last_temp = cur_temp;
      target_timer = millis();
      set_new_target = true;
    }
    if (millis() - target_timer > new_target_time_threshold) {
      if (set_new_target) {
        target_timer = 0;
        target = last_temp;
        if (target <= manual_min_temp+manual_control_threshold){
          force_STOP = true;
          manual_control = false; //this is needed so that when the knob gets rotated again to turn the force_STOP to false!
          ManageThermostat(0);
          Serial.println("The thermostat is manually set to OFF");
          lcd.setCursor(0, 1);
          lcd.print("Status is:");
        }
        else {
          Serial.print("\nThe target temp is now set to: ");
          Serial.println(target);
          lcd.setCursor(0, 1);
          lcd.print("Set:");
          lcd.print(target, 1);
          lcd.print("C ");   
        }

        set_new_target = false;
      }
    }
  }

  //ON - OFF Control Algorithm
  if (!force_STOP and ((manual_control == true and set_new_target == false) or manual_control == false)) {
    if (celsius > target) {
      prevstate = 0;
      ManageThermostat(0); // Pause
    }
    else if (celsius < target - hysteresis) {
      prevstate = 1;
      ManageThermostat(1); // Work
    }
    else {
      if (prevstate == 1){
        ManageThermostat(1);
      }
      else {
        ManageThermostat(0);
      }
    }
  }


  //delay(200);
};


int checkSMS() {
  pos = sms.IsSMSPresent(SMS_ALL);
  // Serial.println((int)pos);
  return (int)pos;
}


void handleSMS(int pos) {
  lcd.setCursor(9, 0);
  lcd.print("   *SMS");
  Serial.print("NEW MESSAGE, POS=");
  Serial.println((int)pos);
  message[0] = '\0';
  sms.GetSMS(pos, number, message, 180);
  p = strstr(message, "passwrd"); //<<<<<<<<<<<<<<<<<<<<<<HERE YOU CAN SET YOUR OWN PASSWORD<<<<<<<<<<<<<<<<<<<<<<< 
  s = strstr(message, "START");
  e = strstr(message, "STOP");
  i = strstr(message, "STATUS");
  t = strstr(message, "SET");
  //   Serial.println("-------------------------DEBUG------------------------");
  //   Serial.println(p);
  //   Serial.println(s);
  //   Serial.println(e);
  //   Serial.println(i);
  //   Serial.println(t);
  //   Serial.println("-------------------------DEBUG------------------------");
  if (p != NULL) {
    Serial.println("PSWD OK");
    if (s != NULL && e == NULL && i == NULL && t == NULL) {
      Serial.println("THERMOSTAT STARTED");
      force_STOP = false;
      digitalWrite(BLUE_RGB, HIGH);
      digitalWrite(GREEN_RGB, HIGH);
      digitalWrite(RED_RGB, HIGH);
    }
    else if (s == NULL && e != NULL && i == NULL && t == NULL) {
      Serial.println("THERMOSTAT STOPPED");
      force_STOP = true;
      manual_control = false;
      ManageThermostat(0);
    }
    else if (s == NULL && e == NULL && i != NULL && t == NULL) {
      Serial.print("\nTHERMOSTAT's STATUS is: ");
      //Send an sms with the status, current temp and target temp
      char buffertxt[6];
      char smstxt[160];
      memset(smstxt, 0, sizeof(smstxt)); //sets all bytes to 0 so it empties the char array 
      strcpy(smstxt, "status=");
      status=digitalRead(RELAY1);
      if(status==1){
        strcat(smstxt,"Working, current temp=");
      }
      else{
        if(force_STOP==true){
          strcat(smstxt,"OFF, current temp=");
        }
        else {
          strcat(smstxt,"Paused, current temp=");
        }
      }
      dtostrf(celsius, 5, 2, buffertxt);
      strcat(smstxt,buffertxt);
      strcat(smstxt,", target=");
      memset(buffertxt, 0, sizeof(buffertxt));
      dtostrf(target, 5, 2, buffertxt);
      strcat(smstxt, buffertxt);
      Serial.println("SMS with the following text is prepared to send:");
      Serial.println(smstxt);
      if (sms.SendSMS(NumberToSendSMS, smstxt)){
          Serial.println("SMS sent OK");
      }
      else {
        Serial.println("ERROR: SMS failed to send");
      }
    }
    else if (s == NULL && e == NULL && i == NULL && t != NULL) {
      char getnewtarget[4];
      t = t + strlen("SET");
      strncpy(getnewtarget, t, 4);
      float newtarget = atof(getnewtarget);
      if (newtarget != 0.00 and newtarget >= manual_min_temp and newtarget <= manual_max_temp) {
        manual_control = false;
        target = newtarget;
        Serial.print("New target set at:");
        Serial.println(newtarget);
        lcd.setCursor(0, 1);
        lcd.print("Set:");
        lcd.print(target, 1);
        lcd.print("C ");
      }
    }
  }
  else
  {
    Serial.println("PSWD WRONG!");
    //send an sms informing of false password try..
  }
  //resetting pointers to NULL values
  *p = NULL;
  *s = NULL;
  *e = NULL;
  *i = NULL;
  *t = NULL;
}


void deleteSMS(int pos) {
  //Delete the message in (int)pos
  //sms.DeleteSMS((int)pos);

  //Delete ALL the messages
  for (int i = 1; i <= (int)pos; i++) {
    del = sms.DeleteSMS(i);
    Serial.print("msg in pos: ");
    Serial.print(i);
    if (del == 1) {
      Serial.println(" deleted successfuly!");
      lcd.setCursor(9, 0);
      lcd.print(" GSM ok");
    }
    else
    {
      Serial.println(" was not deleted - ERROR!");
    }
  }
}

void ManageThermostat(boolean working_status) { // (working_status=0 PAUSE/OFF, working_status=1 WORK)
  if (working_status == 0) {
    digitalWrite(RELAY1, LOW);
    if (force_STOP == true) {
      lcd.setCursor(10, 1);
      lcd.print("   OFF");
      digitalWrite(BLUE_RGB, HIGH);
      digitalWrite(GREEN_RGB, HIGH);
      digitalWrite(RED_RGB, HIGH);
    }
    else
    {
      lcd.setCursor(10, 1);
      lcd.print("Paused");
      digitalWrite(BLUE_RGB, LOW);
      digitalWrite(GREEN_RGB, HIGH);
      digitalWrite(RED_RGB, HIGH);
    }
  }
  else if (working_status == 1) {
    digitalWrite(RELAY1, HIGH);
    lcd.setCursor(10, 1);
    lcd.print("Workin");
    digitalWrite(BLUE_RGB, HIGH);
    digitalWrite(GREEN_RGB, HIGH);
    digitalWrite(RED_RGB, LOW);
  }
}

