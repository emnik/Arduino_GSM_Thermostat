Be carefull to use the right library for the Ethernet shield you have! There is a version 1, version 2 and version 3 of the ethernet shield that have different versions of libraries that are not compatible!!!
For example a version 1 Ethernet Shield without an SD slot needs library version 1 and in the code: #include <Ethernet.h>
while a version 2 Ethernet with an SD slot needs a library version 2 and in the code: #include <Ethernet2.h>

Also in version 1 shield we could use whatever MAC address we may want but in version 2 the MAC address must be the correct one and it is usually printed on a sticker on the shield itself!

To make it work with either shield I needed the following commands:
byte ip[] = {10, 185, 9, 146};
byte gateway[] = {10, 185, 9, 1};
byte subnet[] = {255, 255, 255, 0};
