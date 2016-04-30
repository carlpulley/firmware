#include "application.h"
#include "Light.h"

#pragma once

class DimmerLight : public Light {
  pin_t control;
  
  public:
    int level;

    DimmerLight() {}

    DimmerLight(pin_t pin, pin_t control) : Light(pin) {
      Serial.println("[INFO] configuring dimmer light instance: dimmer control on pin " + String(control)+" and switch sensor on pin " + String(pin));
      this->control = control;
      this->level = (State)(EEPROM.read(control));
      pinMode(this->control, OUTPUT);
    }
    
    void setOn() {
      analogWrite(control, level);
      Light::setOn();
    }
    
    void setOff() {
      analogWrite(control, level);
      Light::setOff();
    }
    
    void setLevel(int newLevel) {
      if (0 <= newLevel && newLevel <= 100) {
        EEPROM.write(control, newLevel);
        level = newLevel;
        analogWrite(control, level);
      } else {
        Serial.println("[ERROR] level should be >= 0 and <= 100");
      }
    }
};
