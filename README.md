# STM32 DHT11 MPU6050 LCD Multi-Sensor Project

![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)
![STM32](https://img.shields.io/badge/STM32-F103C8T6-blue)
![CubeIDE](https://img.shields.io/badge/IDE-STM32CubeIDE-darkblue)

## Video Demonstrations

https://github.com/user-attachments/assets/e8429c52-7080-4330-a7c4-9600e6ef411d

https://github.com/user-attachments/assets/36aa9a49-7e9d-4ebb-aacc-f32500a67af0

## Project Overview

This project demonstrates **complete bare-metal programming** on the STM32F103C8 "Blue Pill" by building a **multi-sensor data acquisition system**. It reads data from **DHT11 (temperature/humidity)** and **MPU6050 (accelerometer/gyroscope)** sensors, displays them on a **16x2 I2C LCD**, and outputs data via **UART** for PC monitoring. The system runs on a **precise 10ms control loop** that schedules all tasks at different frequencies.

### Hardware Components

| Component | Description |
|-----------|-------------|
| **STM32F103C8T6** | "Blue Pill" development board | 
| **DHT11 Sensor** | Temperature & Humidity sensor | 
| **MPU6050 Sensor** | 6-axis inertial measurement unit | 
| **LCD 16x2 with I2C** | Character display module | 
| **USB-to-Serial Converter** | CP2102 / CH340 / FTDI |
| **Push Button** | Two Leg Tactile switch | 

### Task Scheduling

The system uses a **10ms timer-based control loop** with independent counters for each task. TIMER3 is configured to drive the main control loop.

| Task | Frequency | Period | Execution |
|------|-----------|--------|-----------|
| **DHT11 Read** | 1 Hz | 1 second | Every 100 loops |
| **MPU6050 Read** | 20 Hz | 50 ms | Every 5 loops |
| **LCD Update** | 10 Hz | 100 ms | Every 10 loops |
| **UART Output** | 10 Hz | 100 ms | Every 10 loops |
| **Button Check** | Event-driven | On interrupt | EXTI + TIM4 debounce |

### Display Modes

The system features three display modes that can be cycled by pressing the button:

| Mode | Display Content | UART Output |
|------|-----------------|-------------|
| **Mode 0 (Default)** | Temperature & Humidity | Temperature and humidity data |
| **Mode 1** | Accelerometer (X, Y, Z) | Accelerometer values |
| **Mode 2** | Gyroscope (X, Y, Z) | Gyroscope values |

**Behavior:**
- On startup, the LCD shows **temperature and humidity** (Mode 0)
- UART continuously outputs **temperature and humidity data** in Mode 0
- **First button press** → LCD switches to **accelerometer data** (Mode 1)
- **Second button press** → LCD switches to **gyroscope data** (Mode 2)
- **Third button press** → Returns to **temperature and humidity** (Mode 0)
- UART output always matches whatever mode is currently displayed

### Timer Configuration

| Timer | Resolution | Purpose |
|-------|------------|---------|
| **TIMER1** | 1µs | DHT11 precise timing |
| **TIMER2** | 0.1ms | General delays |
| **TIMER3** | 0.1ms | **Control loop scheduler** (10ms base) |
| **TIMER4** | 0.1ms | Button debouncing (50ms) |

🔗 [View TIMER1 Initialization Source Code](https://github.com/rubin-khadka/STM32_DHT11_MPU6050_LCD/blob/main/Src/timer1.c) <br>
🔗 [View TIMER2 Initialization Source Code](https://github.com/rubin-khadka/STM32_DHT11_MPU6050_LCD/blob/main/Src/timer2.c) <br>
🔗 [View TIMER3 Initialization Source Code](https://github.com/rubin-khadka/STM32_DHT11_MPU6050_LCD/blob/main/Src/timer3.c) <br>
🔗 [View External Interrupt Button and TIMER4 Initialization Source Code](https://github.com/rubin-khadka/STM32_DHT11_MPU6050_LCD/blob/main/Src/button.c)

## Pin Configuration

| Peripheral | Pin | Connection | Notes |
|------------|-----|------------|-------|
| **DHT11** | PB0 | DATA | Built-in 1kΩ pull-up |
| | 5V | VCC | Power |
| | GND | GND | Common ground |
| **MPU6050** | PB8 | SCL | I2C clock |
| | PB9 | SDA | I2C data |
| | 5V | VCC | Power |
| | GND | GND | Common ground |
| **LCD 16x2 I2C** | PB8 | SCL | Shares I2C bus with MPU6050 |
| | PB9 | SDA | Shares I2C bus with MPU6050 |
| | 5V | VCC | Power |
| | GND | GND | Common ground |
| **UART** | PA9 | TX to USB-Serial RX | 115200 baud |
| | PA10 | RX to USB-Serial TX | Optional for commands |
| **Button** | PA0 | Mode select | Pull-up, falling edge trigger |

The LCD display and MPU6050 share the same I2C bus (PB8/SCL, PB9/SDA) with different addresses:

| Device | I2C Address (7-bit) | 8-bit Write | 8-bit Read |
|--------|---------------------|-------------|------------|
| **MPU6050** | 0x68 | 0xD0 | 0xD1 |
| **LCD Module** | 0x27 | 0x4E | 0x4F |

🔗 [View Custom Written I2C Driver Source Code](https://github.com/rubin-khadka/STM32_DHT11_MPU6050_LCD/blob/main/Src/i2c1.c) <br>
**Note:** The LCD module uses a PCF8574 I2C backpack. 

### Project Schematic

<img width="1197" height="526" alt="Schematic_diagram" src="https://github.com/user-attachments/assets/eef5449f-f22e-4f18-beb8-1e8a7fe16d54" />

*Schematic designed using EasyEDA*

The schematic shows the STM32F103C8T6 implementation including:
- **8MHz HSE clock circuit** (for 72MHz operation)
- **DHT11 sensor connection** (PB0)
- **MPU6050 sensor connection** (PB8/PB9 I2C bus)
- **LCD 16x2 I2C connection** (PB8/PB9 I2C bus)
- **UART interface** (PA9/PA10 for USB-to-Serial)
- **Push button** (PA0 for mode selection)

## MPU-6050 IMU Driver

The MPU6050 is a 6-axis inertial measurement unit that combines a 3-axis accelerometer and a 3-axis gyroscope.

### Key Features

| Parameter | Value |
|-----------|-------|
| **Interface** | I2C (shared bus with LCD) |
| **I2C Address** | 0x68 (7-bit) |
| **Accelerometer Range** | ±2g/±4g/±8g/±16g (configurable) |
| **Gyroscope Range** | ±250/±500/±1000/±2000 °/s (configurable) |
| **Data Output** | 16-bit raw values |

### Register Map

| Register | Address | Description |
|----------|---------|-------------|
| **WHO_AM_I** | 0x75 | Returns 0x68 for device identification |
| **PWR_MGMT_1** | 0x6B | Power management and device wake-up |
| **ACCEL_XOUT_H** | 0x3B | Accelerometer X-axis high byte |
| **GYRO_XOUT_H** | 0x43 | Gyroscope X-axis high byte |
| **TEMP_OUT_H** | 0x41 | Temperature sensor high byte |

### Implementation Details

- **Burst read** of all 14 data bytes in a single I2C transaction
- Data stored in global structure for access by other tasks
- Configurable measurement ranges via register writes
- Raw 16-bit values accessible for further processing

🔗 [View MPU6050 Driver Source Code](https://github.com/rubin-khadka/STM32_DHT11_MPU6050_LCD/blob/main/Src/mpu6050.c)

**Note:** The data received from the MPU6050 is **raw** and has not been processed or filtered. No sensor fusion algorithms (complementary/Kalman filters) are applied. The accelerometer and gyroscope values are provided directly as read from the sensor registers.

## DHT11 Sensor Driver

The DHT11 uses a **single-wire protocol** with precise timing:

| Phase | Duration | Description |
|-------|----------|-------------|
| **Start Signal** | 18ms LOW + 20µs HIGH | MCU wakes sensor |
| **Sensor Response** | 80µs LOW + 80µs HIGH | Sensor acknowledges |
| **Bit "0"** | 50µs LOW + 26-28µs HIGH | Logic 0 |
| **Bit "1"** | 50µs LOW + 70µs HIGH | Logic 1 |
| **Data Frame** | 40 bits | 5 bytes (humidity ×2 + temp ×2 + checksum) |

Instead of measuring pulse width, I used a **simpler approach** looking at datasheet:

For each bit:
1. Wait for line to go HIGH
2. Delay exactly 40µs
3. If line still HIGH → logic 1 <br>
   If line is LOW → logic 0

To ensure the timing is not interrupted, **interrupts are disabled** while communicating with the sensor. The checksum provided by the sensor is used to verify data integrity.

🔗 [View DHT11 Driver Source Code](https://github.com/rubin-khadka/STM32_DHT11_MPU6050_LCD/blob/main/Src/dht11.c)

## Lightweight String Formatting

Instead of using `sprintf()` (which is computationally heavy and increases code size), I created custom lightweight functions:

### General Utility Functions

| Function | Purpose |
|----------|---------|
| `itoa_16()` | Converts 16-bit integers to strings |
| `itoa_8()` | Converts 8-bit integers to strings |
| `format_value()` | Formats temperature/humidity with 1 decimal place |

### UART Output Formatting Functions

| Function | Purpose | Output Example |
|----------|---------|----------------|
| `format_reading()` | Builds complete temperature/humidity line | `Temp: 24.5C, Hum: 45.0%` |
| `format_accel()` | Formats accelerometer data for UART | `AX:123 AY:456 AZ:789` |
| `format_gyro()` | Formats gyroscope data for UART | `GX:12 GY:34 GZ:56` |

### LCD Display Functions

| Function | Purpose | Display Example |
|----------|---------|-----------------|
| `LCD_DisplayReading()` | Shows temperature & humidity on LCD | Line1: `TEMP: 24.5 C`<br>Line2: `HUMD: 45.0 %` |
| `LCD_DisplayAccel()` | Shows accelerometer data on LCD | Line1: `AX:123 AY:456`<br>Line2: `AZ:789` |
| `LCD_DisplayGyro()` | Shows gyroscope data on LCD | Line1: `GX:123 GY:456`<br>Line2: `GZ:789` |

🔗 [View Custom String Format Utility Source Code](https://github.com/rubin-khadka/STM32_DHT11_MPU6050_LCD/blob/main/Src/utils.c) <br>
🔗 [View LCD Driver Source Code](https://github.com/rubin-khadka/STM32_DHT11_MPU6050_LCD/blob/main/Src/lcd.c)

**Note**: Future versions of this project will implement standardized data output protocols such as **NMEA**.

## Getting Started

### Prerequisites

| Component | Details |
|-----------|---------|
| **MCU** | STM32F103C8T6 "Blue Pill" board |
| **Sensors** | DHT11 (temperature/humidity) + MPU6050 (accelerometer/gyroscope) |
| **Display** | 16x2 LCD with I2C backpack (PCF8574) |
| **Communication** | USB-to-Serial converter (CP2102/CH340/FTDI) |
| **Programmer** | ST-Link V2 |
| **IDE** | STM32CubeIDE v1.13.0+ |

### Installation

1. Clone the repository
```bash
git clone https://github.com/rubin-khadka/STM32_DHT11_MPU6050_LCD.git
```
2. Open in STM32CubeIDE
    - `File` → `Import...`
    - `General` → `Existing Projects into Workspace` → `Next`
    - Select the project directory
    - Click `Finish`    

3. Verify Project Settings
    - `Project` → `Properties` → `C/C++ Build` → `Settings`
    - `MCU GCC Compiler` → `Preprocessor`
    - Ensure `STM32F103xB` is defined
    - `MCU GCC Compiler` → `Include paths` Add the following paths
        - "~\STM32Cube_FW_F1_V1.8.0\Drivers\CMSIS\Core\Include"
        - "~\STM32Cube_FW_F1_V1.8.0\Drivers\CMSIS\Device\ST\STM32F1xx\Include"

4. Build & Flash
    - Build: `Ctrl+B`
    - Debug: `F11`
    - Run: `F8` (Resume)

## Resources
- [STM32F103 Datasheet](https://www.st.com/resource/en/datasheet/stm32f103c8.pdf)
- [STM32F103 Reference Manual](https://www.st.com/resource/en/reference_manual/rm0008-stm32f101xx-stm32f102xx-stm32f103xx-stm32f105xx-and-stm32f107xx-advanced-armbased-32bit-mcus-stmicroelectronics.pdf)
- [DHT11 Sensor Datasheet](https://www.mouser.com/datasheet/2/758/DHT11-Technical-Data-Sheet-Translated-Version-1143054.pdf)
- [MPU6050 Sensor Datasheet](https://invensense.tdk.com/wp-content/uploads/2015/02/MPU-6000-Datasheet1.pdf)
- [PCF8574 I2C Backpack Datasheet](https://www.ti.com/lit/ds/symlink/pcf8574.pdf)

## Project Status
- **Status**: Complete
- **Version**: v1.0
- **Last Updated**: February 2026

## Contact
**Rubin Khadka Chhetri**  
📧 rubin.khadka84@gmail.com <br>
🐙 GitHub: https://github.com/rubin-khadka
