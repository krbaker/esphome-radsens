#include "low_power_control.h"

namespace esphome {
namespace radsense {

void LowPowerControl::write_state(bool state) {
  this->publish_state(state);
  this->parent_->set_low_power(state);
}

}  // namespace radsense
}  // namespace esphome