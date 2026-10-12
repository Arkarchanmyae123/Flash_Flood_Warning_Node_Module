#include "hardware_connect.h"


TIM_HandleTypeDef tim_2;



HAL_StatusTypeDef tim2_init(void)
{
    __HAL_RCC_TIM2_CLK_ENABLE();

    tim_2.Instance = TIM2;
    tim_2.Init.Prescaler = 15;
    tim_2.Init.CounterMode = TIM_COUNTERMODE_UP;
    tim_2.Init.Period = 0xFFFFFFFF;
    tim_2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    tim_2.Init.RepetitionCounter = 0;
    tim_2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
    return HAL_TIM_IC_Init(&tim_2);
}


HAL_StatusTypeDef tim2_ic_config(void)
{
    TIM_IC_InitTypeDef tim2_input = {0};

    tim2_input.ICPolarity = TIM_ICPOLARITY_RISING;
    tim2_input.ICPrescaler = TIM_ICPSC_DIV1;
    tim2_input.ICSelection = TIM_ICSELECTION_DIRECTTI;
    tim2_input.ICFilter = 0;
    return HAL_TIM_IC_ConfigChannel(&tim_2, &tim2_input, TIM_CHANNEL_1);


}