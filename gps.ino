#define ESP32_RTOS  // Uncomment this line if you want to use the code with freertos only on the ESP32
#include "OTA.h"
#include "GPS.hpp"
#include "SIM.hpp"

#define mySSID "Mars"
#define myPASSWORD "counterclockwise"
SIM mysim(&Serial);

void traker(void* nth){
  unsigned long now = millis();
  char dataBuff[] = "{\"location\":\"https://www.google.com/maps/@33.3333333,33.3333333,15.5z\",\"number of Satalites\":\"0\"}";
  double lat = 0.0, lon = 0.0;
  while (true){
    while (Serial2.available()) {  
     if( millis()-now>=1000){
        now = millis();
        int msgType = processGPS();
        if ( msgType == MT_NAV_POSLLH ) {
          lat = ubxMessage.navPosllh.lat/10000000.000000f;
          lon = ubxMessage.navPosllh.lon/10000000.000000f;
          sprintf(dataBuff,"{\"location\":\"https://www.google.com/maps/@%0.7f,%0.7f,15.5z\",\"number of Satalites\":\"%d\"}",lat,lon,ubxMessage.navStatus.gpsFix);  
        }
        mysim.POST(String("https://easy-flow-33dec.firebaseio.com/locatin.json"), String(dataBuff));
        TelnetStream.println("posted some data");
      }
    }
    vTaskDelay(500 / portTICK_PERIOD_MS);
    ArduinoOTA.handle();
    TelnetStream.println("In tracker ...");
 }
}

void setup(){
  TelnetStream.println("Code Started");
  setupOTA("Alis", mySSID, myPASSWORD);
  mysim.StratGPRS();
  Serial2.begin(9600); //for gps
  mysim.POST(String("https://easy-flow-33dec.firebaseio.com/locatin.json"), String("{\"started\":\"yes\"}"));
  xTaskCreate(traker,"traking_code",1024*8,NULL,1,NULL);
  TelnetStream.println("Leaving Setup!");
}

void loop() {
  ArduinoOTA.handle();
  delay(500);
}
