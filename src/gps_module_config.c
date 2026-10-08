#include "hardware_connect.h"

//GPS Module - MAX-M10S (U-blox)

//PA9(TX_PIN) PA10(RX_PIN)
//UART_1

UART_HandleTypeDef uart_1;
void gps_gpio_config(void)
{
    __HAL_RCC_GPIOA_CLK_ENABLE();
    GPIO_InitTypeDef gpioA = {0};

    gpioA.Mode = GPIO_MODE_AF_PP;
    gpioA.Pin = GPIO_PIN_9 | GPIO_PIN_10;
    gpioA.Pull = GPIO_PULLUP;
    gpioA.Speed = GPIO_SPEED_FREQ_HIGH;
    gpioA.Alternate = GPIO_AF7_USART1;
    HAL_GPIO_Init(GPIOA, &gpioA);
}

HAL_StatusTypeDef gps_uart_config(void)
{
    __HAL_RCC_USART1_CLK_ENABLE();
    uart_1.Instance = USART1;
    uart_1.Init.BaudRate = 9600;
    uart_1.Init.WordLength = UART_WORDLENGTH_8B;
    uart_1.Init.StopBits = UART_STOPBITS_1;
    uart_1.Init.Parity = UART_PARITY_NONE;
    uart_1.Init.Mode = UART_MODE_TX_RX;
    uart_1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    uart_1.Init.OverSampling = UART_OVERSAMPLING_16;
    
    return HAL_UART_Init(&uart_1);
}

int gps_start(void)
{
    gps_gpio_config();
    if (gps_uart_config() == HAL_OK)
    {
        return 10;
    }
    else
    {
        return 11;
    }
}