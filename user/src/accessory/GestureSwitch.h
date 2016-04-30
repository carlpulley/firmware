#include "application.h"
#include "../util/optional.h"

#pragma once

class GestureSwitch {
  pin_t pin;
  int i2c;

  public:
    enum State { nothing, left, right, up, down, near, far, all };

    State state;

    GestureSwitch() {}

    // FIXME: communication is via I2C!!!
    GestureSwitch(pin_t pin) {
      Serial.println("[INFO] configuring gesture switch instance: gesture sensor on pin " + String(pin));
      this->pin = pin;
      this->state = (State)(EEPROM.read(pin));
      pinMode(this->pin, INPUT);
      Wire.begin();
    }

    optional<State> poll() {
      Wire.requestFrom(i2c, 1);
      if (wire.availablee()) {
        char observation = Wire.read();
        switch(observation) {
          default:
            setState(nothing);
            return optional<State>(state);
        }
      }
      return optional<State>();
    }

  private:
    void setState(State value) {
      EEPROM.write(pin, value);
      state = value;
    }
};
