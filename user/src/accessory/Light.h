#include "application.h"

#pragma once

class Light {
  pin_t pin;

  public:
    enum State { off, on };

    State state;

    Light() {}
  
    Light(pin_t pin) {
      Serial.println("[INFO] configuring light instance: switch sensor on pin " + String(pin));
      this->pin = pin;
      this->state = (State)(EEPROM.read(pin));
      pinMode(this->pin, OUTPUT);
      digitalWrite(this->pin, LOW);
    }

    void setOn() {
      setState(on);
      digitalWrite(pin, HIGH);
    }
    
    void setOff() {
      setState(off);
      digitalWrite(pin, LOW);
    }

  private:
    void setState(State value) {
      EEPROM.write(pin, value);
      state = value;
   }
};
