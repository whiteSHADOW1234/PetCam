#include "ULN2003.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <cmath>
#include "freertos/FreeRTOSConfig.h"
#include "freertos/portmacro.h"
#include "esp_log.h"
#include "sdkconfig.h"
#include "driver/gpio.h"

const int ULN2003::motorSequenceHalf[8][4] = {
  {1, 0, 0, 0},
  {1, 1, 0, 0},
  {0, 1, 0, 0},
  {0, 1, 1, 0},
  {0, 0, 1, 0},
  {0, 0, 1, 1},
  {0, 0, 0, 1},
  {1, 0, 0, 1}
};

const int ULN2003::motorSequenceFull[4][4] = {
  {1, 1, 0, 0},
  {0, 1, 1, 0},
  {0, 0, 1, 1},
  {1, 0, 0, 1}
};

const char *ULN2003::TAG = "ULN2003";

ULN2003::ULN2003(Mode m, gpio_num_t in1, gpio_num_t in2, gpio_num_t in3, gpio_num_t in4)
    : mode(m), IN1(in1), IN2(in2), IN3(in3), IN4(in4), currentStep(0)
{
    if (mode == HALF_STEP) {
        stepsPerRevolution = 4075.7728395061727/8; //28BYJ-48 的半步模式
    } else {
        stepsPerRevolution = 4075.7728395061727/2; //28BYJ-48 的全步模式
    }
    freqSpeed = 10; // 預設速度
}

ULN2003::~ULN2003()
{
}

void ULN2003::init()
{
    gpio_config_t io_conf = {};
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.pin_bit_mask = (1ULL << IN1) | (1ULL << IN2) | (1ULL << IN3) | (1ULL << IN4);
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
    gpio_config(&io_conf);
    // ESP_LOGI(TAG, "GPIO initialized");
}

void ULN2003::setSpeed(int speed_ms)
{
    freqSpeed = speed_ms;
}

void ULN2003::rotate(int angle)
{
    int steps = (int)round((double)(angle * stepsPerRevolution) / 360.0);
    int direction = (steps > 0) ? 1 : -1;
    for (int i = 0; i < abs(steps); i++) {
        step(direction);
    }
}

void ULN2003::step(int direction)
{
    currentStep += direction;
    if (currentStep >= stepsPerRevolution) {
        currentStep = 0;
    } else if (currentStep < 0) {
        currentStep = stepsPerRevolution - 1;
    }

    int sequenceIndex;
    if (mode == HALF_STEP)
    {
        sequenceIndex = currentStep % 8;
        gpio_set_level(IN1, motorSequenceHalf[sequenceIndex][0]);
        gpio_set_level(IN2, motorSequenceHalf[sequenceIndex][1]);
        gpio_set_level(IN3, motorSequenceHalf[sequenceIndex][2]);
        gpio_set_level(IN4, motorSequenceHalf[sequenceIndex][3]);
    }
    else
    {
        sequenceIndex = currentStep % 4;
        gpio_set_level(IN1, motorSequenceFull[sequenceIndex][0]);
        gpio_set_level(IN2, motorSequenceFull[sequenceIndex][1]);
        gpio_set_level(IN3, motorSequenceFull[sequenceIndex][2]);
        gpio_set_level(IN4, motorSequenceFull[sequenceIndex][3]);
    }
    vTaskDelay(pdMS_TO_TICKS(freqSpeed));
}

double ULN2003::getStepsPerRevolution() const
{
    return stepsPerRevolution;
}

int ULN2003::getCurrentStep() const
{
    return currentStep;
}

void ULN2003::resetStep() {
    currentStep = 0;
}

void ULN2003::setStep(int step) {
    currentStep = step;
}