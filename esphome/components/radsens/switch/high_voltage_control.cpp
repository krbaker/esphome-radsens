#include "high_voltage_control.h"

namespace esphome {
namespace radsens {

void HighVoltageControl::write_state(bool state) {
  this->publish_state(state);
  this->parent_->set_high_voltage(state);
}

}  // namespace radsens
}  // namespace esphome