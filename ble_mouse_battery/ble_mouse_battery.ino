#include <Wire.h>
#include "SparkFun_Qwiic_Joystick_Arduino_Library.h"

#define ESP_32

#ifdef ESP_32
  #include <BleMouse.h>
  #include <BleConnectionStatus.h>
#else
  #include <bluefruit.h>
  BLEDis bledis;
  BLEHidAdafruit blehid;
#endif

JOYSTICK joystick; //Create instance of this object

void setup() {
  Serial.begin(115200);
  
  #ifdef ESP_32
    setupEsp32();
  #else
    while ( !Serial ) delay(10);   // for nrf52840 with native usb
    setupBluefruit();
  #endif

  Serial.println("Qwiic Joystick Example");
  
  if(joystick.begin() == false)
  {
    Serial.println("Joystick does not appear to be connected. Please check wiring. Freezing...");
    while(1);
  }
}
#ifdef ESP_32
  void setupEsp32() {
    
  }
#else
  void setupBluefruit() {
    Serial.print("setupBlueFruit");
    Bluefruit.begin();
    // HID Device can have a min connection interval of 9*1.25 = 11.25 ms
    Bluefruit.Periph.setConnInterval(9, 16); // min = 9*1.25=11.25 ms, max = 16*1.25=20ms
    Bluefruit.setTxPower(4);    // Check bluefruit.h for supported values
    Bluefruit.setName("Bluefruit52");
  
    // Configure and Start Device Information Service
    bledis.setManufacturer("Adafruit Industries");
    bledis.setModel("Bluefruit Feather 52");
    bledis.begin();
  
    // BLE HID
    blehid.begin();

    // Set up and start advertising
    startAdv();
  }

  void startAdv(void)
  {  
    // Advertising packet
    Bluefruit.Advertising.addFlags(BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE);
    Bluefruit.Advertising.addTxPower();
    Bluefruit.Advertising.addAppearance(BLE_APPEARANCE_HID_MOUSE);
    
    // Include BLE HID service
    Bluefruit.Advertising.addService(blehid);
  
    // There is enough room for 'Name' in the advertising packet
    Bluefruit.Advertising.addName();
    
    /* Start Advertising
     * - Enable auto advertising if disconnected
     * - Interval:  fast mode = 20 ms, slow mode = 152.5 ms
     * - Timeout for fast mode is 30 seconds
     * - Start(timeout) with timeout = 0 will advertise forever (until connected)
     * 
     * For recommended advertising interval
     * https://developer.apple.com/library/content/qa/qa1931/_index.html   
     */
    Bluefruit.Advertising.restartOnDisconnect(true);
    Bluefruit.Advertising.setInterval(32, 244);    // in unit of 0.625 ms
    Bluefruit.Advertising.setFastTimeout(120);      // number of seconds in fast mode
    Bluefruit.Advertising.start(0);                // 0 = Don't stop advertising after n seconds  
  }
#endif
void loop() {
  int32_t x = joystick.getHorizontal();
  int32_t y = joystick.getVertical();
  blehid.mouseMove(x, y);
  
  Serial.print("X: ");
  Serial.print(joystick.getHorizontal());

  Serial.print(" Y: ");
  Serial.print(joystick.getVertical());
  
  Serial.print(" Button: ");
  Serial.println(joystick.getButton());

  delay(20);
}
