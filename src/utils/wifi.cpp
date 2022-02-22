#include "wifi.h"
#include "../comms/telegramBot.h"

#define WIFI_CONFIG_PORTAL_TIMEOUT 120

bool shouldSaveConfig;

void saveConfigCallback () {
  Serial.println(F("Should save config"));
  shouldSaveConfig = true;
}

void initWifi(){
  WiFiManager wifiManager;
  WiFiManagerParameter custom_bot_id("botid", "Bot Token", botToken, 50);
  
  Serial.println(F("Send r to reset settings..."));
  delay(2000);
  if (Serial.available()>0){
    if(Serial.read() == 'r'){
      Serial.println(F("Resetting settings"));
      wifiManager.resetSettings(); //RESET SETTINGS
    }
  }
  wifiManager.setSaveConfigCallback(saveConfigCallback);
  wifiManager.addParameter(&custom_bot_id);
  wifiManager.setConfigPortalTimeout(WIFI_CONFIG_PORTAL_TIMEOUT);
  wifiManager.setBreakAfterConfig(true);

  if(!wifiManager.autoConnect("TELEGRAM-BOT")){
    Serial.println(F("Could not connect, resetting"));
    delay(3000);
    ESP.reset();
    delay(5000);
  }

  Serial.println(F("WiFi connected"));
  Serial.print(F("IP address: "));
  IPAddress ip = WiFi.localIP();
  Serial.println(ip);

  if(shouldSaveConfig){
    strcpy(botToken, custom_bot_id.getValue());
  }
}