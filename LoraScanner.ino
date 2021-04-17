/*
LoRa Barcode Scanner Project
Based on the Keyboard Controller example
 */

// Require keyboard control library
#include <KeyboardControllerSAMD.h>
#include <RH_RF95.h>
#include <SPI.h>

// On SAMD boards where the native USB port is also the serial console, use
// Serial1 for the serial console. This applies to all SAMD boards except for
// Arduino Zero and M0 boards.
#if (USB_VID==0x2341 && defined(ARDUINO_SAMD_ZERO)) || (USB_VID==0x2a03 && defined(ARDUINO_SAM_ZERO))
#define SerialDebug SERIAL_PORT_MONITOR
#else
#define SerialDebug Serial1
#endif

//define LORA pins/frequency for feather M0
#define RFM95_CS 8
#define RFM95_RST 4
#define RFM95_INT 3
#define RF95_FREQ 915.0

// Singleton instance of the radio driver
RH_RF95 rf95(RFM95_CS, RFM95_INT);

// Blinky on receipt
#define LED 13

// Initialize USB Controller
USBHost usb;

// Attach keyboard controller to USB
KeyboardControllerSAMD keyboard(usb);

String currentBarcode;

void sendBarcode(String myBarcode){
  SerialDebug.println("Transmitting " + myBarcode);
  char sendChars[sizeof(myBarcode)];
  myBarcode.toCharArray(sendChars, sizeof(myBarcode));
  //uint8_t sendChars  = atoi (myBarcode.c_str ()); 
  rf95.send((const uint8_t*)sendChars, sizeof(sendChars));
  rf95.waitPacketSent();
//  // Now wait for a reply
//  uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
//  uint8_t len = sizeof(buf);
//  
//  if (rf95.waitAvailableTimeout(3000))
//  { 
//    // Should be a reply message for us now   
//    if (rf95.recv(buf, &len))
//   {
//      SerialDebug.print("got reply: ");
//      SerialDebug.println((char*)buf);
//      SerialDebug.print("RSSI: ");
//      SerialDebug.println(rf95.lastRssi(), DEC);
//    }
//    else
//    {
//      SerialDebug.println("recv failed");
//    }
//  }
//  else
//  {
//    SerialDebug.println("No reply, is rf95_server running?");
//  }
}

// This function intercepts key press
void keyPressed() {
  if ( keyboard.getOemKey() == 40 ){
    sendBarcode(currentBarcode);
    currentBarcode = "";
  }
  else{
    currentBarcode += String((char)keyboard.getKey());
  }
}


uint32_t lastUSBstate = 0;

void setup()
{
  //setup LORA items
  pinMode(LED, OUTPUT);
  pinMode(RFM95_RST, OUTPUT);
  digitalWrite(RFM95_RST, HIGH);

  //setup debug
  SerialDebug.begin( 115200 );
  while (!SerialDebug) {
        delay(1);
  }
  delay(100);
  SerialDebug.println("SCCNH Scanner Program started");
  // manual reset
  digitalWrite(RFM95_RST, LOW);
  delay(10);
  digitalWrite(RFM95_RST, HIGH);
  delay(10);
  while (!rf95.init()) {
        SerialDebug.println("LoRa radio init failed");
        SerialDebug.println("Uncomment '#define SERIAL_DEBUG' in RH_RF95.cpp for detailed debug info");
        while (1);
  }
  if (!rf95.setFrequency(RF95_FREQ)) {
        SerialDebug.println("setFrequency failed");
        while (1);
  }
  SerialDebug.print("Set Freq to: "); SerialDebug.println(RF95_FREQ);
  rf95.setTxPower(23, false);
  
  if (usb.Init())
	  SerialDebug.println("USB host did not start.");

  delay( 20 );
  currentBarcode = "";
}

void loop()
{
  // Process USB tasks
  usb.Task();

  uint32_t currentUSBstate = usb.getUsbTaskState();
  if (lastUSBstate != currentUSBstate) {
    SerialDebug.print("USB state changed: 0x");
    SerialDebug.print(lastUSBstate, HEX);
    SerialDebug.print(" -> 0x");
    SerialDebug.println(currentUSBstate, HEX);
    switch (currentUSBstate) {
      case USB_ATTACHED_SUBSTATE_SETTLE: SerialDebug.println("Device Attached"); break;
      case USB_DETACHED_SUBSTATE_WAIT_FOR_DEVICE: SerialDebug.println("Detached, waiting for Device"); break;
      case USB_ATTACHED_SUBSTATE_RESET_DEVICE: SerialDebug.println("Resetting Device"); break;
      case USB_ATTACHED_SUBSTATE_WAIT_RESET_COMPLETE: SerialDebug.println("Reset complete"); break;
      case USB_STATE_CONFIGURING: SerialDebug.println("USB Configuring"); break;
      case USB_STATE_RUNNING: SerialDebug.println("USB Running"); break;
    }
    lastUSBstate = currentUSBstate;
  }
}
