HOW TO USE THE GSM & GPS SHIELD WITH ARDUINO MEGA

For problems with the RAM, or simply for projects that require a larger number of input/output, we can use with the GSM/GPRS & GPS shield the Arduino Mega.  Thanks to four serial port, we can use one of these instead of the serial software to communicate with the shield.

With the latest release, the library can be used completely with Arduino Mega. You must open the file GSM.h and select the tab used appropriately commenting lines of code.

Using the shield with Arduino Mega we comment as follows:

 

//#define UNO

#define MEGA

If we want to use Arduino UNO:

#define UNO

//#define MEGA

 

Similarly, also the file HWSerial.h, must be configured. As before, we see the first example for Arduino Mega:

 

#define MEGA

 

Using the file HWSerial.h is not necessary to define the possible use with Arduino Uno, as implemented in the class it is only used by the hardware serial.

The library uses the serial Serial0 (TX 1 RX 0) for external communication and the serial Serial1 (RX 18 TX 19) to communicate with SIM900 and SIM908. Nothing prevents you replace every occurrence of Serial1 with Serial2 or one that you prefer.

Please note that to use the serial hardware you need to create the connection between the shield and Arduino Mega using a bridge: the TX pin of the shield must be connected to TX1 18 and the RX pin with the RX1 19.
To make the “bridge” I remove the 2 jumbers from the TX / RX triplets on the GSM board and I connect each wire to the middle pin of TX / RX as in the image below:



*** Also in HWSerial.cpp change line 55 (or 63) to:
const char PROGMEM *p = (const char PROGMEM *)ifsh;

otherwise the sketch won’t compile with the latest Arduino versions!http://forum.arduino.cc/index.php?topic=307728.0

(It might be already there if you downloaded the latest github code from https://github.com/MarcoMartines/GSM-GPRS-GPS-Shield)
