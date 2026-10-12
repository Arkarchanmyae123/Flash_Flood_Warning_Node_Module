// #ifndef HARDWARE_H
// #define HARDWARE_H

// #include "stm32l4xx_hal.h"
// #include <stdint.h>
// #include <stdio.h>
// #include <stddef.h> 
// /*
// board - NUCLEO-L452RE
// RTC - 32.768 kHz Crystal Oscillator

// JSN-SR04T - (Waterproof Ultrasonic Sensor)
// Tipping-bucket Rain Gauge

// LoRa Module - 868MHz LoRa Module (SX1262)
// GPS Module - MAX-M10S (U-blox)

// Data Logging - W25Q128 (128M-bit SPI Flash Memory)
// Battery Charger - TP5000 (2A LiFePO4 / Lithium Battery Charger IC)
// */


// /*
//     return code 
//     OK = 10 
//     Error = 11
// */

// // string utils function 
// int	ft_strncmp(const char *s1, const char *s2, size_t n);
// double ft_atof(const char *str);
// void	*ft_memcpy_idx(void *dest, const void *src, size_t start, size_t n);



// /* system clock */
// int system_clock_config(void);
// int rtc_config(void);



// /*
//     UART device GPS
// */
// typedef struct {
//     float latitude;
//     float longitude;
//     uint8_t is_valid;
// } GPS_Data_t;

// extern UART_HandleTypeDef uart_1;
// extern uint8_t gps_rx_bytes;


// void gps_gpio_config(void);
// HAL_StatusTypeDef gps_uart_config(void);
// int gps_start(void);
// void gps_process_byte(uint8_t byte);
// int gps_parser(char *nmea_str); // NMEA to Decimal Function 


// /*
//     All spi function 
// */
// extern SPI_HandleTypeDef hspi1;
// void shared_spi1_pins_config(void);
// void lora_specific_pins_config(void);
// void flash_specific_pins_config(void);
// void all_spi_devices_start(void);
// void flash_mem_cs_config(void);
// void lora_cs_config(void);
// void lora_busy_config(void);
// void lora_DIO1_config(void);
// void lora_reset(void);
// void lora_check_busy(void);
// HAL_StatusTypeDef spi1_init(void);
// void lora_select(void);
// void lora_deselect(void);
// void flash_mem_select(void);
// void flash_mem_deselect(void);

// /*
//  lora data transmission and reccive
// */
// void lora_transmit_Tx(uint8_t *pData, uint16_t size);
// void lora_reccive_Rx(uint8_t *pData, uint16_t size);

// /*
//     flash memory operation
// */
// void flash_mem_check_busy(void);
// void flash_mem_del_data(uint32_t Address);
// void flash_mem_write_data(uint32_t Address, uint8_t *pData, uint16_t size);
// void flash_mem_read_data(uint32_t Address, uint8_t *pBuffer, uint16_t size);


// /*
//     UART device GPS
// */
// extern UART_HandleTypeDef uart_1;
// void gps_gpio_config(void);
// HAL_StatusTypeDef gps_uart_config(void);
// int gps_start(void);


// /*
//     sensor using tim2
// */
// extern TIM_HandleTypeDef tim_2;
// HAL_StatusTypeDef tim2_init(void);
// HAL_StatusTypeDef tim2_ic_config(void);
// void JSN_SR04T_config(void);
// void rain_gauge_config(void);
// void lora_DIO1_process_interrupt(void);
// void rain_gauge_process_interrupt(void);
// void HAL_GPIO_EXTI_Callback(uint16_t GPIO_PIN);

// //read water level function 
// void TIM_IC_IT_start(void);
// void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim);
// void delay_micro_sec(uint16_t micro_sec);
// void read_water_level(void);
// #endif





// Orgnize with AI



#ifndef HARDWARE_CONNECT_H
#define HARDWARE_CONNECT_H

#ifdef __cplusplus
extern "C" {
#endif

/* ========================================================================== */
/*  INCLUDES                                                                  */
/* ========================================================================== */
#include "stm32l4xx_hal.h"
#include <stdint.h>
#include <stdio.h>
#include <stddef.h>

/*
    Board        : NUCLEO-L452RE
    RTC          : 32.768 kHz crystal oscillator
    Water level  : JSN-SR04T (waterproof ultrasonic sensor)
    Rain         : Tipping-bucket rain gauge
    LoRa         : 868 MHz LoRa module (SX1262)
    GPS          : MAX-M10S (u-blox)
    Data logging : W25Q128 (128 Mbit SPI flash)
    Charger      : TP5000 (LiFePO4 / Li-ion charger IC)
*/
/* ========================================================================== */
/*  Time out for all                                                                */
/* ========================================================================== */
#define LORA_RX_TX_TIMEOUT 50


/* ========================================================================== */
/*  RETURN CODES                                                              */
/* ========================================================================== */
#define RET_OK      10
#define RET_ERROR   11
#define TIMEOUT     13


/* ========================================================================== */
/*  PIN MAP (reference; .c files may use the literals directly)               */
/* ========================================================================== */
/* SPI1 shared bus (LoRa + Flash) */
#define SPI1_PORT           GPIOA
#define SPI1_PINS           (GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7)  /* SCK, MISO, MOSI */

/* LoRa SX1262 */
#define LORA_CS_PORT        GPIOA
#define LORA_CS_PIN         GPIO_PIN_4
#define LORA_BUSY_PORT      GPIOB
#define LORA_BUSY_PIN       GPIO_PIN_0
#define LORA_DIO1_PORT      GPIOB
#define LORA_DIO1_PIN       GPIO_PIN_1
#define LORA_RESET_PORT     GPIOB
#define LORA_RESET_PIN      GPIO_PIN_2

/* W25Q128 flash */
#define FLASH_CS_PORT       GPIOB
#define FLASH_CS_PIN        GPIO_PIN_4

/* GPS (USART1) */
#define GPS_PORT            GPIOA
#define GPS_TX_PIN          GPIO_PIN_9
#define GPS_RX_PIN          GPIO_PIN_10

/* JSN-SR04T */
#define JSN_TRIG_PORT       GPIOA
#define JSN_TRIG_PIN        GPIO_PIN_1
#define JSN_ECHO_PORT       GPIOA
#define JSN_ECHO_PIN        GPIO_PIN_0      /* TIM2_CH1 */

/* Rain gauge */
#define RAIN_PORT           GPIOC
#define RAIN_PIN            GPIO_PIN_4

/* ========================================================================== */
/*  DATA TYPES                                                                */
/* ========================================================================== */
/* GPS parsed result */
typedef struct
{
    float   latitude;
    float   longitude;
    uint8_t is_valid;
} GPS_Data_t;

/* LoRa transmit packet (packed, no padding) */
#pragma pack(push, 1)
typedef struct
{
    uint8_t  node_id;
    float    latitude;
    float    longitude;
    uint8_t  hour;

    float    current_water_level;

    uint16_t hourly_rain_count;
    float    hourly_water_level;

    uint8_t  warning_status;
} transmit_packet;
#pragma pack(pop)

/* ========================================================================== */
/*  GLOBAL HANDLES / VARIABLES (defined in .c files)                          */
/* ========================================================================== */
extern RTC_HandleTypeDef   hrtc;            /* main.c                 */
extern UART_HandleTypeDef  uart_1;          /* gps_module_config.c    */
extern SPI_HandleTypeDef   hspi1;           /* all_spi_handel_...c    */
extern TIM_HandleTypeDef   tim_2;           /* handle_tim.c           */

extern GPS_Data_t          gps_data_1;      /* gps_module_config.c    */
extern uint8_t             gps_rx_bytes;    /* gps_module_config.c    */

extern float               water_level;     /* read_water_level.c     */
extern volatile uint32_t   rain_cup_throw_time; /* handle_interrupt.c */

/* ========================================================================== */
/*  1. SYSTEM (main.c)                                                        */
/* ========================================================================== */
int  system_clock_config(void);
int  rtc_config(void);

/* ========================================================================== */
/*  2. STRING UTILS (utils.c)                                                 */
/* ========================================================================== */
int     ft_strncmp(const char *s1, const char *s2, size_t n);
double  ft_atof(const char *str);
void   *ft_memcpy_idx(void *dest, const void *src, size_t start, size_t n);

/* ========================================================================== */
/*  3. GPS - USART1 (gps_module_config.c)                                     */
/* ========================================================================== */
void             gps_gpio_config(void);
HAL_StatusTypeDef gps_uart_config(void);
int              gps_start(void);
void             gps_process_byte(uint8_t byte);
int              gps_parser(char *nmea_str);   /* NMEA -> decimal degrees */

/* ========================================================================== */
/*  4. SPI1 SHARED BUS (all_spi_handel_lora_flash_mem.c)                      */
/* ========================================================================== */
HAL_StatusTypeDef spi1_init(void);
void shared_spi1_pins_config(void);
void lora_specific_pins_config(void);
void flash_specific_pins_config(void);
void all_spi_devices_start(void);

/* ========================================================================== */
/*  5. LORA SX1262                                                            */
/* ========================================================================== */
/* pin config (lora_module_config.c) */
void lora_cs_config(void);
void lora_busy_config(void);
void lora_DIO1_config(void);
void lora_reset(void);
int lora_check_busy(void);

/* chip select + data transfer (all_spi_handel_lora_flash_mem.c) */
void lora_select(void);
void lora_deselect(void);
int lora_transmit_Tx(uint8_t *pData, uint16_t size);
int lora_reccive_Rx(uint8_t *pData, uint16_t size);

/* DIO1 interrupt work (handle_interrupt.c) */
int lora_DIO1_mapping_interrupt(void);
void lora_DIO1_process_events(void);

/* ========================================================================== */
/*  6. FLASH MEMORY W25Q128                                                   */
/* ========================================================================== */
/* pin config (flash_memory_W25Q128.c) */
void flash_mem_cs_config(void);

/* chip select + operations (all_spi_handel_lora_flash_mem.c) */
void flash_mem_select(void);
void flash_mem_deselect(void);
void flash_mem_check_busy(void);
void flash_mem_del_data(uint32_t Address);                              /* 4KB sector erase */
void flash_mem_write_data(uint32_t Address, uint8_t *pData, uint16_t size); /* max 256 B per page */
void flash_mem_read_data(uint32_t Address, uint8_t *pBuffer, uint16_t size);

/* ========================================================================== */
/*  7. SENSORS - GPIO CONFIG (sensor.c)                                       */
/* ========================================================================== */
void JSN_SR04T_config(void);
void rain_gauge_config(void);

/* ========================================================================== */
/*  8. TIMER TIM2 (handle_tim.c)                                              */
/* ========================================================================== */
HAL_StatusTypeDef tim2_init(void);
HAL_StatusTypeDef tim2_ic_config(void);

/* ========================================================================== */
/*  9. WATER LEVEL (read_water_level.c)                                       */
/* ========================================================================== */
void TIM_IC_IT_start(void);
void delay_micro_sec(uint16_t micro_sec);
void read_water_level(void);

/* ========================================================================== */
/*  10. INTERRUPT HANDLERS & CALLBACKS (handle_interrupt.c)                   */
/* ========================================================================== */
/* Cortex-M / NVIC handlers */
void SysTick_Handler(void);
void EXTI1_IRQHandler(void);       /* LoRa DIO1   */
void EXTI4_IRQHandler(void);       /* Rain gauge  */
void USART1_IRQHandler(void);      /* GPS UART    */
void TIM2_IRQHandler(void);        /* JSN echo capture - still to be written */

/* Process functions */
void rain_gauge_process_interrupt(void);

/* HAL callbacks */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_PIN);
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart);
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim);   /* defined in read_water_level.c */

#ifdef __cplusplus
}
#endif

#endif /* HARDWARE_CONNECT_H */