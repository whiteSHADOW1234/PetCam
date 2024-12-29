#pragma once

#include "driver/gpio.h"
#include "esp_adc/adc_oneshot.h"

// 搖桿控制閾值 (根據實際情況調整)
#define JOYSTICK_THRESHOLD 500

class Joystick
{
public:
  Joystick(adc_unit_t adc_unit, adc_channel_t vrx_channel, adc_channel_t vry_channel, gpio_num_t sw_pin);
  ~Joystick();

  void init();
  void read();
  int getVRxValue() const;
  int getVRyValue() const;
  bool isSWPressed() const;
  void resetSWPressed();

private:
  adc_oneshot_unit_handle_t adc_handle;
  adc_unit_t adc_unit;
  adc_channel_t VRx_channel;
  adc_channel_t VRy_channel;
  gpio_num_t SW_pin;
  int VRx_value;
  int VRy_value;
  bool SW_pressed;

  static void IRAM_ATTR joystick_sw_isr_handler(void *arg);
};