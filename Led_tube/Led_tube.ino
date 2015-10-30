/*
* 4 Bit Digital LED Tube Module with two 74HC595 shift registers
* Nikiforakis Manos ver. 1.0
* 9/6/2015
*/

//VCC to 3.3V (although it can work with 5V without problem)
const int latchPin = 6; // connect to RCLK
const int clockPin = 5; // connect to SCLK
const int dataPin = 4;  // connect to DIO

byte col[4] = {
  B01111111, //digit 1 
  B10111111, //digit 2
  B11011111, //digit 3
  B11101111  //digit 4 -the left most
};

/* A SEGMENT WITH 7 BITS TO FORM A DIGIT
   LSBFIRST REPRESENTATION
      
     --1--
   6|     |2
     --7--
   5|     |3
     --4--  .8
      
*/

byte dig[14] = {
//B12345678 1=high, 0=low
  B11111100, //0
  B01100000, //1
  B11011010, //2
  B11110010, //3
  B01100110, //4
  B10110110, //5
  B10111110, //6
  B11100000, //7
  B11111110, //8
  B11110110, //9
  B11101110, //A
  B11111110, //B - same as 8
  B00011010, //c
  B00000001  //.
};     

int digitBuffer[2] = {0};
float tempC;
  
void setup()
{
  pinMode(latchPin, OUTPUT) ;
  pinMode(clockPin, OUTPUT) ;
  pinMode(dataPin, OUTPUT) ;
};

void updateDisp(){
  for(int j=3; j>0; j--)  {
      digitalWrite(latchPin, LOW); 
      
      if (j==2){ 
        shiftOut(dataPin, clockPin, LSBFIRST, (dig[digitBuffer[j-1]]|dig[13]));
        shiftOut(dataPin, clockPin, LSBFIRST, col[j]);
      }
      else
      {
        shiftOut(dataPin, clockPin, LSBFIRST, dig[digitBuffer[j-1]]);
        shiftOut(dataPin, clockPin, LSBFIRST, col[j]);
      };    
      digitalWrite(latchPin, HIGH); 
      delay(4);
  }
  digitalWrite(latchPin, LOW); 
  shiftOut(dataPin, clockPin, LSBFIRST, dig[12]); //c : celcius
  shiftOut(dataPin, clockPin, LSBFIRST, col[0]); 
  digitalWrite(latchPin, HIGH);
  delay(2);
}

void loop()
{
  tempC = int(28.6*10);
  digitBuffer[2] = int(tempC)/100; //hundrends
  digitBuffer[1] = (int(tempC)%100)/10; //tens
  digitBuffer[0] = (int(tempC)%100)%10;//units
  updateDisp();
  delay(2);
//  digitalWrite(latchPin, LOW); 
//  shiftOut(dataPin, clockPin, LSBFIRST, dig[2]);
//  shiftOut(dataPin, clockPin, LSBFIRST, col[3]);
//  digitalWrite(latchPin, HIGH); 
//  delay(2);
//
//  digitalWrite(latchPin, LOW); 
//  shiftOut(dataPin, clockPin, LSBFIRST, (dig[8]|dig[13])); //add the decimal sign
//  shiftOut(dataPin, clockPin, LSBFIRST, col[2]);
//  digitalWrite(latchPin, HIGH); 
//  delay(2);
//  digitalWrite(latchPin, LOW); 
//
//  shiftOut(dataPin, clockPin, LSBFIRST, dig[5]); 
//  shiftOut(dataPin, clockPin, LSBFIRST, col[1]); 
//  digitalWrite(latchPin, HIGH); 
//  delay(2);
//  digitalWrite(latchPin, LOW); 
//
//  shiftOut(dataPin, clockPin, LSBFIRST, dig[12]); //c : celcius
//  shiftOut(dataPin, clockPin, LSBFIRST, col[0]); 
//  digitalWrite(latchPin, HIGH);
//  delay(2);
}

