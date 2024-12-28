#include "low_power_control.h"

namespace esphome {
namespace radsens {

void LowPowerControl::write_state(bool state) {
  this->publish_state(state);
  this->parent_->set_low_power(state);
}

}  // namespace radsens
}  // namespace esphome