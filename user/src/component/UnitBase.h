#include "application.h"

#pragma once

class UnitBase {
  protected:
    void log(String path, String value) {
      Particle.publish(path, value, PRIVATE);
      Serial.println("[DEBUG] "+path+"="+value);
    }

    void logHeader(String header) {
      String version = "Photon firmware version " + System.version();
      Particle.publish(version, PRIVATE);
      Serial.println("[INFO] "+version);
      Particle.publish(header, PRIVATE);
      Serial.println("[INFO] "+header);
    }
};
