#pragma once

#include "esphome/core/component.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/components/i2c/i2c.h"

namespace esphome {
namespace radsense {

union Uint32
{
    uint32_t u32;
    uint8_t a8[4];
};

union Uint16
{
    uint16_t u16;
    uint8_t a8[2];
};


class RadSenseComponent : public PollingComponent, public i2c::I2CDevice {
 public:
  void setup() override;
  void dump_config() override;
  float get_setup_priority() const override;
  void update() override;

  void set_sensitivity(uint16_t sensitivity);
  void set_dynamic_intensity_sensor(sensor::Sensor *dynamic_intensity_sensor) { dynamic_intensity_sensor_ = dynamic_intensity_sensor; }
  void set_static_intensity_sensor(sensor::Sensor *static_intensity_sensor) { static_intensity_sensor_ = static_intensity_sensor; }
  void set_counts_per_minute_sensor(sensor::Sensor *counts_per_minute_sensor) { counts_per_minute_sensor_ = counts_per_minute_sensor; }
  void set_firmware_version_sensor(sensor::Sensor *firmware_version_sensor) { firmware_version_sensor_ = firmware_version_sensor; }

 protected:
  sensor::Sensor *dynamic_intensity_sensor_{nullptr};
  sensor::Sensor *static_intensity_sensor_{nullptr};
  sensor::Sensor *counts_per_minute_sensor_{nullptr};
  sensor::Sensor *firmware_version_sensor_{nullptr};
  uint32_t last_update = 0;
  uint8_t firmware_version = 0;
  uint16_t sensitivity_ = 0;

  enum ErrorCode {
    NONE = 0,
    COMMUNICATION_FAILED,
    ID_REGISTERS,
  } error_code_;

};

}  // namespace radsense
}  // namespace esphome