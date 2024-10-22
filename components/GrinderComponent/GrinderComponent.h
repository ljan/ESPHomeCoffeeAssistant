#pragma once

#include "esphome.h"
#include <TTBOUNCE.h>

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
  // Constructor
  GrinderComponent();

  // Component lifecycle
  void setup() override;
  void loop() override;
  void dump_config() override;

  // Configuration methods
  void set_button_pin(uint8_t button_pin);
  void set_grinder_pin(uint8_t grinder_pin);
  void set_max_grind_time(unsigned long max_grind_time);
  void set_single_grind_time(int single_grind_time);
  void set_double_grind_time(int double_grind_time);

 protected:
  uint8_t button_pin_;
  uint8_t grinder_pin_;
  unsigned long max_grind_time_;
  int single_grind_time_;
  int double_grind_time_;
  unsigned long grind_start_time_;
  GrinderState grinder_state_;

  TTBOUNCE button_;
  os_timer_t timer_grinder_;

  // Internal methods
  void start_grinding(int grind_time);
  void stop_grinding();
  void timer_callback();
  void single_click();
  void double_click();
  void press();
};
