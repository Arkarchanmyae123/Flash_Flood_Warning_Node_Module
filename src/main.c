#include "stm32l4xx_hal.h"
#include "hardware_connect.h"

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

RTC_HandleTypeDef hrtc; //real time clock

int system_clock_config(void)
{
    RCC_OscInitTypeDef RCC_Oscinit = {0};
    RCC_ClkInitTypeDef RCC_Clkinit = {0};

    RCC_Oscinit.OscillatorType = RCC_OSCILLATORTYPE_HSI;
    RCC_Oscinit.HSIState = RCC_HSI_ON;
    RCC_Oscinit.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
    RCC_Oscinit.PLL.PLLState = RCC_PLL_NONE;

    if (HAL_RCC_OscConfig(&RCC_Oscinit) != HAL_OK)
    {
        return (11);
    }

    RCC_Clkinit.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
    RCC_Clkinit.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
    RCC_Clkinit.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_Clkinit.APB1CLKDivider = RCC_HCLK_DIV1;
    RCC_Clkinit.APB2CLKDivider = RCC_HCLK_DIV1;

    if (HAL_RCC_ClockConfig(&RCC_Clkinit, FLASH_LATENCY_0) != HAL_OK)
    {
        return (11);
    }

    return (10);
}

int rtc_config(void)
{
    __HAL_RCC_PWR_CLK_ENABLE();
    HAL_PWR_EnableBkUpAccess();

    RCC_OscInitTypeDef RCC_Oscinit = {0};
    RCC_Oscinit.OscillatorType = RCC_OSCILLATORTYPE_LSE;
    RCC_Oscinit.LSEState = RCC_LSE_ON;
    if (HAL_RCC_OscConfig(&RCC_Oscinit) != HAL_OK)
    {
        return (11);
    }

    RCC_PeriphCLKInitTypeDef RCC_Peripinit = {0};
    RCC_Peripinit.PeriphClockSelection = RCC_PERIPHCLK_RTC;
    RCC_Peripinit.RTCClockSelection = RCC_RTCCLKSOURCE_LSE;

    if (HAL_RCCEx_PeriphCLKConfig(&RCC_Peripinit) != HAL_OK)
    {
        return (11);
    }

    __HAL_RCC_RTC_ENABLE();
    hrtc.Instance = RTC;
    hrtc.Init.HourFormat = RTC_HOURFORMAT_24;
    hrtc.Init.AsynchPrediv = 127;
    hrtc.Init.SynchPrediv = 255;
    hrtc.Init.OutPut = RTC_OUTPUT_DISABLE;

    if (HAL_RTC_Init(&hrtc) != HAL_OK)
    {
        return (11);
    }

    return (10);
}


int main(void)
{
    HAL_Init();
    system_clock_config();
    rtc_config();
    
    all_spi_devices_start();

    gps_start();

    JSN_SR04T_config();

    tim2_init();

    tim2_ic_config();
    
    rain_gauge_config();

    while(1)
    {

    }
    
}








