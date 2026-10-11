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

//field 2 error code 11
char field_1[15]; //time
char field_2[2]; // status for satllite 'A' active 'V' no active
char field_3[15]; // lat
char field_4[2]; // NS north south
char field_5[15]; // lon
char field_6[2]; // EW east west
// field 3 and 5 lat and lon

GPS_Data_t gps_data_1;

int gps_parser(char *nmea_str)
{
    int comma_count = 0;
    int char_index = 0;
    if (ft_strncmp(nmea_str, "$GNRMC", 6) == 0)
    {
        for (int i = 0; nmea_str[i] != '\0'; i++)
        {
            if (nmea_str[i] == ',')
            {
                comma_count = comma_count + 1;
                char_index = 0;
                continue;
            }
            if (comma_count == 1)
            {
                field_1[char_index++] = nmea_str[i];
                field_1[char_index] = '\0';
            }
            else if (comma_count == 2)
            {
                field_2[char_index++] = nmea_str[i];
                field_2[char_index] = '\0';
            }
            else if (comma_count == 3)
            {
                field_3[char_index++] = nmea_str[i];
                field_3[char_index] = '\0';
            }
            else if (comma_count == 4)
            {
                field_4[char_index++] = nmea_str[i];
                field_4[char_index] = '\0';
            }
            else if (comma_count == 5)
            {
                field_5[char_index++] = nmea_str[i];
                field_5[char_index] = '\0';
            }
            else if (comma_count == 6)
            {
                field_6[char_index++] = nmea_str[i];
                field_6[char_index] = '\0';
            }

        }
        if (field_2[0] == 'A')
        {
            gps_data_1.is_valid = 1;
            char lat_deg[3];
            char lat_min[8];

            char lon_deg[4];
            char lon_min[8];

            ft_memcpy_idx(lat_deg, field_3, 0, 2);// copy for lat_deg
            ft_memcpy_idx(lat_min, field_3, 2, 7);// copy for lat min

            ft_memcpy_idx(lon_deg, field_5, 0, 3);// copy for lon deg
            ft_memcpy_idx(lon_min, field_5, 3, 7);// copy lon min

               lat_deg[2] = '\0';
               lat_min[7] = '\0';
               lon_deg[3] = '\0';
               lon_min[7] = '\0';

            float lat_deg_float_num = ft_atof(lat_deg);
            float lat_min_float_num =  ft_atof(lat_min);
            
            float lon_deg_float_num = ft_atof(lon_deg);
            float lon_min_float_num = ft_atof(lon_min);

            float lat_degree = lat_deg_float_num + (lat_min_float_num / 60.0f);
            float lon_degree = lon_deg_float_num + (lon_min_float_num / 60.0f);

            if (field_4[0] == 'S')
            {
                lat_degree = lat_degree * (-1.0f);
            }
            if (field_6[0] == 'W')
            {
                lon_degree = lon_degree * (-1.0f);
            }

            gps_data_1.latitude = lat_degree;
            gps_data_1.longitude = lon_degree;
            
            return (10);
        }
        else
        {
            gps_data_1.is_valid = 0;
            return (11);
        }
    }
    return (1);//not "$GNRMC"

}