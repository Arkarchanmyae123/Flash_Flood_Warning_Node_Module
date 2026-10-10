#ifndef hardware_h
#define hardware_h

#include "stm32l4xx_hal.h"
#include "stdint.h"
#include "stdio.h"

/*
board - NUCLEO-L452RE
RTC - 32.768 kHz Crystal Oscillator

JSN-SR04T - (Waterproof Ultrasonic Sensor)
Tipping-bucket Rain Gauge

LoRa Module - 868MHz LoRa Module (SX1262)
GPS Module - MAX-M10S (U-blox)

Data Logging - W25Q128 (128M-bit SPI Flash Memory)
Battery Charger - TP5000 (2A LiFePO4 / Lithium Battery Charger IC)
*/


/*
    return code 
    OK = 10 
    Error = 11
*/


/* system clock */
int system_clock_config(void);
int rtc_config(void);



/*
    All spi function 
*/
extern SPI_HandleTypeDef hspi1;
void shared_spi1_pins_config(void);
void lora_specific_pins_config(void);
void flash_specific_pins_config(void);
void all_spi_devices_start(void);
void flash_mem_cs_config(void);
void lora_cs_config(void);
void lora_busy_config(void);
void lora_DIO1_config(void);
void lora_reset(void);
void lora_check_busy(void);
HAL_StatusTypeDef spi1_init(void);
void lora_select(void);
void lora_deselect(void);
void flash_mem_select(void);
void flash_mem_deselect(void);

/*
 lora data transmission and reccive
*/
void lora_transmit_Tx(uint8_t *pData, uint16_t size);
void lora_reccive_Rx(uint8_t *pData, uint16_t size);

/*
    flash memory operation
*/
void flash_mem_check_busy(void);
void flash_mem_del_data(uint32_t Address);
void flash_mem_write_data(uint32_t Address, uint8_t *pData, uint16_t *size);
void flash_mem_read_data(uint32_t Address, uint8_t *pBuffer, uint16_t *size);


/*
    UART device GPS
*/
extern gps_rx_bytes;
extern UART_HandleTypeDef uart_1;
void gps_gpio_config(void);
HAL_StatusTypeDef gps_uart_config(void);
void gps_process_byte(uint8_t *gps_data);
int gps_start(void);


/*
    sensor using tim2
*/
extern TIM_HandleTypeDef tim_2;
HAL_StatusTypeDef tim2_init(void);
HAL_StatusTypeDef tim2_ic_config(void);
void JSN_SR04T_config(void);
void rain_gauge_config(void);
void lora_DIO1_process_interrupt(void);
void rain_gauge_process_interrupt(void);
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_PIN);
#endif