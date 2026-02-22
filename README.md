# STM32 DHT11 MPU6050 LCD Multi-Sensor Project

![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)
![STM32](https://img.shields.io/badge/STM32-F103C8T6-blue)
![CubeIDE](https://img.shields.io/badge/IDE-STM32CubeIDE-darkblue)

## Video Demonstrations


## Project Overview

This project demonstrates **complete bare-metal programming** on the STM32F103C8 "Blue Pill" by building a **multi-sensor data acquisition system**. It reads data from **DHT11 (temperature/humidity)** and **MPU6050 (accelerometer/gyroscope)** sensors, displays them on a 16x2 I2C LCD, and outputs data via UART for PC monitoring. The system runs on a **precise 10ms control loop** that schedules all tasks at different frequencies.

### Hardware Components

| Component | Description |
|-----------|-------------|
| **STM32F103C8T6** | "Blue Pill" development board | 
| **DHT11 Sensor** | Temperature & Humidity sensor | 
| **MPU6050 Sensor** | 6-axis motion tracking device | 
| **LCD 16x2 with I2C** | Character display module | 
| **USB-to-Serial Converter** | CP2102 / CH340 / FTDI |
| **Push Button** | Tactile switch | 

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

**Note:** The LCD module uses a PCF8574 I2C backpack.

### Project Schematic

## MPU6050 IMU Driver

## DHT11 Sensor Driver

## Timer Configuration

| Timer | Resolution | Purpose |
|-------|------------|---------|
| **TIM1** | 1µs | DHT11 precise timing |
| **TIM2** | 0.1ms | General delays |
| **TIM3** | 0.1ms | **Control loop scheduler** (10ms base) |
| **TIM4** | 0.1ms | Button debouncing (50ms) |