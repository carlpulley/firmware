#include "application.h"
#include "stdarg.h"
#include "component/DimmerUnit.h"
#include "component/SwitchUnit.h"

#define LOOP_WAIT 90 // milliseconds
#define UNIT_SIZE 2

PRODUCT_ID(PLATFORM_ID);
PRODUCT_VERSION(3);

#if Wiring_WiFi
STARTUP(System.enable(SYSTEM_FLAG_WIFITESTER_OVER_SERIAL1));
#endif

SYSTEM_MODE(AUTOMATIC);

/* Components ----------------------------------------------------------------*/

DimmerUnit dimmer[1] = {
  DimmerUnit(0),
  DimmerUnit(1)
};

SwitchUnit light[UNIT_SIZE] = {
  SwitchUnit(2),
  SwitchUnit(3)
};

void loop() {
  for (int i = 0; i < UNIT_SIZE; i++) {
    dimmer[i].poll();
    light[i].poll();
  }
  delay(LOOP_WAIT);
}

/* REST API setup ------------------------------------------------------------*/

int setLevel(String command);
int setOn(String command);
int setOff(String command);

void setup() {
  for (int i = 0; i < UNIT_SIZE; i++) {
    Particle.variable("level"+String(i), dimmer[i].getLevel());
    Particle.variable("on"+String(i), dimmer[i].getState());
  }
  for (int i = 0; i < UNIT_SIZE; i++) {
    Particle.variable("on"+String(UNIT_SIZE+i), light[i].getState());
  }
  Particle.function("level", setLevel);
  Particle.function("on", setOn);
  Particle.function("off", setOff);
}

int setLevel(String command) {
  // command == index,level
  char inputStr[20];
  command.toCharArray(inputStr,20);
  char *p = strtok(inputStr,",");
  int index = atoi(p);
  int level = atoi(strtok(NULL,","));
  if (0 <= index && index < UNIT_SIZE) {
    dimmer[index].setLevel(level);
    return 0;
  } else {
    return 1;
  }
}

int setOn(String command) {
  // command == index
  int index = command.toInt();
  if (0 <= index && index < UNIT_SIZE) {
    dimmer[index].setOn();
    return 0;
  } else if (UNIT_SIZE <= index && index < 2 * UNIT_SIZE) {
    light[index-UNIT_SIZE].setOn();
    return 0;
  } else {
    return 1;
  }
}

int setOff(String command) {
  // command == index
  int index = command.toInt();
  if (0 <= index && index < UNIT_SIZE) {
    dimmer[index].setOff();
    return 0;
  } else if (UNIT_SIZE <= index && index < 2 * UNIT_SIZE) {
    light[index-UNIT_SIZE].setOff();
    return 0;
  } else {
    return 1;
  }
}
