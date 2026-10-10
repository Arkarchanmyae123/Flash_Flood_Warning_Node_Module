#include "hardware_connect.h"

//GPS Module - MAX-M10S (U-blox)

//PA9(TX_PIN) PA10(RX_PIN)
//UART_1

UART_HandleTypeDef uart_1;

uint8_t gps_rx_bytes = 0;

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
    gps_uart_config();

    if (gps_uart_config() != HAL_OK)
    {
        return 11;
    }

    HAL_NVIC_SetPriority(USART1_IRQn, 2, 0);
    HAL_NVIC_EnableIRQ(USART1_IRQn);

    if (HAL_UART_Receive_IT(&uart_1, &gps_rx_bytes, 1) != HAL_OK)
    {
        return 11;
    }
    return 10;
}

//gps data translation function

char gps_buffer[100];
uint8_t gps_index = 0;
uint8_t is_collecting = 0;

void gps_process_byte(uint8_t byte)
{
    if (byte == '$')
    {
        gps_index = 0;
        is_collecting = 1;
        gps_buffer[gps_index++] = byte;
    }
    else if (is_collecting)
    {
        if (gps_index < sizeof(gps_buffer) - 1)
        {
            gps_buffer[gps_index++] = byte;
        }
        if (byte == '\n')
        {
            gps_buffer[gps_index] = '\0';
            is_collecting = 0;

            gps_parser(gps_buffer);
        }
    }

}