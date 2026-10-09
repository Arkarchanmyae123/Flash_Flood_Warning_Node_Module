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


void SysTick_Handler(void){ 
    HAL_IncTick();
}

void EXTI1_IRQHandler(void){ 
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_1); 
}// LoRa DIO1

void EXTI4_IRQHandler(void){ 
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_4);
}// Rain gauge



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

/*
    UART device GPS
*/
extern UART_HandleTypeDef uart_1;
void gps_gpio_config(void);
HAL_StatusTypeDef gps_uart_config(void);
int gps_start(void);

/*
    sensor using tim2
*/
extern TIM_HandleTypeDef tim_2;
HAL_StatusTypeDef tim2_init(void);
HAL_StatusTypeDef tim2_ic_config(void);
void JSN_SR04T_config(void);
void rain_gauge_config(void);


#endif