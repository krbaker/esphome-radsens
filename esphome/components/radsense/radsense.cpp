#include "radsense.h"
#include "esphome/core/log.h"
#include "esphome/core/application.h"
#include "esphome/core/datatypes.h"
#include "esphome/core/helpers.h"
namespace esphome {
namespace radsense {

static const char *const TAG = "radsense";
static const uint8_t RADSENSE_DEVICE_ID = 0x7D;
static const uint8_t RADSENSE_ADDRESS = 0x66;
static const uint8_t RADSENSE_REGISTER_IDENTIFICATION = 0x00; // 8 bit
static const uint8_t RADSENSE_REGISTER_IDENTIFICATION_FIRMWARE_VERSION = 0x01; // 8bit
static const uint8_t RADSENSE_REGISTER_DATA_DYNAMIC_INTENSITY = 0x03; // 24bit
static const uint8_t RADSENSE_REGISTER_DATA_STATIC_INTENSITY = 0x06; // 24bit
static const uint8_t RADSENSE_REGISTER_DATA_PULSE_COUNTER= 0x09; // 16bit
static const uint8_t RADSENSE_REGISTER_CONTROL_DEVICE_ADDRESS = 0x10; // 8bit
static const uint8_t RADSENSE_REGISTER_CONTROL_HIGH_VOLTAGE_GENERATOR = 0x11; // 8bit
static const uint8_t RADSENSE_REGISTER_CONTROL_SENSITIVITY = 0x12; // 16bit
static const uint8_t RADSENSE_REGISTER_CONTROL_LED = 0x14;  // 8bit
static const uint8_t RADSENSE_REGISTER_CONTROL_LOW_POWER_MODE = 0x0C; ///8bit

void RadSenseComponent::set_control(uint8_t reg, uint8_t val){
  if (!this->write_byte(reg, val)){
    ESP_LOGCONFIG(TAG, "RadSense Write: failed writing control register %u", reg);
    this->error_code_ = COMMUNICATION_FAILED;
    this->mark_failed();
    return;    
  }

}

void RadSenseComponent::set_high_voltage(bool enable){
  set_control(RADSENSE_REGISTER_CONTROL_HIGH_VOLTAGE_GENERATOR, enable);
}

void RadSenseComponent::set_led(bool enable){
  set_control(RADSENSE_REGISTER_CONTROL_LED, enable);
}

void RadSenseComponent::set_low_power(bool enable){
  set_control(RADSENSE_REGISTER_CONTROL_LOW_POWER_MODE, enable);
}

void RadSenseComponent::setup() {
  ESP_LOGCONFIG(TAG, "RadSense Setup: starting...");
  uint8_t id;
  if (!this->read_byte(RADSENSE_REGISTER_IDENTIFICATION, &id)) {
    ESP_LOGCONFIG(TAG, "RadSense Setup: failed reading id");
    this->error_code_ = COMMUNICATION_FAILED;
    this->mark_failed();
    return;
  }

  if (id != RADSENSE_DEVICE_ID) {
    ESP_LOGCONFIG(TAG, "RadSense Setup: id wrong %u != %u", id, RADSENSE_DEVICE_ID);
    this->error_code_ = ID_REGISTERS;
    this->mark_failed();
    return;
  }

  if (!this->read_byte(RADSENSE_REGISTER_IDENTIFICATION_FIRMWARE_VERSION, &this->firmware_version)) {
    ESP_LOGCONFIG(TAG, "RadSense Setup: failed reading firmware version");
    this->error_code_ = COMMUNICATION_FAILED;
    this->mark_failed();
    return;
  }

  // The docs say this should be a 16 bit value defaulting to 105
  // however I read mine before writing to it and I got the wrong
  // byte order (it was 105, but backwards compared to the pulse
  // counter).  Referencing
  // https://github.com/climateguard/RadSens/blob/master/src/CG_RadSens.cpp
  // it does reverse count, but not sensitivity
  // Read in byte order from device (expecting little endian)
  Uint16 old_sensitivity;
  if (!this->read_bytes(RADSENSE_REGISTER_CONTROL_SENSITIVITY, old_sensitivity.a8, 2)) {
    ESP_LOGCONFIG(TAG, "RadSense Setup: failed reading sensitivity");
    this->error_code_ = COMMUNICATION_FAILED;
    this->mark_failed();
    return;
  }
  // This number is little endian oddly, convert if needed
  old_sensitivity.u16 = convert_little_endian(old_sensitivity.u16); // No-Op on esp32
  ESP_LOGCONFIG(TAG, "RadSense setup: sensitivity is %u", old_sensitivity.u16);
  if (this->sensitivity_ != 0 && old_sensitivity.u16 != this->sensitivity_){
    ESP_LOGCONFIG(TAG, "RadSense setup: sensitivity setting to %u", this->sensitivity_);
    Uint16 sensitivity_to_send;
    sensitivity_to_send.u16 = convert_little_endian(this->sensitivity_);
    ESP_LOGCONFIG(TAG, "RadSense setup: writing %u %u", sensitivity_to_send.a8[0], sensitivity_to_send.a8[1]);
    if (!this->write_bytes(RADSENSE_REGISTER_CONTROL_SENSITIVITY, sensitivity_to_send.a8, 2)) {
      ESP_LOGCONFIG(TAG, "RadSense Setup: failed writing sensitivity");
      this->error_code_ = COMMUNICATION_FAILED;
      this->mark_failed();
      return;
    }
  }

  ESP_LOGCONFIG(TAG, "RadSense setup: Complete (firmware version %u)!", this->firmware_version);
}

void RadSenseComponent::dump_config() {
  ESP_LOGCONFIG(TAG, "RadSense:");
  LOG_I2C_DEVICE(this);
  if (this->error_code_ == COMMUNICATION_FAILED) {
    ESP_LOGE(TAG, "Communication with RadSense failed!");
  } else if (this->error_code_ == ID_REGISTERS) {
    ESP_LOGE(TAG, "The ID registers don't match - Is this really a RadSense?");
  }
  LOG_UPDATE_INTERVAL(this);

  LOG_SENSOR("  ", "Dynamic Intensity", this->dynamic_intensity_sensor_);
  LOG_SENSOR("  ", "Static Intensity", this->static_intensity_sensor_);
  LOG_SENSOR("  ", "Counts Per Minute", this->counts_per_minute_sensor_);
}

float RadSenseComponent::get_setup_priority() const { return setup_priority::DATA; }
void RadSenseComponent::set_sensitivity(uint16_t sensitivity) { this->sensitivity_ = sensitivity; }

void RadSenseComponent::update() {
  // must be zero'd as we write to the last 3 bytes
  // Union allows us to write as bytes then read / write as u32
  Uint32 raw_dynamic_intensity{}, raw_static_intensity{}; 
  uint16_t raw_counts;
  // NOTES: 
  // The manual says reading the rawcount resets it.
  // However it appears reading *any* I2C value resets the raw count
  // This reads the raw count first to make sure we don't lose that data 
  // I think we can still lose some between the count read and the intensity reads
  // but we do them as quickly as we can to minimize data loss 
  // TODO: see if we can keep the bus open?
  // The intensity values are 24bit uints that is why this writes 1 byte into a 32bit uint
  // we also have to fix endien
  uint32_t this_update = millis();
  if (!this->read_byte_16(RADSENSE_REGISTER_DATA_PULSE_COUNTER, &raw_counts) ||
      !this->read_bytes(RADSENSE_REGISTER_DATA_DYNAMIC_INTENSITY, &raw_dynamic_intensity.a8[1], 3) ||
      !this->read_bytes(RADSENSE_REGISTER_DATA_STATIC_INTENSITY, &raw_static_intensity.a8[1], 3)){
    this->status_set_warning();
    return;
  }

  raw_dynamic_intensity.u32 = convert_big_endian(raw_dynamic_intensity.u32);
  raw_static_intensity.u32 = convert_big_endian(raw_static_intensity.u32);

  ESP_LOGD(TAG, "Got dynamic=%.1f static=%.1f counts=%d", 
                raw_dynamic_intensity.u32 * 0.1,
                raw_static_intensity.u32 * 0.1,
                raw_counts);

  if (this->last_update != 0 && (this->counts_per_minute_sensor_ != nullptr)){
    float scale_to_minutes = ((millis() - this->last_update) / 60000.0);
    this->counts_per_minute_sensor_->publish_state(raw_counts / scale_to_minutes);
  }
  this->last_update = millis();

  if (this->dynamic_intensity_sensor_ != nullptr)
    this->dynamic_intensity_sensor_->publish_state(raw_dynamic_intensity.u32 * 0.1);
  if (this->static_intensity_sensor_ != nullptr)
    this->static_intensity_sensor_->publish_state(raw_static_intensity.u32 * 0.1);

  if (this->firmware_version_sensor_ != nullptr)
    this->firmware_version_sensor_->publish_state(this->firmware_version);

}

}  // namespace radsense
}  // namespace esphome