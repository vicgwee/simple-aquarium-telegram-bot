#include "lights.h"
#include "../utils/time.h"

/*Timer */
const int timer_states = 8;

const int timer_brightness[timer_states] PROGMEM = {10,700,1000,900,900,250,100,0};

const int timer_start[timer_states] PROGMEM = {
  7 *60 + 30,
  9 *60 + 30,
  12*60 + 30,
  17*60     ,
  18*60 + 30,
  19*60 + 30,
  23*60,
  23*60 + 15
  }; //SR start, SS start, TW start night

const char* timer_chars[timer_states] = {"Sunrise", "Morning", "Afternoon","Sunset", "Evening","Twilight","Night", "Night"};

bool timer_on = 1; 
uint8_t timer_state = 0;

//Lights
const int MAX_BRIGHTNESS = 1000;

//LED hardware
const int light_pin = D7;
const int PWM_FREQ = 781;
const int PWM_RES = 1023;

uint32_t lights_lasttime;
const int adjust_brightness_interval = 200;

bool lights_on = 0;
int brightness = 0;

void initLights(){
  analogWriteFreq(PWM_FREQ);
  pinMode(light_pin, OUTPUT);
  digitalWrite(light_pin,0);
}

void writeLights(){
  int pwm_val = (brightness*lights_on*PWM_RES)/(MAX_BRIGHTNESS);
  if (pwm_val <= 1 ){
    digitalWrite(light_pin,0);
  }
  else if (pwm_val >= PWM_RES - 10 ){
    digitalWrite(light_pin,1);
  }
  else{
    analogWrite(light_pin,pwm_val);
  }
}

bool toggleLights(){
  lights_on = !lights_on;
  writeLights();
  return (lights_on);
}

bool toggleTimer(){
  timer_on = !timer_on;
  updateLights();
  return (timer_on);
}

void adjustBrightness(bool dir){
  brightness += (-1)*(!dir)*adjust_brightness_interval + dir*adjust_brightness_interval ;
  brightness = max(0, min(MAX_BRIGHTNESS,brightness));
  if (brightness == 0){
    lights_on = 0;
  }
  else{
    lights_on = 1;
  }
  writeLights();
}

float updateTimeOfDay(int mins){
  if (mins < timer_start[0] || mins >= timer_start[timer_states-1]){
    timer_state = timer_states - 1;
    return 0;
  }
  for(int i = 0; i < timer_states-1 ; i++){
    if (mins >= timer_start[i] && mins < timer_start[i+1]){
      timer_state = i;
      return float(mins- timer_start[i])/(timer_start[i+1]-timer_start[i]);
    }
  }
  timer_state = timer_states - 1;
  return 0;
}

void setTimedLights(float factor){
    Serial.print(F("Timer mode: "));
    Serial.println(timer_chars[timer_state]);
    lights_on = 1;
    if (timer_state == timer_states - 1){ //Night
      if (brightness > 10){
        brightness -= 10;
      }
      else{
        brightness = 0; 
        lights_on = 0; 
      }
      return;
    }

    brightness = timer_brightness[timer_state] * (1-factor) + timer_brightness[timer_state+1] * factor;
  
}

void updateLights(){
  int mins = getDaySeconds() / 60UL;

  float factor = updateTimeOfDay(mins);
  if(timer_on){
    setTimedLights(factor);
    writeLights();
  }
}