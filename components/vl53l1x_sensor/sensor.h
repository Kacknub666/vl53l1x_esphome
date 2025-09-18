#pragma once

#include "esphome.h"
#include <Wire.h>
#include <VL53L1X.h>

namespace esphome {
namespace vl53l1x_sensor {

struct VL53L1XSensorSettings {
  bool long_range = true;
  uint16_t timing_budget = 200; // ms
};

class VL53L1XSensor : public PollingComponent, public sensor::Sensor {
 public:
  VL53L1XSensor(const VL53L1XSensorSettings &settings = VL53L1XSensorSettings(),
                uint32_t update_interval = 1000)
      : PollingComponent(update_interval), settings_(settings) {}

  void setup() override {
    Wire.begin();
    if (!vl53_.init()) {
      ESP_LOGE("vl53l1x", "Failed to initialize VL53L1X!");
      this->mark_failed();
      return;
    }
    vl53_.setDistanceMode(settings_.long_range ? VL53L1X::Long : VL53L1X::Short);
    vl53_.setMeasurementTimingBudget(settings_.timing_budget * 1000);
    vl53_.startContinuous(50);
    ESP_LOGI("vl53l1x", "VL53L1X initialized");
  }

  void update() override {
    if (this->is_failed()) return;
    uint16_t distance = vl53_.read();
    if (vl53_.timeoutOccurred()) {
      ESP_LOGW("vl53l1x", "Timeout occurred!");
      return;
    }
    publish_state(distance);
  }

 protected:
  VL53L1X vl53_;
  VL53L1XSensorSettings settings_;
};

}  // namespace vl53l1x_sensor
}  // namespace esphome
