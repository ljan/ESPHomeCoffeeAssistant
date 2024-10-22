#pragma once
#include "esphome.h"
#include <TTBOUNCE.h> // Button

enum GrinderState { 
  GRINDER_IDLE, 
  GRINDER_SINGLE, 
  GRINDER_DOUBLE, 
  GRINDER_MANUAL, 
  GRINDER_SAVE_SINGLE, 
  GRINDER_SAVE_DOUBLE 
};

class GrinderComponent : public Component {
 public:
  GrinderComponent(int button_pin, int grinder_pin, unsigned long max_grind_time, int single_grind_time, int double_grind_time)
      : button_pin_(button_pin), grinder_pin_(grinder_pin), max_grind_time_(max_grind_time),
        single_grind_time_(single_grind_time), double_grind_time_(double_grind_time) {}

  void setup() override;
  void loop() override;
  void dump_config() override;

  void set_single_grind_time(int single_grind_time) {
    this->single_grind_time_ = single_grind_time;
  }
  
  void set_double_grind_time(int double_grind_time) {
    this->double_grind_time_ = double_grind_time;
  }

  int get_single_grind_time() const {
    return this->single_grind_time_;
  }
  
  int get_double_grind_time() const {
    return this->double_grind_time_;
  }

 protected:
  static void static_timer_callback(void* p_arg);
  void single_click();
  void double_click();
  void press();
  void timer_callback();

  int single_grind_time_;
  int double_grind_time_;

 private:
  int button_pin_;
  int grinder_pin_;
  unsigned long max_grind_time_;
  int single_grind_time_;
  int double_grind_time_;
  unsigned long grind_start_time_ = 0;
  int grind_duration_ = 0;

  TTBOUNCE button_;
  os_timer_t timer_grinder_;
  GrinderState grinder_state_ = GRINDER_IDLE;

  static const char* TAG;

  // Helper methods for state transitions
  void handle_single_grind();
  void handle_double_grind();
  void handle_manual_grind();
  void handle_save_single();
  void handle_save_double();
  void abort_grind();
};
