//
// Created by Dananjaya RAMANAYAKE on 26/08/2023.
//

#include "application.h"
#include "stm32f4xx_ll_gpio.h"

extern UART_HandleTypeDef huart2;
extern TIM_HandleTypeDef htim2;
extern uint8_t uart_ready;

void timer_pwm_set_duty_cycle(float duty_cycle);

extern ring_buffer_t rb;
extern uint8_t data_buffer[RING_BUFFER_SIZE];

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
uint8_t data;

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
    if(GPIO_Pin == GPIO_PIN_13) {
        button_flag = 1;
    }
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
    uart_ready = SET;
    ring_buffer_write(&rb,data);
}

void timer_pwm_set_duty_cycle(float duty_cycle) {
    const float raw_value = (float)ARR * (duty_cycle / 100.0f);
    htim2.Instance->CCR1 = (uint32_t )raw_value;
}

void setup(void) {

    uart_write("uart_write (application)\r\n",26);

    timer_pwm_set_duty_cycle(duty_cycle);
    HAL_TIM_PWM_Start(&htim2,TIM_CHANNEL_1);

    ring_buffer_setup(&rb,data_buffer,RING_BUFFER_SIZE);
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
        uart_write("\r\nButton pressed (app).\r\n",23);
        button_flag = 0;
    }

    HAL_UART_Receive_IT(&huart2,&data,1);

    while(uart_data_available()) {
        uint8_t data = uart_read_byte();
        uart_write_byte(data + 1);
    }

    HAL_Delay(5);
}
