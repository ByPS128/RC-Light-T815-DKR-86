# RC-Light Controller for T815-DKR-86

## Introduction

RC-Light Controller is a sophisticated system for controlling the lighting of RC models, specifically designed for the 1:12 scale Tatra 815 1986 DAKAR model. This project combines hardware and software solutions to create a realistic and fully functional lighting system for RC models.

## Prerequisites
The control system assumes the use of an RC pistol transmitter with a steering wheel and at least 3 channels.
1. Channel for steering
2. Channel for throttle/brake
3. Channel of button type or a lever with two states (e.g., start/stop, on/off)

For development purposes, I used an inexpensive RC transmitter: TURBO 2.4GHz 91803G-VT 3CH RC Transmitter.

## Outline

1. [Feature Overview](#feature-overview)
2. [System Architecture](#system-architecture)
3. [Main Components](#main-components)
4. [Configuration and Customization](#configuration-and-customization)
5. [Optimization and Efficiency](#optimization-and-efficiency)
6. [Hardware Connection](#hardware-connection)
7. [Installation and Usage](#installation-and-usage)
8. [Project Structure and File Description](#project-structure-and-file-description)

## Feature Overview

- Multiple lighting modes including daytime running lights in 3 levels, long-distance lights
- Dynamic brightness control
- Realistic brake and reverse lights
- Calibration of RC channels for precise control
- Signal loss detection and indication
- Programmable flashing sequences for various system states
- Powered by the model's battery pack, with the option to connect to an independent power source

- Turn signals - this feature is not implemented because I don't need turn signals for the Dakar model. However, it's not difficult to program them. With the architecture I've designed, it's straightforward. If you're reading this and want turn signals but Arduino or C++ isn't your forte, write to me and I'll program it for you.

## System Architecture

The system is built on the Arduino platform and uses an object-oriented approach for a modular and extensible design. Key classes include:

- `MainApp`: Central control class coordinating all components
- `LightsController`: Manages various lighting modes
- `RCChannel`: Processes input signals from the RC receiver
- `CalibrationManager`: Ensures accurate calibration of RC channels
- `LedBlinker`: Provides a flexible interface for programmable LED sequences

## Main Classes

### MainApp

This class serves as the main entry point of the application. It initializes all other components, processes button inputs, and coordinates the overall system behavior.

### LightsController

Manages various lighting modes and controls individual LEDs. Supports smooth transitions between modes and responds to braking and reversing.

### RCChannel

Processes input PWM signals from the RC receiver, provides calibrated values for precise control.

### CalibrationManager

Allows calibration of RC channels for optimal performance. Stores calibration data in EEPROM for permanent retention.

### LedBlinker

A flexible system for creating complex flashing sequences, used to indicate various system states.

## Configuration and Customization

The system can be easily customized by modifying constants in the `AppConstants.h` file. Key configurable parameters include:

- Pin mapping for various functions
- Timing and threshold values for button detection
- Calibration constants for RC signals

To customize lighting behavior, the `LightsController` class can be modified, especially the `LIGHT_MATRIX` which defines various lighting modes.

## Optimization and Efficiency

The project uses several techniques to optimize memory usage and performance:

- Use of `PROGMEM` for storing constants in flash memory
- Efficient use of EEPROM for storing calibration data
- Optimized timing in the main loop to reduce power consumption
- Modular design allowing easy disabling of unused features

## Hardware Connection

The project includes a wiring diagram created in KiCad, providing a detailed view of the system's hardware configuration. The diagram includes the connection of Arduino Nano, RC receiver, LEDs, and other components.

I used the following key components to achieve the goal:

- Arduino Nano - main control unit
- Isolation module with optocoupler - detects motor rotation direction, necessary for movement direction detection
- PWM MOSFET module - creates PWM modulated signal on a separate circuit
- Step-down converter to 5V - powers Arduino and other peripherals
- LEDs .. describe types and properties
- Resistors .. describe quantities and values
- Capacitors .. describe quantities and values
- Transistors .. describe quantities and values
- Other components .. describe quantities and values

## Installation and Usage

1. Upload the code to Arduino Nano
2. Connect the hardware according to the provided diagram
3. Perform RC channel calibration using the built-in calibration mode
4. The system is ready for use - control the lighting using the RC transmitter

For detailed installation and usage instructions, see [documentation](./docs/USAGE.md).

---

This project represents a comprehensive and professional solution for controlling RC model lighting. Thanks to its modular design, efficient memory management, and flexible configuration, the RC-Light Controller is an ideal platform for modelers seeking realistic lighting for their RC models.

## Project Structure and File Description

### RC-Light-T815-DKR-86.ino
The main Arduino project file. Contains `setup()` and `loop()` functions that initialize and control the entire application. Creates an instance of `MainApp` and calls its methods for initialization and updating.

### AppConstants.h
File containing important constants used throughout the application. Defines pin mapping, EEPROM addresses, limits for RC signal validation, and other configuration values. Centralizing these constants facilitates maintenance and customization of the application.

### MainApp.h and MainApp.cpp
Defines the `MainApp` class, which serves as the central control unit for the entire application. Coordinates all other components, processes button inputs, and controls overall system behavior. Implements interfaces for processing button events and LED blinkers.

### RCChannel.h and RCChannel.cpp
The `RCChannel` class represents one channel of the RC receiver. Processes input PWM signals, performs calibration, and provides methods for obtaining current channel values. It is crucial for precise model control.

### CalibrationManager.h and CalibrationManager.cpp
`CalibrationManager` manages the RC channel calibration process. Allows saving and loading calibration data from EEPROM, ensuring that calibration is preserved even after power off.

### LightsController.h and LightsController.cpp
`LightsController` controls various lighting modes of the model. Manages switching between modes, controls individual LEDs, and responds to vehicle state (braking, reversing). Implements logic for various light configurations.

### ButtonBase.h and ButtonBase.cpp
The base class `ButtonBase` provides general functionality for processing button events. Implements detection of various press types (short, long, double) and a system for registering and notifying event subscribers.

### DigitalPullUpButton.h and DigitalPullUpButton.cpp
`DigitalPullUpButton` is a specialized class for working with digital buttons using Arduino's internal pull-up resistors. Inherits from `ButtonBase` and implements specific logic for detecting button presses.

### RcPwmButton.h and RcPwmButton.cpp
`RcPwmButton` is a class derived from `ButtonBase` that interprets PWM signal from the RC receiver as a button state. Allows using an RC channel as a virtual button.

### LedBlinker.h and LedBlinker.cpp
The `LedBlinker` class provides a flexible system for creating various LED blinking sequences. It is used to indicate various system states, such as calibration or signal loss.

### RCSteeringHandler.h and RCSteeringHandler.cpp
`RCSteeringHandler` processes input signals for steering wheel tracking. Implements logic for mapping input values to output actions, including LED brightness setting.

### RCThrottleHandler.h and RCThrottleHandler.cpp
`RCThrottleHandler` processes input signals for tracking the position of the throttle/brake lever. Detects states such as braking and provides this data to other components.

### SignalValidator.h and SignalValidator.cpp
The `SignalValidator` class checks the validity of RC signals. Verifies signal presence and pulse width validity for all RC channels, ensuring control reliability.

### NoiseFilter.h and NoiseFilter.cpp
`NoiseFilter` implements an algorithm for filtering noise in digital signals. Helps eliminate false triggers caused by electrical interference.

### cli.bat
A batch file for Windows that facilitates project compilation using Arduino CLI. Installs necessary libraries and runs compilation for Arduino Nano including cache clearing.

Each of these files plays an important role in the overall architecture of the project, contributing to its modularity, extensibility, and easy maintenance. Together, they create a robust system for controlling RC model lighting with advanced features such as calibration, signal loss detection, and flexible programming of light sequences.
