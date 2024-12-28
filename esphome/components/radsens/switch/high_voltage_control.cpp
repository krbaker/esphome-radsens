#include "high_voltage_control.h"

namespace esphome {
namespace radsens {

void HighVoltageControl::setup() {
  optional<bool> initial_state = Switch::get_initial_state_with_restore_mode();
  if (initial_state.has_value()) {
    // if it has a value, restore_mode is not "DISABLED", therefore act on the switch:
    if (initial_state.value()) {
      this->turn_on();
    } else {
      this->turn_off();
    }
  }
  else {
    this->publish_state(this->parent_->get_high_voltage());
  }
}

void HighVoltageControl::write_state(bool state) {
  this->parent_->set_high_voltage(state);
  this->publish_state(state);
}

}  // namespace radsens
}  // namespace esphome