#include "application.h"

#pragma once

class Switch {
  pin_t pin;

  public:
    enum State { off, on };

    State state;

    Switch() {}

    Switch(pin_t pin) {
      Serial.println("[INFO] configuring switch instance: switch sensor on pin " + String(pin));
      this->pin = pin;
      this->state = (State)(EEPROM.read(pin));
      pinMode(this->pin, INPUT);
    }

    optional<State> poll() {
      int observation = digitalRead(pin);
      if (observation == 1) {  
        setState(on);
        return optional<State>(state);;
      } else if (observation == 0) {
        setState(off);
        return optional<State>(state);;
      } else {
        return optional<State>();;
      }
    }

  private:
    void setState(State value) {
      EEPROM.write(pin, value);
      state = value;
   }
};
