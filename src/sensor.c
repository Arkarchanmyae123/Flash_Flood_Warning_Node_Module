#include "hardware_connect.h"


//JSN-SR04T - (Waterproof Ultrasonic Sensor)

void JSN_SR04T_config(void)
{
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_TIM2_CLK_ENABLE();
    
    GPIO_InitTypeDef gpioA = {0};

    gpioA.Mode = GPIO_MODE_OUTPUT_PP;
    gpioA.Pin = GPIO_PIN_1;
    gpioA.Pull = GPIO_NOPULL;
    gpioA.Speed = GPIO_SPEED_FREQ_MEDIUM;
    HAL_GPIO_Init(GPIOA, &gpioA);

    gpioA.Mode = GPIO_MODE_AF_PP;
    gpioA.Pin = GPIO_PIN_0;
    gpioA.Pull = GPIO_NOPULL;
    gpioA.Alternate = GPIO_AF1_TIM2;
    HAL_GPIO_Init(GPIOA, &gpioA);
}

void rain_gauge_config(void)
{
    __HAL_RCC_GPIOC_CLK_ENABLE();

    GPIO_InitTypeDef gpioC = {0};
    gpioC.Mode = GPIO_MODE_IT_FALLING;
    gpioC.Pin = GPIO_PIN_4;
    gpioC.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(GPIOC, &gpioC);

    HAL_NVIC_SetPriority(EXTI4_IRQn, 1, 0);
    HAL_NVIC_EnableIRQ(EXTI4_IRQn);
}