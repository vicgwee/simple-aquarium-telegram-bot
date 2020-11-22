#include "../comms/commands.h"
#include "../comms/telegramBot.h"
#include "../lights/lights.h"
#include "../utils/time.h"

const char* commands[] = {
  "/stats",
  "/geek_stats",
  "/toggle_lights",
  "/increase", 
  "/decrease", 
  "/toggle_timer",
  "/state"
};
const uint8_t n_commands = sizeof(commands)/ sizeof(commands[0]);

char reply1[MAX_REPLY_LENGTH];
char reply2[MAX_REPLY_LENGTH];
char reply3[MAX_REPLY_LENGTH];
char* replies[] = {reply1, reply2, reply3};

void messageHandler(const char *message, int i);
int8_t parseCommand(const char *message);
void cmd_stats(int i);
void cmd_geekstats(int i);
void cmd_toggleLights(int i);
void cmd_increaseBrightness(int i);
void cmd_decreaseBrightness(int i);
void cmd_toggleTimer(int i);
void cmd_timeOfDay(int i);
void cmd_unknown(int i);

void messageHandler(const char *message, int i){
  static void (*const cmd_jump_table[n_commands])(int) PROGMEM = {
    cmd_stats, 
    cmd_geekstats, 
    cmd_toggleLights, 
    cmd_increaseBrightness, 
    cmd_decreaseBrightness, 
    cmd_toggleTimer, 
    cmd_timeOfDay, 
  };
  int8_t command = parseCommand(message);
  if(command < 0 || command >= n_commands){
    cmd_unknown(i);
  }
  else{
    cmd_jump_table[command](i);
  }
}

int8_t parseCommand(const char *message){
  if (message[0] == '/'){
    for(int i = n_commands-1; i >= 0; i--){
      if (strcmp(message, commands[i])==0){
        return i;
      }
    }
  }
  return -1;
}

void cmd_stats(int i){
  uint32_t time= timeClient.getEpochTime() - start_epoch_time;
  uint32_t day = time / (24 * 3600);
  time = time % (24 * 3600);
  uint8_t hour = time / 3600;
  time %= 3600;
  uint8_t min = time / 60;
  time %= 60;
  uint8_t sec = time;

  snprintf_P(
    replies[i], MAX_REPLY_LENGTH,
    (PGM_P)F("I've been alive for: %lud %uh %um %us\nI've replied %lu times"),
    day,hour,min,sec,messages_replied+1
  );
}

void cmd_geekstats(int i){
  snprintf_P(
    replies[i], MAX_REPLY_LENGTH,
    (PGM_P)F("Last reset reason: %s\nFree Heap: %luB\nFragmentation: %u%%"),
    ESP.getResetReason().c_str(),ESP.getFreeHeap(), ESP.getHeapFragmentation()
  );
}

void cmd_toggleLights(int i){
  if(toggleLights()){
    strcpy_P(replies[i],(PGM_P)F("It's bright!"));
  }
  else{
    strcpy_P(replies[i],(PGM_P)F("It's dark!"));
  }
}

void cmd_increaseBrightness(int i){
  adjustBrightness(1);
  snprintf_P(
    replies[i], MAX_REPLY_LENGTH,
    (PGM_P)F("The lights are increased to %u%%"),
    brightness/10
  );
}

void cmd_decreaseBrightness(int i){
  adjustBrightness(0);
  snprintf_P(
    replies[i], MAX_REPLY_LENGTH,
    (PGM_P)F("The lights are dimmed to %u%%"),
    brightness/10
  );
}

void cmd_toggleTimer(int i){
  if(toggleTimer()){
    strcpy_P(replies[i],(PGM_P)F("Timer mode active"));
  }
  else{
    strcpy_P(replies[i],(PGM_P)F("I'll follow your commands"));
  }
}

void cmd_timeOfDay(int i){
  snprintf_P(
    replies[i], MAX_REPLY_LENGTH,
    (PGM_P)F("It's now %s and the lights are at %u%%"),
    timer_chars[timer_state], brightness/10*lights_on
  );
}

void cmd_unknown(int i){
  strcpy_P(replies[i],  (PGM_P)F("Command not understood"));
}
