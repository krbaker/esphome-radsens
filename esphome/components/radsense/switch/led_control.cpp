#include "led_control.h"

namespace esphome {
namespace radsense {

void LedControl::write_state(bool state) {
  this->publish_state(state);
  this->parent_->set_led(state);
}

}  // namespace radsense
}  // namespace esphome