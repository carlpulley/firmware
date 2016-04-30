#include "application.h"
#include "../accessory/Light.h"
#include "../accessory/MotionSensor.h"
#include "../accessory/Switch.h"
#include "../util/optional.h"
#include "UnitBase.h"

#pragma once

// Unit pin outs and  data flows
// =============================
// Photon -> PIN[D<N>] -> Relay
// Wall Switch -> SWITCH[A<N>] -> Photon
// PIR Sensor -> MOTION[D<N+4>] -> Photon
class SwitchUnit : protected UnitBase {
  int _id;
  Light _light;
  Switch _switch;
  MotionSensor _motion;

  public:
    SwitchUnit(int N) {
      pin_t PIN = N;
      pin_t SWITCH = FIRST_ANALOG_PIN + N;
      pin_t MOTION = N + 4;
      this->_id = N;
      this->_light = Light(PIN);
      this->_switch = Switch(SWITCH);
      this->_motion = MotionSensor(MOTION, MOTION_PERIOD);
      logHeader("switch unit pinout: PIN="+String(PIN)+"; SWITCH="+String(SWITCH)+"; MOTION="+String(MOTION));
      log("switch/state/"+String(N), this->_light.state == Light::on ? "on" : "off");
    }

    // Following are exposed via REST API

    void setOn() {
      _light.setOn();
      log("switch/state/"+String(_id), "on");
    }

    void setOff() {
      _light.setOff();
      log("switch/state/"+String(_id), "off");
    }

    Light::State getState() {
      return _light.state;
    }

  protected:
    void trigger() {
      if (_light.state == Light::on) {
        setOff();
      } else {
        setOn();
      }
    }

    // Used by loop to poll unit

  public:
    void poll() {
      optional<Switch::State> event = _switch.poll();
      if (event.isDefined()) {
        // `Some` case
        switch (event.get()) {
          case Switch::on:
            setOn();
            break;
          case Switch::off:
            setOff();
            break;
        }
      }
      // TODO: work out a more effective way of using the motion sensor
      optional<MotionSensor::State> sensor = _motion.poll();
      if (sensor.isDefined()) {
        // `Some` case
        if (sensor.get() == MotionSensor::movement) {
          setOn();
        } else {
          setOff();
        }
      }
    }
};
