#include "grinder.h"

const char* GrinderComponent::TAG = "GrinderComponent";

void GrinderComponent::setup() {
  pinMode(this->grinder_pin_, OUTPUT);
  digitalWrite(this->grinder_pin_, LOW);
  pinMode(this->button_pin_, INPUT_PULLUP);

  os_timer_setfn(&this->timer_grinder_, static_timer_callback, this);

  this->button_ = TTBOUNCE(this->button_pin_);
  this->button_.set_active_low();
  this->button_.enable_pullup();
  this->button_.set_debounce_interval(TIME_DEBOUNCE);
  this->button_.set_press_interval(TIME_PRESS);

  this->button_.attach_click([this]() { this->single_click(); });
  this->button_.attach_double_click([this]() { this->double_click(); });
  this->button_.attach_press([this]() { this->press(); });
}

void GrinderComponent::loop() {
  this->button_.update();

  // Handle the state machine transitions
  switch (this->grinder_state_) {
    case GRINDER_SINGLE:
      this->handle_single_grind();
      break;
    case GRINDER_DOUBLE:
      this->handle_double_grind();
      break;
    case GRINDER_MANUAL:
      this->handle_manual_grind();
      break;
    case GRINDER_SAVE_SINGLE:
      this->handle_save_single();
      break;
    case GRINDER_SAVE_DOUBLE:
      this->handle_save_double();
      break;
    default:
      break;
  }
}

void GrinderComponent::dump_config() {
  ESP_LOGCONFIG(TAG, "GrinderComponent:");
  ESP_LOGCONFIG(TAG, "  Button Pin: %d", this->button_pin_);
  ESP_LOGCONFIG(TAG, "  Grinder Pin: %d", this->grinder_pin_);
  ESP_LOGCONFIG(TAG, "  Max Grind Time: %lu", this->max_grind_time_);
  ESP_LOGCONFIG(TAG, "  Single Grind Time: %d", this->single_grind_time_);
  ESP_LOGCONFIG(TAG, "  Double Grind Time: %d", this->double_grind_time_);
}

void GrinderComponent::static_timer_callback(void* p_arg) {
  static_cast<GrinderComponent*>(p_arg)->timer_callback();
}

void GrinderComponent::timer_callback() {
  ESP_LOGD(TAG, "Timer fired, stopping grinder.");
  digitalWrite(this->grinder_pin_, LOW);
  this->grind_duration_ = millis() - this->grind_start_time_;
  this->grinder_state_ = GRINDER_IDLE;
  os_timer_disarm(&this->timer_grinder_);
}

void GrinderComponent::single_click() {
  ESP_LOGD(TAG, "Single click detected.");
  if (this->grinder_state_ == GRINDER_IDLE) {
    ESP_LOGD(TAG, "Start single grind.");
    this->grinder_state_ = GRINDER_SINGLE;
    digitalWrite(this->grinder_pin_, HIGH);
    this->grind_start_time_ = millis();
    os_timer_arm(&this->timer_grinder_, this->single_grind_time_, false);
  } else {
    this->abort_grind();
  }
}

void GrinderComponent::double_click() {
  ESP_LOGD(TAG, "Double click detected.");
  if (this->grinder_state_ == GRINDER_IDLE) {
    ESP_LOGD(TAG, "Start double grind.");
    this->grinder_state_ = GRINDER_DOUBLE;
    digitalWrite(this->grinder_pin_, HIGH);
    this->grind_start_time_ = millis();
    os_timer_arm(&this->timer_grinder_, this->double_grind_time_, false);
  } else {
    this->abort_grind();
  }
}

void GrinderComponent::press() {
  ESP_LOGD(TAG, "Press detected.");
  if (this->grinder_state_ == GRINDER_IDLE) {
    ESP_LOGD(TAG, "Start manual grind.");
    this->grinder_state_ = GRINDER_MANUAL;
    digitalWrite(this->grinder_pin_, HIGH);
    this->grind_start_time_ = millis();
    os_timer_arm(&this->timer_grinder_, this->max_grind_time_, false);
  } else {
    os_timer_disarm(&this->timer_grinder_);
    os_timer_arm(&this->timer_grinder_, this->max_grind_time_ - (millis() - this->grind_start_time_), false);
  }
}

void GrinderComponent::abort_grind() {
  ESP_LOGD(TAG, "Grind aborted.");
  digitalWrite(this->grinder_pin_, LOW);
  os_timer_disarm(&this->timer_grinder_);
  this->grinder_state_ = GRINDER_IDLE;
}

void GrinderComponent::handle_single_grind() {
  ESP_LOGD(TAG, "Handling single grind.");
  if (this->button_.read() == LOW) {
    this->abort_grind();
  }
}

void GrinderComponent::handle_double_grind() {
  ESP_LOGD(TAG, "Handling double grind.");
  if (this->button_.read() == LOW) {
    this->abort_grind();
  }
}

void GrinderComponent::handle_manual_grind() {
  ESP_LOGD(TAG, "Handling manual grind.");
  if (this->button_.read() == LOW) {
    ESP_LOGD(TAG, "Manual grind stopped.");
    this->abort_grind();
  }
}

void GrinderComponent::set_single_grind_time(int single_grind_time) {
  this->single_grind_time_ = single_grind_time;
}

void GrinderComponent::set_double_grind_time(int double_grind_time) {
  this->double_grind_time_ = double_grind_time;
}

void GrinderComponent::handle_save_single() {
  ESP_LOGD(TAG, "Handling save single grind.");
  if (this->button_.read() == LOW) {
    this->abort_grind();
    this->single_grind_time_ = this->grind_duration_;
    ESP_LOGD(TAG, "Single grind time updated to %d ms", this->single_grind_time_);
  }
}

void GrinderComponent::handle_save_double() {
  ESP_LOGD(TAG, "Handling save double grind.");
  if (this->button_.read() == LOW) {
    this->abort_grind();
    this->double_grind_time_ = this->grind_duration_;
    ESP_LOGD(TAG, "Double grind time updated to %d ms", this->double_grind_time_);
  }
}
