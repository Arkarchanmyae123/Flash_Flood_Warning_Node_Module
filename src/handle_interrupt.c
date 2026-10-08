#include "hardware_connect.h"



void lora_DIO1_process_interrupt()
{

}


void rain_gauge_process_interrupt()
{
    volatile uint32_t rain_cup_throw_time = 0;
    static uint32_t last_time_cup_throw = 0;
    uint32_t current_time = HAL_GetTick();

    if ((current_time - last_time_cup_throw) > 500)
    {
        rain_cup_throw_time = rain_cup_throw_time + 1;
        last_time_cup_throw = HAL_GetTick();
    }
}



void HAL_GPIO_EXTI_Callback(uint16_t GPIO_PIN)
{
    if (GPIO_PIN == GPIO_PIN_4)
    {
        rain_gauge_process_interrupt();
    }

}