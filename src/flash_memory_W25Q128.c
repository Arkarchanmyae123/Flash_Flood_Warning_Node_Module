#include "hardware_connect.h"

//W25Q128 Flash Memory

// void flash_mem_config()
// {
//     __HAL_RCC_GPIOA_CLK_ENABLE();

//     GPIO_InitTypeDef gpioA = {0};

//     gpioA.Pin = GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7;
//     gpioA.Mode = GPIO_MODE_AF_PP;
//     gpioA.Pull = GPIO_NOPULL;
//     gpioA.Speed = GPIO_SPEED_FREQ_HIGH;
//     gpioA.Alternate = GPIO_AF5_SPI1;
//     HAL_GPIO_Init(GPIOA, &gpioA);
// }

void flash_mem_cs_config(void)
{
    /*
    before Flash Memory work always HIGH  
    when store data or del data make low
    */
    __HAL_RCC_GPIOB_CLK_ENABLE();

    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, GPIO_PIN_SET);
    GPIO_InitTypeDef gpioB = {0};
    gpioB.Mode = GPIO_MODE_OUTPUT_PP;
    gpioB.Pin = GPIO_PIN_4;
    gpioB.Pull = GPIO_NOPULL;
    gpioB.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOB, &gpioB);
}

// void flash_mem_start()
// {
//     flash_mem_config();
//     flash_mem_cs_config();
// }

