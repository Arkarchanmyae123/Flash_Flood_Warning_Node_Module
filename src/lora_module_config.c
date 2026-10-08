#include "hardware_connect.h"

//LoRa Module - 868MHz LoRa Module (SX1262)


// void lora_sck_scl_mosi_miso_config()
// {
//     //lora module sx1262 use gpioA and gpioB
//     __HAL_RCC_GPIOA_CLK_ENABLE();

//     GPIO_InitTypeDef gpioA = {0};

//     gpioA.Mode = GPIO_MODE_AF_PP;
//     gpioA.Pin = GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7; //SCK and SCL one line PA5 and pa6 and pa7 miso and mosi
//     gpioA.Pull = GPIO_NOPULL;
//     gpioA.Speed = GPIO_SPEED_FREQ_HIGH;
//     gpioA.Alternate = GPIO_AF5_SPI1;
//     HAL_GPIO_Init(GPIOA, &gpioA);

// }

void lora_cs_config(void)
{
    //lora spi chip slect pin
    __HAL_RCC_GPIOA_CLK_ENABLE();

    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET);
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = GPIO_PIN_4;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

}

void lora_busy_config(void)
{
    //lora check busy input pin
    __HAL_RCC_GPIOB_CLK_ENABLE();

    GPIO_InitTypeDef GPIO_InitStruct = {0};

    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pin = GPIO_PIN_0;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

}

void lora_DIO1_config(void)
{
    /*
        နောက်တစ်ဆင့်အတွက် ကြိုတင် အသိပေးချက် (Interrupt)
        lora_DIO1_config() တွင် GPIO_MODE_IT_RISING (Interrupt) အဖြစ် သတ်မှတ်ထားသောကြောင့် ဤ Pin မှ Signal ဝင်လာလျှင် STM32 က သိရှိနိုင်ရန် သင်၏ main() (သို့မဟုတ် သီးသန့် Interrupt Config Function တစ်ခု) အတွင်း၌ အောက်ပါအတိုင်း NVIC ကို ဖွင့်ပေးရန် လိုအပ်ပါလိမ့်မည်။
        EXTI Line 1 (PB1 အတွက်) Interrupt ကို ဖွင့်ခြင်း
        HAL_NVIC_SetPriority(EXTI1_IRQn, 0, 0);
        HAL_NVIC_EnableIRQ(EXTI1_IRQn);
    */
    //lora interrupt pin (DIO1: STM32 ၏ PB1 သို့ ဆက်ပါ။ (Data ပို့/လက်ခံ ပြီးစီးကြောင်း STM32 ထံ အသိပေးမည့် Interrupt/EXTI Pin))
    __HAL_RCC_GPIOB_CLK_ENABLE();

    GPIO_InitTypeDef GPIO_InitStruct = {0};

    GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
    GPIO_InitStruct.Pin = GPIO_PIN_1;
    GPIO_InitStruct.Pull = GPIO_PULLDOWN;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    HAL_NVIC_SetPriority(EXTI1_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(EXTI1_IRQn);
}

void lora_reset(void)
{
    //lora rest ((စနစ် Error တက်ပါက LoRa ကို Restart ပြန်လုပ်ရန် Output))
    __HAL_RCC_GPIOB_CLK_ENABLE();
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_2, GPIO_PIN_SET);

    GPIO_InitTypeDef GPIO_InitStruct = {0};

    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pin = GPIO_PIN_2;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
}

// void lora_start()
// {
//     lora_sck_scl_mosi_miso_config();
//     lora_cs_config();
// }