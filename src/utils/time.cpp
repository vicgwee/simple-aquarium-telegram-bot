#include <NTPClient.h>
#include <WiFiUdp.h>
#include "time.h"

uint32_t init_epoch_seconds;
uint32_t init_millis;

void initTimeClient(){
  WiFiUDP ntpUDP;
  NTPClient timeClient(ntpUDP, "sg.pool.ntp.org", 8*3600, 3600000);
  timeClient.begin();
  if(timeClient.forceUpdate()){
    Serial.println(timeClient.getFormattedTime());
    init_epoch_seconds = timeClient.getEpochTime();
    init_millis = millis();
  };
  timeClient.end();
}

uint32_t getUptimeSeconds(){
  return millis() / 1000;
}

// Returns the seconds since the start of the day
uint32_t getDaySeconds(){
  uint32_t seconds_since_init = (millis() - init_millis) / 1000UL;
  return (init_epoch_seconds + seconds_since_init) % (24*60*60);
}
