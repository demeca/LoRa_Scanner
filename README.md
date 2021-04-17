# Lora Scanner
This pairs with the Lora Scanner Reciever project. This project is desigened around an Adafruit Feather M0 LoRa dev board. This board has a SAMD21 processor that includes an onboard USB host mode. This is used to read input from a USB Barcode scanner. The goal is to provide our autocross club a long range barcode scanner to queue cars at the start line.  
The scanner must present itself as a HID Boot Keyboard. Mine was a cheap $30 one from Amazon that is paired with an OTG cable that provides power as well.  
The Arduino IDE needs an additional board URL for the Adafruit boards: https://adafruit.github.io/arduino-board-index/package_adafruit_index.json  

## Flashing the M0
You need to push the reset button twice, then the LEE will pulse and it is ready to be programmed.  

## USBSAMD
I've included the library that worked for me. The first one I tried did not work with my USB scanner.