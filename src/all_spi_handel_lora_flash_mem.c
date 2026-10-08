#include "hardware_connect.h"


SPI_HandleTypeDef hspi1;

HAL_StatusTypeDef spi1_init(void)
{
    __HAL_RCC_SPI1_CLK_ENABLE();
    hspi1.Instance = SPI1;
    hspi1.Init.Mode = SPI_MODE_MASTER;
    hspi1.Init.Direction = SPI_DIRECTION_2LINES;
    hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
    hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
    hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
    hspi1.Init.NSS = SPI_NSS_SOFT;
    hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_8;
    hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
    hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
    hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
    hspi1.Init.CRCPolynomial = 7;
    hspi1.Init.NSSPMode = SPI_NSS_PULSE_DISABLE;
    return HAL_SPI_Init(&hspi1);
}



void shared_spi1_pins_config(void)
{
    __HAL_RCC_GPIOA_CLK_ENABLE();
    GPIO_InitTypeDef gpioA = {0};
    gpioA.Pin = GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7;
    gpioA.Mode = GPIO_MODE_AF_PP;
    gpioA.Pull = GPIO_NOPULL;
    gpioA.Speed = GPIO_SPEED_FREQ_HIGH;
    gpioA.Alternate = GPIO_AF5_SPI1;
    HAL_GPIO_Init(GPIOA, &gpioA);
}

void lora_specific_pins_config(void)
{
    lora_cs_config();    // PA4
    lora_busy_config();  // PB0
    lora_DIO1_config();  // PB1
    lora_reset();        // PB2
}

void flash_specific_pins_config(void)
{
    flash_mem_cs_config(); // PB4 
}

void all_spi_devices_start(void)
{
    spi1_init();

    //main pin route for lora and flash mem
    shared_spi1_pins_config();
    
    //opeing lora's all function
    lora_specific_pins_config();
    
    //open flash chip slect 
    flash_specific_pins_config();
}