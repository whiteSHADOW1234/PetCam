#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "ULN2003.h"
#include "joystick.h"
#include "wifi.h"

// 定義步進電機引腳
#define IN1 GPIO_NUM_15
#define IN2 GPIO_NUM_16
#define IN3 GPIO_NUM_17
#define IN4 GPIO_NUM_18

// 建立 ULN2003 物件，使用全步進模式
ULN2003 stepper(ULN2003::FULL_STEP, IN1, IN2, IN3, IN4);
// 建立 Joystick 物件,
// ADC1_CHANNEL_0 對應 GPIO1, ADC1_CHANNEL_1 對應 GPIO2, 因板子而異
Joystick joystick(ADC_UNIT_1, ADC_CHANNEL_0, ADC_CHANNEL_1, GPIO_NUM_4);
// 標籤用於日誌輸出
static const char *TAG = "main";

// 馬達控制函數
void rotate_clockwise() {
    ESP_LOGI(TAG, "順時針旋轉");
    stepper.rotate(15); // 旋轉角度，根據實際情況調整
}

void rotate_counterclockwise() {
    ESP_LOGI(TAG, "逆時針旋轉");
    stepper.rotate(-15); // 旋轉角度，根據實際情況調整
}

extern "C" void app_main(void)
{
  // 初始化 Wi-Fi
  wifi_init_sta();

  // 初始化步進電機
  stepper.init();

  // 設定速度 (延遲時間 5ms)
  stepper.setSpeed(5);

  // 初始化搖桿
  joystick.init();

  while (1)
  {
    // 讀取搖桿數值
    joystick.read();

    // ESP_LOGI(TAG, "VRx: %d, VRy: %d, SW: %s", joystick.getVRxValue(), joystick.getVRyValue(), joystick.isSWPressed() ? "Pressed" : "Released");

    // 根據搖桿值控制步進電機
    if (joystick.getVRxValue() > (4095 - JOYSTICK_THRESHOLD))
    {
      ESP_LOGI(TAG, "順時針旋轉");
      stepper.rotate(5); // 旋轉小角度，根據實際情況調整
    }
    else if (joystick.getVRxValue() < JOYSTICK_THRESHOLD)
    {
      ESP_LOGI(TAG, "逆時針旋轉");
      stepper.rotate(-5); // 旋轉小角度，根據實際情況調整
    }

    // 處理搖桿按鈕按下事件
    if (joystick.isSWPressed())
    {
      ESP_LOGI(TAG, "搖桿按鈕按下");

      joystick.resetSWPressed(); // 重置按鈕狀態否則會一直觸發
    }

    vTaskDelay(pdMS_TO_TICKS(50)); // 調整延遲時間，根據實際情況調整
  }
}