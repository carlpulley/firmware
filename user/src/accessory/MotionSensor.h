#include "application.h"
#include <functional>
#include "../util/optional.h"

#pragma once

class MotionSensor {
  pin_t pin;
  optional<Timer> active;

  public:
    enum State { still, movement };

    State state;

    MotionSensor() {}

    MotionSensor(pin_t pin, unsigned sensitivity) {
      Serial.println("[INFO] configuring motion sensor instance: motion sensed on pin " + String(pin)+"; sensitivity is " + String(sensitivity));
      this->pin = pin;
      std::function<void(void)> callback = std::bind(&MotionSensor::stopTimer, this);
      this->active = optional<Timer>(Timer(sensitivity, callback));
      this->state = (State)(EEPROM.read(pin));
      pinMode(this->pin, INPUT);
    }

    optional<State> poll() {
      int observation = digitalRead(pin);
      Serial.println("[INFO] observation=" + String(observation));
      if (active.isDefined() && ! active.get().isActive()) {
        if (observation == movement) {
          // Movement detected
          setState(movement);
          startTimer();
        } else if (state == movement && observation == still) {
          // Movement stopped
          setState(still);
        }
      }
      return optional<State>(state);
    }

  private:
    void startTimer() {
      if (active.isDefined() && ! active.get().isActive()) {
        active.get().start();
      }
    }

    void stopTimer() {
      if (active.isDefined() && active.get().isActive()) {
        active.get().stop();
      }
    }

    void setState(State value) {
      EEPROM.write(pin, value);
      state = value;
   }
};
