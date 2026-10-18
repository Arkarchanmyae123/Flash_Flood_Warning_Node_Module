# Flash Flood Warning Node

Firmware project for an STM32 NUCLEO-L452RE flood-monitoring node. The code is
written with STM32 HAL and built with PlatformIO.

## Hardware and source files

| File | What it contains |
| --- | --- |
| `src/main.c` | System clock and RTC setup, then the application's startup sequence. |
| `src/hardware_connect.h` | Shared HAL includes, pin map, data structures, return codes, global declarations, and function prototypes. |
| `src/sensor.c` | GPIO setup for the JSN-SR04T ultrasonic sensor and tipping-bucket rain gauge. |
| `src/handle_tim.c` | TIM2 setup for measuring the ultrasonic sensor's echo pulse. |
| `src/read_water_level.c` | Starts TIM2 input-capture interrupts, sends the ultrasonic trigger pulse, and converts echo time into distance. |
| `src/handle_interrupt.c` | STM32 interrupt handlers and HAL callbacks; receives GPS bytes, tracks rain-gauge pulses, and handles the LoRa DIO1 signal. |
| `src/gps_module_config.c` | USART1 setup, NMEA sentence collection, and parsing of GPS time/status/latitude/longitude. |
| `src/lora_module_config.c` | SX1262 GPIO setup and its BUSY-pin timeout check. |
| `src/all_spi_handel_lora_flash_mem.c` | Shared SPI1 setup and chip-select handling, plus LoRa SPI commands and W25Q128 flash read/erase/write functions. |
| `src/flash_memory_W25Q128.c` | W25Q128 flash chip-select GPIO setup. |
| `src/utils.c` | Small string/number helpers used by the GPS parser. |
| `src/data_packet.c` | Currently only includes the shared header; packet-building/encoding functions have not been implemented here. |
| `platformio.ini` | PlatformIO target: STM32 NUCLEO-L452RE using the STM32Cube framework. |

## Startup flow

`main()` in `src/main.c` currently runs these steps:

1. `HAL_Init()` initializes the STM32 HAL.
2. `system_clock_config()` selects the internal HSI clock and configures the
   system, AHB, and APB clocks.
3. `rtc_config()` selects the external LSE clock and initializes the RTC.
4. `all_spi_devices_start()` initializes SPI1 and configures its shared bus,
   LoRa pins, and flash chip-select pin.
5. `gps_start()` configures USART1 and begins receiving GPS data one byte at a
   time using interrupts.
6. `JSN_SR04T_config()` configures the ultrasonic sensor's trigger and echo
   pins.
7. `tim2_init()` and `tim2_ic_config()` initialize TIM2 and configure its
   input-capture channel.
8. `rain_gauge_config()` enables the rain-gauge GPIO interrupt.
9. The main loop is empty at present.

## Function guide

### System: `src/main.c`

- `system_clock_config()` configures the MCU clocks. Returns `RET_OK` on success
  or `RET_ERROR` if a HAL clock call fails.
- `rtc_config()` enables backup-domain access, selects the LSE RTC clock, and
  initializes the RTC. Uses the same return codes.
- `main()` performs startup and then stays in an empty infinite loop.

### Ultrasonic water-level sensor: `src/sensor.c`, `src/handle_tim.c`, `src/read_water_level.c`

- `JSN_SR04T_config()` configures PA1 as the trigger output and PA0 as the
  TIM2_CH1 echo input.
- `tim2_init()` initializes TIM2 for input capture.
- `tim2_ic_config()` configures TIM2 channel 1 to capture a rising edge.
- `TIM_IC_IT_start()` enables the TIM2 interrupt and starts input capture.
- `read_water_level()` sends a short trigger pulse on PA1.
- `HAL_TIM_IC_CaptureCallback()` records the start and end of the echo pulse,
  calculates its duration, and stores the calculated distance in `water_level`.
- `delay_micro_sec()` provides a short busy-wait delay for the trigger pulse.

### Rain gauge: `src/sensor.c`, `src/handle_interrupt.c`

- `rain_gauge_config()` configures PC4 for a falling-edge interrupt.
- `HAL_GPIO_EXTI_Callback()` dispatches PC4 events to
  `rain_gauge_process_interrupt()`.
- `rain_gauge_process_interrupt()` debounces pulses with a 500 ms minimum
  interval and increments `rain_cup_throw_time`.

### GPS: `src/gps_module_config.c`, `src/handle_interrupt.c`

- `gps_gpio_config()` assigns PA9/PA10 to USART1.
- `gps_uart_config()` configures USART1 for 9600 baud, 8 data bits, no parity,
  and one stop bit.
- `gps_start()` configures the GPS UART, enables its interrupt, and begins
  receiving one byte.
- `USART1_IRQHandler()` forwards USART1 interrupts to the HAL.
- `HAL_UART_RxCpltCallback()` passes each received byte to
  `gps_process_byte()` and starts the next one-byte receive.
- `gps_process_byte()` collects a line beginning with `$` and sends a complete
  line to `gps_parser()`.
- `gps_parser()` accepts `$GNRMC` sentences and converts valid latitude and
  longitude from degrees/minutes to decimal degrees in `gps_data_1`.

### LoRa radio: `src/lora_module_config.c`, `src/all_spi_handel_lora_flash_mem.c`, `src/handle_interrupt.c`

- `lora_cs_config()`, `lora_busy_config()`, `lora_DIO1_config()`, and
  `lora_reset()` configure the SX1262 control pins.
- `lora_check_busy()` waits for the radio BUSY pin to clear, returning
  `TIMEOUT` if it stays busy too long.
- `lora_select()` / `lora_deselect()` control the radio's SPI chip select.
- `lora_write_buffer()` and `lora_read_buffer()` transfer data to/from the
  radio's internal buffer.
- `lora_clear_irq_status()` sends a command to clear radio interrupt flags.
- `lora_send_packet()` prepares the radio and attempts a transmission.
- `lora_DIO1_mapping_interrupt()` configures which radio events trigger DIO1.
- `lora_DIO1_process_events()` reads and clears radio IRQ status.
- `EXTI1_IRQHandler()` and `HAL_GPIO_EXTI_Callback()` handle the DIO1 GPIO
  interrupt; the callback sets `lora_dio1_flag`.

### SPI bus and flash: `src/all_spi_handel_lora_flash_mem.c`, `src/flash_memory_W25Q128.c`

- `spi1_init()` initializes SPI1 as an 8-bit master.
- `shared_spi1_pins_config()` configures PA5/PA6/PA7 for SPI1.
- `lora_specific_pins_config()` and `flash_specific_pins_config()` configure
  each device's control pins.
- `all_spi_devices_start()` calls the SPI and device pin setup functions.
- `flash_mem_cs_config()` configures the flash chip-select output.
- `flash_mem_select()` / `flash_mem_deselect()` control the flash chip select.
- `flash_mem_check_busy()` polls the flash status register until it is ready.
- `flash_mem_del_data()` erases the 4 KB flash sector containing an address.
- `flash_mem_write_data()` programs bytes into flash at an address.
- `flash_mem_read_data()` reads bytes from flash at an address.

### Interrupts and utilities

- `SysTick_Handler()`, `EXTI1_IRQHandler()`, `EXTI4_IRQHandler()`,
  `USART1_IRQHandler()`, and `TIM2_IRQHandler()` forward MCU interrupts to HAL
  handlers.
- `ft_strncmp()` compares up to a specified number of characters.
- `ft_atof()` converts a decimal string to a floating-point value.
- `ft_memcpy_idx()` copies bytes from a source buffer starting at an offset.

## Things to know about the current code

- The main loop is empty: it does not yet schedule sensor reads, build/send
  packets, or save measurements to flash.
- `TIM_IC_IT_start()` and `read_water_level()` are defined but are not called
  from `main()`. The sensor pins and timer are configured, but the capture
  interrupt is not started by the current startup sequence.
- The LoRa receive function in `src/all_spi_handel_lora_flash_mem.c` is
  incomplete (`lora_recive_packet()` currently has no receive logic or return
  value). The header also declares differently named transmit/receive
  functions, so the LoRa API declarations and implementations need to be
  reconciled before relying on them.
- Most initialization return values are currently ignored in `main()`, so a
  peripheral setup failure does not stop startup or report an error.
- `water_level` and `rain_cup_throw_time` are measurement values; they are not
  yet assembled into the `transmit_packet` structure declared in
  `src/hardware_connect.h`.
