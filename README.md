# Automated Warehouse Crane

An embedded firmware project for controlling an automated warehouse crane system using an STM32F411 microcontroller. The system features precise servo control for vertical and horizontal crane movement with real-time communication capabilities.

## Overview

This project implements a warehouse crane control system with the following features:

- **Real-time Operating System**: FreeRTOS-based task management for concurrent operations
- **Servo Control**: PWM-based control for vertical and horizontal crane positioning
- **Host Communication**: USART-based serial communication with a host PC
- **Modular Architecture**: Layered software design (L1-L4) for maintainability

## Hardware Specifications

- **Microcontroller**: STM32F411RETx (ARM Cortex-M4)
  - 100 MHz maximum clock frequency
  - FPU support (floating-point unit)
  - 512 KB Flash, 128 KB SRAM
- **Peripherals Used**:
  - TIM1: PWM generation for servo control (50 Hz)
  - USART2: Serial communication with host PC
  - FreeRTOS: Real-time task scheduling

## Software Architecture

The project follows a layered architecture:

```
User/
├── L1/    # Hardware drivers (PWM, USART)
├── L2/    # Communication datalink layer
├── L3/    # (Reserved for higher-level logic)
└── L4/    # (Reserved for application layer)
```

### Key Components

- **PWM_Driver**: Controls servo motors with 50 Hz PWM signals (1-2 ms pulse width)
- **USART_Driver**: Manages serial communication queues and data transmission
- **Comm_Datalink**: Handles communication protocol and data framing
- **FreeRTOS Tasks**:
  - `HostPC_RX_Task`: Receives and processes commands from host PC
  - `PWM_Timer_Task`: Controls servo positioning based on queue messages
  - `Debug_Task`: Development and testing support

## Prerequisites

To build this project, you need:

- **CMake** (version 3.22 or higher)
- **Ninja** build system
- **ARM GCC Toolchain** (`arm-none-eabi-gcc`)
  - Must be available in your system PATH
  - Download from [ARM Developer](https://developer.arm.com/tools-and-software/open-source-software/developer-tools/gnu-toolchain/gnu-rm)
- **Git** (for cloning the repository)

### Installing Prerequisites

#### Ubuntu/Debian
```bash
sudo apt-get update
sudo apt-get install cmake ninja-build gcc-arm-none-eabi
```

#### macOS (using Homebrew)
```bash
brew install cmake ninja
brew install --cask gcc-arm-embedded
```

#### Windows
1. Install CMake from [cmake.org](https://cmake.org/download/)
2. Install Ninja from [GitHub releases](https://github.com/ninja-build/ninja/releases)
3. Install ARM GCC from [ARM Developer](https://developer.arm.com/tools-and-software/open-source-software/developer-tools/gnu-toolchain/gnu-rm)
4. Add all tools to your system PATH

## Building with CMake

This project uses **CMake** as its build system (not STM32CubeIDE). The build process is straightforward:

### Quick Build

```bash
# Configure the project (Debug build)
cmake --preset Debug

# Build the project
cmake --build --preset Debug
```

### Build Options

The project supports two build configurations:

#### Debug Build (default)
```bash
cmake --preset Debug
cmake --build --preset Debug
```
- Optimization: `-O0` (no optimization)
- Debug symbols: `-g3` (maximum debug info)
- Best for development and debugging

#### Release Build
```bash
cmake --preset Release
cmake --build --preset Release
```
- Optimization: `-Os` (optimize for size)
- Debug symbols: `-g0` (no debug info)
- Best for production deployment

### Build Output

The build process generates:
- `Automated_Warehouse_Crane.elf`: Executable binary
- `Automated_Warehouse_Crane.map`: Memory map file
- Build artifacts in `build/Debug/` or `build/Release/`

### Advanced Build Options

#### Clean Build
```bash
# Remove build directory and rebuild
rm -rf build/
cmake --preset Debug
cmake --build --preset Debug
```

#### Verbose Build
```bash
cmake --build --preset Debug --verbose
```

## Flashing the Firmware

After building, you can flash the firmware to the STM32F411:

### Using STM32CubeProgrammer
```bash
STM32_Programmer_CLI -c port=SWD -w build/Debug/Automated_Warehouse_Crane.elf -rst
```

### Using OpenOCD
```bash
openocd -f interface/stlink.cfg -f target/stm32f4x.cfg \
        -c "program build/Debug/Automated_Warehouse_Crane.elf verify reset exit"
```

### Using GDB
```bash
arm-none-eabi-gdb build/Debug/Automated_Warehouse_Crane.elf
(gdb) target remote localhost:3333
(gdb) load
(gdb) monitor reset halt
(gdb) continue
```

## Project Structure

```
Automated_Warehouse_Crane/
├── CMakeLists.txt              # Main CMake configuration
├── CMakePresets.json           # Build presets (Debug/Release)
├── cmake/
│   ├── gcc-arm-none-eabi.cmake # ARM GCC toolchain configuration
│   └── stm32cubemx/            # STM32 HAL and startup files
├── Core/                       # STM32 generated core files
│   ├── Inc/                    # Core headers
│   └── Src/                    # Core sources
├── User/                       # User application code
│   ├── Inc/                    # User headers
│   │   ├── L1/                 # Hardware driver headers
│   │   ├── L2/                 # Datalink layer headers
│   │   └── L3/                 # Application layer headers
│   └── Src/                    # User sources
│       ├── L1/                 # Hardware driver implementations
│       ├── L2/                 # Datalink layer implementations
│       └── user_main.c         # Main application entry
├── Drivers/                    # STM32 HAL, CMSIS, FreeRTOS
├── Middlewares/                # Third-party middleware
└── STM32F411XX_FLASH.ld        # Linker script
```

## Usage

### Serial Communication

The crane system communicates with a host PC via USART2:
- **Baud Rate**: Configured in STM32CubeMX project
- **Data Format**: 8N1 (8 data bits, no parity, 1 stop bit)
- **Interface**: USB-to-serial adapter or ST-Link virtual COM port

### Servo Control

The system controls two servo motors:
- **Channel 1 (PA8)**: Vertical crane movement
- **Channel 2 (PA9)**: Horizontal crane movement
- **PWM Frequency**: 50 Hz
- **Pulse Width Range**: 1 ms (0%) to 2 ms (100%)
  - 1.0 ms = 0° position
  - 1.5 ms = 90° position (center)
  - 2.0 ms = 180° position

### Sending Commands

Commands can be sent via serial terminal to control the crane. The system processes incoming characters through a FreeRTOS queue for thread-safe operation.

## Development

### Adding New Features

1. Place hardware drivers in `User/Src/L1/` and `User/Inc/L1/`
2. Place protocol/datalink code in `User/Src/L2/` and `User/Inc/L2/`
3. Place application logic in `User/Src/L3/` and `User/Inc/L3/`
4. Update `CMakeLists.txt` to include new source files
5. Rebuild using CMake

### Modifying STM32 Configuration

The project uses STM32CubeMX for peripheral initialization:
1. Open `Automated_Warehouse_Crane.ioc` in STM32CubeMX
2. Modify peripherals, clocks, or pin assignments
3. Regenerate code (CMake build files will be preserved)
4. Rebuild using CMake

## Debugging

### VSCode Integration

The project includes VSCode configuration for debugging:
1. Install Cortex-Debug extension
2. Connect ST-Link debugger
3. Open the project in VSCode
4. Use F5 to start debugging

### Serial Output

Debug messages are printed via USART and can be monitored:
```bash
# Linux/macOS
screen /dev/ttyUSB0 115200

# Windows (using PuTTY or similar)
# Connect to appropriate COM port at 115200 baud
```

## Troubleshooting

### Build Issues

**Problem**: `arm-none-eabi-gcc: command not found`
- **Solution**: Install ARM GCC toolchain and add to PATH

**Problem**: CMake cannot find Ninja
- **Solution**: Install Ninja build system and ensure it's in PATH

**Problem**: Linker errors about undefined references
- **Solution**: Check that all required source files are listed in `CMakeLists.txt`

### Flash Issues

**Problem**: Cannot connect to target
- **Solution**: Check ST-Link connection, try holding RESET while connecting

**Problem**: Flash verification failed
- **Solution**: Ensure correct device is selected, try erasing flash first

## Contributing

When contributing to this project:
1. Follow the existing code structure and layered architecture
2. Add appropriate header comments to new files
3. Update CMakeLists.txt for new source files
4. Test changes on actual hardware before committing
5. Keep commits focused and well-documented

## License

This project is developed for educational and research purposes.

## Authors

- Zachary Fraser (Project Maintainer)
- Contributors: See Git commit history

## Acknowledgments

- STMicroelectronics for STM32 HAL libraries
- ARM for CMSIS and FreeRTOS
- The open-source embedded systems community
