//
// Created by Dananjaya RAMANAYAKE on 26/08/2023.
//

#include "application.h"
#include "stm32f4xx_ll_gpio.h"

extern UART_HandleTypeDef huart2;
extern TIM_HandleTypeDef htim2;
extern uint8_t uart_ready;
extern uint8_t data_available;

extern uint8_t data_buffer;

void timer_pwm_set_duty_cycle(float duty_cycle);

#define PRESCALER (84)
#define ARR       (1000)
// 84_000_000
// freq = system_freq / ((prescaler - 1) * (arr - 1))
// duty cycle = (ccr / arr) * 100
// duty cycle / 100 = ccr / arr
// ccr = arr * (duty cycle / 100)

float duty_cycle = 0.0f;
float direction = 1.0f;

uint8_t button_flag = 0;

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
    if(GPIO_Pin == GPIO_PIN_13) {
        // IMPLEMENT ME!
        button_flag = 1;
    }
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart) {
    uart_ready = SET;
    data_available = 0;
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
    uart_ready = SET;
    data_available = 1;
}

void timer_pwm_set_duty_cycle(float duty_cycle) {
    const float raw_value = (float)ARR * (duty_cycle / 100.0f);
    htim2.Instance->CCR1 = (uint32_t )raw_value;
}

void setup(void) {

    uart_write("uart_write (application)\r\n",26);

    timer_pwm_set_duty_cycle(duty_cycle);
    HAL_TIM_PWM_Start(&htim2,TIM_CHANNEL_1);
}

void loop(void) {

    duty_cycle += direction;

    if(duty_cycle > 100.0f) {
        duty_cycle = 100.0f;
        direction = -1.0f;
    } else if (duty_cycle < 0.0f) {
        duty_cycle = 0.0f;
        direction = 1.0f;
    }

    timer_pwm_set_duty_cycle(duty_cycle);

    if(button_flag) {
        uart_write("Button pressed (app).\r\n",23);
        button_flag = 0;
    }

    uint8_t data = uart_read_byte();
    while(data_available) {
        uart_write_byte(data + 1);
        data_available = 0;
    }

    HAL_Delay(5);
}
