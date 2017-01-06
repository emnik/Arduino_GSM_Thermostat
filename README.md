# Arduino_GSM_Thermostat
An SMS controlled home thermostat made with an arduino

It is an Arduino home thermostat based on the digital temperature sensor DS18b20 that can me controlled either manually via a rotary knob or remotely via sms!

The SMS commands are: 
* passwrdSETxx.xx (where xx.xx is the temperature we want e.g 24.00) 
* passwrdSTART 
* passwrdSTOP 
* passwrdSTATUS 

The last command **responds with an SMS message** about the set point, the current temperature and the current status (Working|Paused|OFF) of the thermostat. 

_the password is needed to provide unauthorised access and you can set your own password in the Arduino_GSM_LCD_Thermostat.ino file (line 258). The default is "**passwrd**". If the message does not begin with the right password, then it is discarded!_

The schematic (can also be found in the corresponding PDF file) is shown below:
![schematic image](https://github.com/emnik/Arduino_GSM_Thermostat/blob/master/images/im3.jpg)

You can find the part list in the https://github.com/emnik/Arduino_GSM_Thermostat/blob/master/Parts_list.md.
The total cost of this project is around 90 - 100euros.

The final project looks like this: 

Inside view:
![inside_photo](https://github.com/emnik/Arduino_GSM_Thermostat/blob/master/images/im1_small.jpg)

Outside view:
![outside photo](https://github.com/emnik/Arduino_GSM_Thermostat/blob/master/images/im2_small.jpg)
