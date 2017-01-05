#include <XBee.h>
#include <OneWire.h>
#include <DallasTemperature.h>

// Data wire is plugged into pin 2 on the Arduino
#define ONE_WIRE_BUS 2

OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

XBee xbee = XBee();

unsigned long start = millis();

// allocate 4 bytes to hold 1 float
uint8_t payload[4] = {0, 0, 0, 0};

// with Series 1 you can use either 16-bit or 64-bit addressing

// 16-bit addressing: Enter address of remote XBee, typically the coordinator
Tx16Request tx = Tx16Request(0x0001, payload, sizeof(payload));

// 64-bit addressing: This is the SH + SL address of remote XBee
//XBeeAddress64 addr64 = XBeeAddress64(0x0013a200, 0x4008b490);
// unless you have MY on the receiving radio set to FFFF, this will be received as a RX16 packet
//Tx64Request tx = Tx64Request(addr64, payload, sizeof(payload));

TxStatusResponse txStatus = TxStatusResponse();

//int pin5 = 0;
//int statusLed = 13;

union {
      float f;
      byte b[4];
   } stuff;

void setup() {
//  pinMode(statusLed, OUTPUT);
  Serial.begin(9600);
  xbee.setSerial(Serial);
  sensors.begin();
//  flashLed(statusLed, 30, 50);
}

void loop() {
   sensors.requestTemperatures(); // Send the command to get temperatures
   // start transmitting after a startup delay.  Note: this will rollover to 0 eventually so not best way to handle
    if (millis() - start > 15000) {
      
        stuff.f = sensors.getTempCByIndex(0);

	payload[0] = stuff.b[0];
	payload[1] = stuff.b[1];
	payload[2] = stuff.b[2];
        payload[3] = stuff.b[3];

      xbee.send(tx);
      delay(2000);               // Delay to allow reading of the message

      // flash TX indicator
      //flashLed(statusLed, 5, 100);
    }
  
    // after sending a tx request, we expect a status response
    // wait up to 5 seconds for the status response
    if (xbee.readPacket(5000)) {
        // got a response!

        // should be a znet tx status            	
    	if (xbee.getResponse().getApiId() == TX_STATUS_RESPONSE) {
    	   xbee.getResponse().getZBTxStatusResponse(txStatus);
    		
    	   // get the delivery status, the fifth byte
           if (txStatus.getStatus() == SUCCESS) {
            	// success.  time to celebrate
             	//flashLed(statusLed, 5, 50);
           } else {
            	// the remote XBee did not receive our packet. is it powered on?
             	//flashLed(errorLed, 3, 500);
               //flashLed(statusLed, 3, 1000);
           }
        }      
    } else if (xbee.getResponse().isError()) {
      //nss.print("Error reading packet.  Error code: ");  
      //nss.println(xbee.getResponse().getErrorCode());
      // or flash error led
    } else {
      // local XBee did not provide a timely TX Status Response.  Radio is not configured properly or connected
      //flashLed(errorLed, 2, 50);
      //flashLed(statusLed, 5, 1000);
    }
    
    delay(3000);
}

//void flashLed(int pin, int times, int wait) {
    
//    for (int i = 0; i < times; i++) {
//      digitalWrite(pin, HIGH);
//      delay(wait);
//      digitalWrite(pin, LOW);
      
//      if (i + 1 < times) {
//        delay(wait);
//      }
//    }
//}
