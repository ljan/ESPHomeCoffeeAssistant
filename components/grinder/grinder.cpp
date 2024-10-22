#include "grinder.h"
#include "esphome/core/log.h"

namespace esphome {
namespace grinder {

// Constructor
GrinderComponent::GrinderComponent() : grinder_state_(GRINDER_IDLE), grind_start_time_(0) {}

// Setup method
void GrinderComponent::setup() {
  ESP_LOGCONFIG(TAG, "Setting up GrinderComponent...");
  
  pinMode(this->grinder_pin_, OUTPUT);
  digitalWrite(this->grinder_pin_, LOW);
  
  this->button_.attach(this->button_pin_);
  this->button_.setActiveLow();
  this->button_.enablePullup();
  this->button_.setDebounceInterval(50);  // debounce time
  this->button_.setPressInterval(300);  // press detection time
  this->button_.attachClick([this]() { this->single_click(); });
  this->button_.attachDoubleClick([this]() { this->double_click(); });
  this->button_.attachPress([this]() { this->press(); });

  os_timer_setfn(&this->timer_grinder_, [](void *pArg) { ((GrinderComponent*)pArg)->timer_callback(); }, this);
}

// Loop method
void GrinderComponent::loop() {
  this->button_.update();
  
  if (this->grinder_state_ != GRINDER_IDLE) {
    // Handle the running state
  }
}

// Dump config to log
void GrinderComponent::dump_config() {
  ESP_LOGCONFIG(TAG, "GrinderComponent:");
  ESP_LOGCONFIG(TAG, "  Button Pin: %u", this->button_pin_);
  ESP_LOGCONFIG(TAG, "  Grinder Pin: %u", this->grinder_pin_);
  ESP_LOGCONFIG(TAG, "  Single Grind Time: %d ms", this->single_grind_time_);
  ESP_LOGCONFIG(TAG, "  Double Grind Time: %d ms", this->double_grind_time_);
  ESP_LOGCONFIG(TAG, "  Max Grind Time: %lu ms", this->max_grind_time_);
}

// Setters
void GrinderComponent::set_button_pin(uint8_t button_pin) {
  this->button_pin_ = button_pin;
}

void GrinderComponent::set_grinder_pin(uint8_t grinder_pin) {
  this->grinder_pin_ = grinder_pin;
}

void GrinderComponent::set_max_grind_time(unsigned long max_grind_time) {
  this->max_grind_time_ = max_grind_time;
}

void GrinderComponent::set_single_grind_time(int single_grind_time) {
  this->single_grind_time_ = single_grind_time;
}

void GrinderComponent::set_double_grind_time(int double_grind_time) {
  this->double_grind_time_ = double_grind_time;
}

// Button Actions
void GrinderComponent::single_click() {
  if (this->grinder_state_ == GRINDER_IDLE) {
    ESP_LOGD(TAG, "Single Click detected, starting single grind.");
    this->start_grinding(this->single_grind_time_);
    this->grinder_state_ = GRINDER_SINGLE;
  } else {
    this->stop_grinding();
  }
}

void GrinderComponent::double_click() {
  if (this->grinder_state_ == GRINDER_IDLE) {
    ESP_LOGD(TAG, "Double Click detected, starting double grind.");
    this->start_grinding(this->double_grind_time_);
    this->grinder_state_ = GRINDER_DOUBLE;
  } else {
    this->stop_grinding();
  }
}

void GrinderComponent::press() {
  if (this->grinder_state_ == GRINDER_IDLE) {
    ESP_LOGD(TAG, "Press detected, starting manual grind.");
    this->start_grinding(this->max_grind_time_);
    this->grinder_state_ = GRINDER_MANUAL;
  }
}

// Start grinding
void GrinderComponent::start_grinding(int grind_time) {
  digitalWrite(this->grinder_pin_, HIGH);
  this->grind_start_time_ = millis();
  os_timer_arm(&this->timer_grinder_, grind_time, false);
}

// Stop grinding
void GrinderComponent::stop_grinding() {
  digitalWrite(this->grinder_pin_, LOW);
  os_timer_disarm(&this->timer_grinder_);
  this->grinder_state_ = GRINDER_IDLE;
}

// Timer Callback
void GrinderComponent::timer_callback() {
  ESP_LOGD(TAG, "Timer has expired, stopping grinder.");
  this->stop_grinding();
}

}  // namespace grinder
}  // namespace esphome
