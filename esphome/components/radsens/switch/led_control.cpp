#include "led_control.h"

namespace esphome {
namespace radsens {

void LedControl::setup() {
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
    this->publish_state(this->parent_->get_led());
  }
}

void LedControl::write_state(bool state) {
  this->parent_->set_led(state);
  this->publish_state(state);
}

}  // namespace radsens
}  // namespace esphome