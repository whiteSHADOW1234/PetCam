#pragma once
#include "driver/gpio.h"

class ULN2003
{
public:
    enum Mode
    {
        HALF_STEP,
        FULL_STEP,
    };

    ULN2003(Mode m, gpio_num_t in1, gpio_num_t in2, gpio_num_t in3, gpio_num_t in4);
    ~ULN2003();

    void init();
    void setSpeed(int speed_ms);
    void rotate(int angle);
    void step(int direction);
    double getStepsPerRevolution() const;
    int getCurrentStep() const;
    void resetStep();
    void setStep(int step);

protected:
    Mode mode;
    gpio_num_t IN1, IN2, IN3, IN4;
    double stepsPerRevolution;
    int freqSpeed;
    int currentStep;

private:
    static const int motorSequenceHalf[8][4];
    static const int motorSequenceFull[4][4];
    static const char *TAG;
};