#include "joystick.h"
#include "esp_log.h"

static const char *TAG = "joystick";

Joystick::Joystick(adc_unit_t adc_unit, adc_channel_t vrx_channel, adc_channel_t vry_channel, gpio_num_t sw_pin)
    : adc_unit(adc_unit), VRx_channel(vrx_channel), VRy_channel(vry_channel), SW_pin(sw_pin), VRx_value(0), VRy_value(0), SW_pressed(false)
{
  // Initialize ADC unit
  adc_oneshot_unit_init_cfg_t init_config = {
      .unit_id = adc_unit,
  };
  ESP_ERROR_CHECK(adc_oneshot_new_unit(&init_config, &adc_handle));
}

Joystick::~Joystick()
{
  // 解除安裝 GPIO 中斷服務
  gpio_isr_handler_remove(SW_pin);
  // 刪除 ADC unit
  ESP_ERROR_CHECK(adc_oneshot_del_unit(adc_handle));
}

void Joystick::init()
{
  // 設定 ADC 通道
  adc_oneshot_chan_cfg_t config = {
      .atten = ADC_ATTEN_DB_12,
      .bitwidth = ADC_BITWIDTH_DEFAULT,
  };
  ESP_ERROR_CHECK(adc_oneshot_config_channel(adc_handle, VRx_channel, &config));
  ESP_ERROR_CHECK(adc_oneshot_config_channel(adc_handle, VRy_channel, &config));

  // 初始化搖桿按鈕 GPIO
  gpio_config_t io_conf = {};
  io_conf.intr_type = GPIO_INTR_NEGEDGE;
  io_conf.mode = GPIO_MODE_INPUT;
  io_conf.pin_bit_mask = (1ULL << SW_pin);
  io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
  io_conf.pull_up_en = GPIO_PULLUP_ENABLE;
  gpio_config(&io_conf);

  // 安裝 GPIO 中斷服務 (只安裝一次)
  static bool isr_service_installed = false;
  if (!isr_service_installed)
  {
    gpio_install_isr_service(0);
    isr_service_installed = true;
  }

  // 掛載搖桿按鈕中斷服務程序
  gpio_isr_handler_add(SW_pin, joystick_sw_isr_handler, (void *)this); // 傳入 this 指標
}

void Joystick::read()
{
  // 讀取搖桿 X 軸和 Y 軸的值
  ESP_ERROR_CHECK(adc_oneshot_read(adc_handle, VRx_channel, &VRx_value));
  ESP_ERROR_CHECK(adc_oneshot_read(adc_handle, VRy_channel, &VRy_value));
}

int Joystick::getVRxValue() const
{
  return VRx_value;
}

int Joystick::getVRyValue() const
{
  return VRy_value;
}

bool Joystick::isSWPressed() const
{
  return SW_pressed;
}

void IRAM_ATTR Joystick::joystick_sw_isr_handler(void *arg)
{
  Joystick *joystick = (Joystick *)arg; // 取得 Joystick 物件指標
  joystick->SW_pressed = true;          // 設定按鈕狀態
}

void Joystick::resetSWPressed()
{
  SW_pressed = false;
  ESP_LOGI(TAG, "搖桿按鈕已重置");
}