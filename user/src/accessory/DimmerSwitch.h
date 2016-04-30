#include "application.h"
#include "../util/optional.h"

#pragma once

// Classification macros
#define SILENCE_LOWER 0
#define SILENCE_UPPER 10
#define TAP_LOWER 10
#define TAP_UPPER 20
#define TAP_TAP_LOWER 20
#define TAP_TAP_UPPER 30
#define HOLD_LOWER 30
#define HOLD_UPPER 40
// Baysian weighting
#define SAMPLE_SIZE 10
const double WEIGHTS[SAMPLE_SIZE] = {0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1};

class DimmerSwitch {
  pin_t pin;
  int window[SAMPLE_SIZE];
  int ptr = 0;

  public:
    enum State { silent, tap, tap_tap, hold };

    State state;

    DimmerSwitch() {}

    DimmerSwitch(pin_t pin) {
      Serial.println("[INFO] configuring dimmer switch instance: switch sensor on pin " + String(pin));
      this->pin = pin;
      this->state = (State)(EEPROM.read(pin));
      pinMode(this->pin, INPUT);
      clearWindow();
    }

    optional<State> poll() {
      window[ptr] = digitalRead(pin);
      ptr = (ptr + 1) % SAMPLE_SIZE;
      double classification = 0;
      for (int index = 0; index < SAMPLE_SIZE; index++) {
        classification += WEIGHTS[index] * window[(ptr + index) % SAMPLE_SIZE];
      }
      if (SILENCE_LOWER <= classification && classification < SILENCE_UPPER) {
        setState(silent);
      } else if (TAP_LOWER <= classification && classification < TAP_UPPER) {
        setState(tap);
      } else if (TAP_TAP_LOWER <= classification && classification < TAP_TAP_UPPER) {
        setState(tap_tap);
      } else if (HOLD_LOWER <= classification && classification < HOLD_UPPER) {
        setState(hold);
      } else {
        return optional<State>();;
      }
      return optional<State>(state);;
    }

  private:
    void setState(State value) {
      EEPROM.write(pin, value);
      state = value;
    }

    void clearWindow() {
      for (int index = 0; index < SAMPLE_SIZE; index++) {
        window[index] = 0;
      }
    }
};
