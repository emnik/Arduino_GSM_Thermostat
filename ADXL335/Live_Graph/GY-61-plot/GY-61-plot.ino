/*ADXL335
note:vcc-->5v ,but ADXL335 Vs is 3.3V
The circuit:
      5V: VCC
analog 1: x-axis
analog 2: y-axis
analog 3: z-axis
*/


const int xpin = 0;                  // x-axis of the accelerometer
const int ypin = 1;                  // y-axis
const int zpin = 2;                  // z-axis (only on 3-axis models)

const int x_zero = 337;
const int y_zero = 331;
const int z_zero = 338;

const float sensitivity = 330.0/1000.0; // sensitivity = 330mV per g based on the datasheet
const float g = 9.8;

const float ADC_Ref = 5.0;          //Arduino ADC is referenced at 5.0 Volts
                                    //If one wants to change it he should connect the desired Reference voltage to AREF pin
                                    
                                    //As the sensor operates at 3.3V (it has a voltage regulator on board) we would get a 
                                    //better precision if a 3.3V Reference was used. 
                                    //Ideal either the output of the on board regulator would connect to arduino's AREF, 
                                    //or we would power the sensor from an external voltage regulator and then use the same
                                    //output to connect the AREF pin.

void setup()
{
  // initialize the serial communications:
  Serial.begin(9600);
}

void loop()
{
  int x = analogRead(xpin);  //ADC read from xpin
  int y = analogRead(ypin);  //ADC read from ypin
  int z = analogRead(zpin);  //ADC read from zpin

  float xV = (float)(x-x_zero)*ADC_Ref/1024; //x ADC value converted to Volts
  float yV = (float)(y-y_zero)*ADC_Ref/1024; //y ADC value converted to Volts
  float zV = (float)(z-z_zero)*ADC_Ref/1024; //z ADC value converted to Volts

  float xg = xV/sensitivity; //x Volts converted to how many g
  float yg = yV/sensitivity; //y Volts converted to how many g
  float zg = zV/sensitivity; //z Volts converted to how many g

  float xaccel = xg*g; //x axis acceleration in m/sec^2
  float yaccel = yg*g; //y axis acceleration in m/sec^2
  float zaccel = zg*g; //z axis acceleration in m/sec^2

  float absolute_accel=sqrt(xaccel*xaccel+yaccel*yaccel+zaccel*zaccel); //absolute acceleration in m/sec^2

  Serial.print(xaccel); 
  Serial.print(" ");
  Serial.print(yaccel);
  Serial.print(" ");
  Serial.print(zaccel);  
//  Serial.print(" ");
//  Serial.print(absolute_accel);  
  Serial.print("\n");
  
  delay(50);
}
