#include "hardware_connect.h"


void SysTick_Handler(void){ 
    HAL_IncTick();
}

void EXTI1_IRQHandler(void){ 
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_1); 
}// LoRa DIO1

void EXTI4_IRQHandler(void){ 
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_4);
}// Rain gauge


int lora_DIO1_mapping_interrupt(void)
{
    uint8_t cmd[9];

    cmd[0] = 0x08;

    cmd[1] = 0x02; // IrqMask High Byte
    cmd[2] = 0x03; // IrqMask Low Byte

    cmd[3] = 0x02; // DIO1Mask High Byte
    cmd[4] = 0x03; // DIO1Mask Low Byte

    // DIO2 Mask: None
    cmd[5] = 0x00; 
    cmd[6] = 0x00; 

    // DIO3 Mask: None
    cmd[7] = 0x00; 
    cmd[8] = 0x00;


    if (lora_check_busy() == RET_OK)
    {
        lora_select();

        HAL_SPI_Transmit(&hspi1, cmd, 9, 100);

        lora_deselect();

        lora_check_busy();
    }
    else
    {
        return RET_ERROR;
    }

}


volatile uint8_t lora_dio1_flag = 0;

void lora_DIO1_process_events(void)
{
    uint8_t get_status_cmd[3] = {0x12, 0x00, 0x00};
    uint8_t rx_buf[3] = {0};
    uint16_t irq_status = 0;

    lora_select();
    while (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_0) == GPIO_PIN_SET);
    
    HAL_SPI_TransmitReceive(&hspi1, get_status_cmd, rx_buf, 3, LORA_RX_TX_TIMEOUT);
    lora_deselect();

    irq_status = ((uint16_t)rx_buf[1] << 8) | rx_buf[2];

    if (irq_status & 0x0001)
    {
        
    }
    if (irq_status & 0x0002)
    {
        
    }

    uint8_t clear_cmd[3];
    clear_cmd[0] = 0x02;
    clear_cmd[1] = rx_buf[1];
    clear_cmd[2] = rx_buf[2];

    lora_select();
    while (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_0) == GPIO_PIN_SET);
    HAL_SPI_Transmit(&hspi1, clear_cmd, 3, LORA_RX_TX_TIMEOUT);
    lora_deselect();
}


volatile uint32_t rain_cup_throw_time = 0;

void rain_gauge_process_interrupt()
{
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
    else if (GPIO_PIN == GPIO_PIN_1)
    {
        lora_dio1_flag = 1;
    }

}


//gps part

void USART1_IRQHandler(void)
{
    HAL_UART_IRQHandler(&uart_1);
}


void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == USART1)
    {
        gps_process_byte(gps_rx_bytes);
        HAL_UART_Receive_IT(huart, &gps_rx_bytes, 1);
    }
}

//tim interrupt

void TIM2_IRQHandler(void)
{
    HAL_TIM_IRQHandler(&tim_2);
}
