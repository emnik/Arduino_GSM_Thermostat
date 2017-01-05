#define DELAY 100

//Analog read pins
const int xPin = 0;
const int yPin = 1;
const int zPin = 2;

int received;              // characters received
char buffer[10];      // input buffer
int N;                // how many measurements to make
char temp;
boolean done = false;


//The minimum and maximum values that came from
//the accelerometer while standing still
//You very well may need to change these
int minValx = 269;
int maxValx = 403;
int minValy = 263;
int maxValy = 397;
int minValz = 273;
int maxValz = 405;

//to hold the caculated values
double theta;
double psi;
double phi;


void setup(){
  Serial.begin(9600); 
}


void loop(){
  received = 0;
  buffer[received] = '\0';
  temp = ' ';
  done = false;

  // Check input on serial line.
  while (!done) {
    if (Serial.available()) { // Something is in the buffer
      N = Serial.read();  // so get the number byte
      done = true;
    }
  }
  
  // Now we know how many to get, so get them.
  for (byte j=0;j<N;j++) {
    Serial.print(millis(), DEC);
    Serial.print('\t');
    
    //read the analog values from the accelerometer
    int xRead = analogRead(xPin);
    int yRead = analogRead(yPin);
    int zRead = analogRead(zPin);
  
    //convert read values to degrees -90 to 90 - Needed for atan2
    int xAng = map(xRead, minValx, maxValx, -90, 90);
    int yAng = map(yRead, minValy, maxValy, -90, 90);
    int zAng = map(zRead, minValz, maxValz, -90, 90);
  
    //Caculate 360deg values like so: atan2(-yAng, -zAng)
    //atan2 outputs the value of -π to π (radians)
    //We are then converting the radians to degrees
    
    //The atan2() function calculates the arc tangent of the two variables y and x. It is similar to calculating the arc 
    //tangent of y / x, except that the signs of both arguments are used to determine the quadrant of the result.
    // atan2(y,x) = arctan(y/x)
    theta = RAD_TO_DEG * (atan2(-xAng, -zAng) + PI);    
    psi = RAD_TO_DEG * (atan2(-yAng, -zAng) + PI);
    phi = RAD_TO_DEG * (atan2(-xAng, -yAng) + PI);

      
    //Output the caculations
      Serial.print((int)theta);
      Serial.print('\t');
      Serial.print((int)psi);
      Serial.print('\t');
      Serial.print((int)phi);
      Serial.print('\n');
      delay(DELAY);
  }

  //delay(500);//just here to slow down the serial output - Easier to read

}

