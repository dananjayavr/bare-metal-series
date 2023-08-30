//
// Created by Dananjaya RAMANAYAKE on 26/08/2023.
//

#include "application.h"
#include "stm32f4xx_ll_gpio.h"
#include "main.h"

extern UART_HandleTypeDef huart2;
extern TIM_HandleTypeDef htim2;

#define PRESCALER (84)
#define ARR       (1000)
// 84_000_000
// freq = system_freq / ((prescaler - 1) * (arr - 1))
// duty cycle = (ccr / arr) * 100
// duty cycle / 100 = ccr / arr
// ccr = arr * (duty cycle / 100)

float duty_cycle = 0.0f;

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
    if(GPIO_Pin == GPIO_PIN_13) {
        printf("Button Pressed.\r\n");
    }
}

void timer_pwm_set_duty_cycle(float duty_cycle) {
    const float raw_value = (float)ARR * (duty_cycle / 100.0f);
    htim2.Instance->CCR1 = (uint32_t )raw_value;
}

void setup(void) {
    RetargetInit(&huart2);
    printf("Hello, World!\r\n");

    timer_pwm_set_duty_cycle(duty_cycle);
    HAL_TIM_PWM_Start(&htim2,TIM_CHANNEL_1);
}

void loop(void) {

    duty_cycle += 1.0f;
    if(duty_cycle > 100.0f) {
        duty_cycle = 0.0f;
    }
    timer_pwm_set_duty_cycle(duty_cycle);
    HAL_Delay(10);
}
