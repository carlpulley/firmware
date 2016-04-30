#include "application.h"
#include "../accessory/DimmerLight.h"
#include "../accessory/DimmerSwitch.h"
#include "../accessory/MotionSensor.h"
#include "../util/optional.h"
#include "UnitBase.h"

#pragma once

#define INCREMENT 100
#define SECOND 1000
#define MOTION_PERIOD 5*SECOND

// Unit pin outs and data flows
// ============================
// Photon -> PIN[D<N>] -> Relay
// Photon -> CONTROL[A<N+4>] -> Dimmer Unit
// Touch Control -> DIMMER[A<N>] -> Photon
// PIR Sensor -> MOTION[D<N+4>] -> Photon
class DimmerUnit : protected UnitBase {
  int _id;
  DimmerSwitch _switch;
  DimmerLight _dimmer;
  bool _increaseDimmer = true;
  MotionSensor _motion;

  public:
    DimmerUnit(int N) {
      pin_t PIN = N;
      pin_t CONTROL = FIRST_ANALOG_PIN + N + 4;
      pin_t DIMMER = FIRST_ANALOG_PIN + N;
      pin_t MOTION = N + 4;
      this->_id = N;
      this->_switch = DimmerSwitch(DIMMER);
      this->_dimmer = DimmerLight(PIN, CONTROL);
      this->_motion = MotionSensor(MOTION, MOTION_PERIOD);
      logHeader("dimmer unit pinout: PIN="+String(PIN)+"; CONTROL="+String(CONTROL)+"; DIMMER="+String(DIMMER)+"; MOTION="+String(MOTION));
      log("dimmer/state/"+String(N), this->_dimmer.state == DimmerLight::on ? "on" : "off");
      log("dimmer/level/"+String(N), String(this->_dimmer.level));
    }

    // Following are exposed via REST API

    void setOn() {
      _dimmer.setOn();
      log("dimmer/state/"+String(_id), "on");
    }

    void setOff() {
      _dimmer.setOff();
      log("dimmer/state/"+String(_id), "off");
    }

    Light::State getState() {
      return _dimmer.state;
    }

    int getLevel() {
      return _dimmer.level;
    }

    void setLevel(int level) {
      _dimmer.setLevel(level);
      log("dimmer/level/"+String(_id), String(level));
    }

  protected:
    void trigger() {
      if (_dimmer.state == DimmerLight::on) {
        setOff();
      } else {
        setOn();
      }
    }

    void reverse() {
      _increaseDimmer = !_increaseDimmer;
    }

    void move() {
      int newLevel = _dimmer.level;
      if (_increaseDimmer) {
        newLevel += INCREMENT;
      } else {
        newLevel -= INCREMENT;
      }
      setLevel(newLevel);
    }

    // Used by loop to poll unit

  public:
    void poll() {
      optional<DimmerSwitch::State> event = _switch.poll();
      if (event.isDefined()) {
        // `Some` case
        switch (event.get()) {
          case DimmerSwitch::tap:
            log("dimmer/switch/"+String(_id), "tap");
            trigger();
            break;
          case DimmerSwitch::tap_tap:
            log("dimmer/switch/"+String(_id), "double tap");
            reverse();
            break;
          case DimmerSwitch::hold:
            log("dimmer/switch/"+String(_id), "hold");
            move();
            break;
          case DimmerSwitch::silent:
            log("dimmer/switch/"+String(_id), "silent");
            break;
        }
      }
      optional<MotionSensor::State> sensor = _motion.poll();
      if (sensor.isDefined()) {
        // `Some` case
        switch (sensor.get()) {
          case MotionSensor::movement:
            log("dimmer/pir/"+String(_id), "movement");
            setOn();
            break;
          case MotionSensor::still:
            log("dimmer/pir/"+String(_id), "still");
            setOff();
            break;
        }
      }
    }
};
