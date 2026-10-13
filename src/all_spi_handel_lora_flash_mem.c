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

// chip select for lora
void lora_select(void)
{
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET);
}

void lora_deselect(void)
{
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET);
}

// chip select for flash mem
void flash_mem_select(void)
{
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, GPIO_PIN_RESET);
}
void flash_mem_deselect(void)
{
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, GPIO_PIN_SET);
}



int lora_write_buffer(uint8_t offset, uint8_t *pData, uint8_t size)
{
   if (lora_check_busy() != RET_OK)
   {
    return RET_ERROR;
   }

   uint8_t cmd[2] = {0x0E, offset};

   lora_select();

   HAL_SPI_Transmit(&hspi1, cmd, 2, LORA_RX_TX_TIMEOUT);
   HAL_SPI_Transmit(&hspi1, pData, size, LORA_RX_TX_TIMEOUT);

   lora_deselect();

   if (lora_check_busy() != RET_OK)
   {
    return RET_ERROR;
   }
   return RET_OK;

}


int lora_read_buffer(uint8_t offset, uint8_t *pBuffer, uint8_t size)
{
    if (lora_check_busy() != RET_OK)
    {
        return RET_ERROR;
    }

    uint8_t cmd[3] = {0x1E, offset, 0x00};

    lora_select();

    HAL_SPI_Transmit(&hspi1, cmd, 3, LORA_RX_TX_TIMEOUT);
    HAL_SPI_Receive(&hspi1, pBuffer, size, LORA_RX_TX_TIMEOUT);

    lora_deselect();

    if (lora_check_busy() != RET_OK)
    {
        return RET_ERROR;
    }

    return RET_OK;
}

int lora_clear_irq_status(uint16_t mask)
{
    if (lora_check_busy() != RET_OK)
    {
        return RET_OK;
    }

    //uint8_t cmd[3] = {0x02, (uint8_t)mask >> 8, (uint8_t)mask & 0xFF};
    uint8_t cmd[3];
            cmd[0] = 0x02;
            cmd[1] = (uint8_t)mask >> 8;
            cmd[2] = (uint8_t)mask & 0xFF;

    lora_select();

    HAL_SPI_Transmit(&hspi1, cmd, 3, LORA_RX_TX_TIMEOUT);

    lora_deselect();

    if (lora_check_busy() != RET_OK)
    {
        return RET_OK;
    }

    return RET_OK;

}


static int lora_cmd(uint8_t *buf, uint16_t len)
{
    lora_select();
    HAL_StatusTypeDef st = HAL_SPI_Transmit(&hspi1, buf, len, 100);
    lora_deselect();
    
    if (st != HAL_OK) 
    {
        return RET_ERROR;
    }
    
    return lora_check_busy();
}


int lora_send_packet(uint8_t *pData, uint8_t size)
{
    if (lora_check_busy() != RET_OK) return RET_ERROR;

    uint8_t standby[2]  = {0x80, 0x00};// SetStandby
    uint8_t bufbase[3]  = {0x8F, 0x00, 0x00};// SetBufferBaseAddress
    uint8_t pktparam[7] = {0x8C, 0x00, 0x08, 0x00, size, 0x01, 0x00};// SetPacketParams
    uint8_t irq[9]      = {0x08, 0x02, 0x01, 0x02, 0x01, 0x00, 0x00, 0x00, 0x00}; // SetDioIrqParams
    uint8_t settx[4]    = {0x83, 0x00, 0x00, 0x00}; // SetTx

    if (lora_cmd(standby, 2) != RET_OK) return RET_ERROR;
    if (lora_cmd(bufbase, 3) != RET_OK) return RET_ERROR;

    if (lora_write_buffer(0x00, pData, size) != RET_OK) return RET_ERROR;

    if (lora_cmd(pktparam, 7) != RET_OK) return RET_ERROR;
    if (lora_cmd(irq, 9)      != RET_OK) return RET_ERROR;

    lora_clear_irq_status(0x03FF); 

    if (lora_cmd(settx, 4) != RET_OK) return RET_ERROR;

    uint32_t start_time = HAL_GetTick();
    
    while (HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_1) == GPIO_PIN_RESET)
    {
        if ((HAL_GetTick() - start_time) >= 2000)
        {
            lora_cmd(standby, 2);
            lora_clear_irq_status(0x03FF);
            return RET_ERROR;
        }
    }

    lora_clear_irq_status(0x03FF);

    return RET_OK;
}


int lora_recive_packet(uint8_t *pBuffer, uint8_t size)
{
    if (lora_check_busy() != RET_OK)
    {
        return RET_ERROR;
    }
    uint8_t Buffer_Base_Addr[3] = {0x8F, 0x00, 0x00};
    uint8_t Buffer_Base_Addr[2] = {0x8F, 0x00};
    uint8_t PacketParams[7] = {0x8C, 0x00, 0x08, 0x00, size, 0x01, 0x00};
    uint8_t DioIrqParams[9] = {0x08, 0x02, 0x62, 0x02, 0x62, 0x00, 0x00, 0x00, 0x00};
    uint8_t SetRx[4] = {0x82, 0x01, 0xF4, 0x00};
    uint8_t tx_buf[4] = {0x12, 0x00, 0x00, 0x00};
    uint8_t rx_buf[4];

}




















void flash_mem_check_busy()
{
    uint8_t status_reg_cmd = 0x05; // Read Status Register-1 Command
    uint8_t status_byte = 0;

    do
    {
        flash_mem_select();

        HAL_SPI_Transmit(&hspi1, &status_reg_cmd, 1, 100);
        HAL_SPI_Receive(&hspi1, &status_byte, 1, 100);

        flash_mem_deselect();

    } while ((status_byte & 0x01) == 0x01);
    
}

void flash_mem_del_data(uint32_t Address) //4KB sector erase function register is 0x20
{
    uint8_t write_open_cmd = 0x06; //write open register 
    uint8_t del_cmd = 0x20;
    uint8_t addr_bytes[3];

    addr_bytes[0] = (Address >> 16) & 0xFF;
    addr_bytes[1] = (Address >> 8) & 0xFF;
    addr_bytes[2] = Address & 0xFF;

    flash_mem_select();
    HAL_SPI_Transmit(&hspi1, &write_open_cmd, 1, 100);
    flash_mem_deselect();

    flash_mem_select();
    HAL_SPI_Transmit(&hspi1, &del_cmd, 1, 100);
    HAL_SPI_Transmit(&hspi1, addr_bytes, 3, 100);
    flash_mem_deselect();

    flash_mem_check_busy();
}

void flash_mem_write_data(uint32_t Address, uint8_t *pData, uint16_t size)
{
    uint8_t write_open_cmd = 0x06;
    uint8_t page_program_cmd =0x02; //page program cmd register
    uint8_t addr_bytes[3];
    
    addr_bytes[0] = (Address >> 16) & 0xFF;
    addr_bytes[1] = (Address >> 8) & 0xFF;
    addr_bytes[2] = Address & 0xFF;

    flash_mem_select();
    HAL_SPI_Transmit(&hspi1, &write_open_cmd, 1, 100);
    flash_mem_deselect();

    flash_mem_select();
    HAL_SPI_Transmit(&hspi1, &page_program_cmd, 1, 100);
    HAL_SPI_Transmit(&hspi1, addr_bytes, 3, 100);
    HAL_SPI_Transmit(&hspi1, pData, size, HAL_MAX_DELAY);
    flash_mem_deselect();

    flash_mem_check_busy();

}

void flash_mem_read_data(uint32_t Address, uint8_t *pBuffer, uint16_t size)
{
    uint8_t read_cmd = 0x03;
    uint8_t addr_bytes[3];

    addr_bytes[0] = (Address >> 16) & 0xFF;
    addr_bytes[1] = (Address >> 8) & 0xFF;
    addr_bytes[2] = Address & 0xFF;

    flash_mem_select();
    HAL_SPI_Transmit(&hspi1, &read_cmd, 1, 100);
    HAL_SPI_Transmit(&hspi1, addr_bytes, 3, 100);

    HAL_SPI_Receive(&hspi1, pBuffer, size, HAL_MAX_DELAY);

    flash_mem_deselect();
}
