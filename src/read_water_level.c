#include "hardware_connect.h"

uint16_t T1 = 0;
uint16_t T2 = 0;
uint32_t time_diff = 0;
uint16_t flag = 0;
float water_level = 0.0f;

void TIM_IC_IT_start(void)
{
    HAL_NVIC_SetPriority(TIM2_IRQn, 1, 0);
    HAL_NVIC_EnableIRQ(TIM2_IRQn);
    HAL_TIM_IC_Start_IT(&tim_2, TIM_CHANNEL_1);

}


void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == TIM2 && htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1)
    {
        if (flag == 0)
        {
            T1 = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1);
            flag = 1;
            __HAL_TIM_SET_CAPTUREPOLARITY(htim, TIM_CHANNEL_1, TIM_INPUTCHANNELPOLARITY_FALLING);
        }
        else if (flag == 1)
        {
            T2 = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_1);

            if (T2 > T1)
            {
                time_diff = (T2 - T1);
            }
            else
            {
                time_diff = (0xFFFFFFFF - T1) + T2;
            }
            water_level = (time_diff * 0.0343) / 2;

            flag = 0;

             __HAL_TIM_SET_CAPTUREPOLARITY(htim, TIM_CHANNEL_1, TIM_INPUTCHANNELPOLARITY_RISING);

        }
        
    }
}

void delay_micro_sec(uint16_t micro_sec)
{
    for (int i = 0; i < (micro_sec * 10); i++)
    {
        __NOP();
    }
}

void read_water_level(void)
{
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_SET);
    delay_micro_sec(10);
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_RESET);

}