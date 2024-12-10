# Dendrophone Buildroot Image

A musical instrument based on Raspberry Pi CM4.

## Hardware Requirements
- Raspberry Pi Compute Module 4 (2GB RAM)
- Waveshare 3.5" IPS LCD (480x320)
- MCP3002 ADC
- Cherry MX switches (x4)
- Analog joystick
- Various resistors and capacitors

## Directory Structure
- `src/` - Application source code
 - `display/` - Display driver and graphics
 - `main.cpp` - Main application
- `dendrophone-board/` - System configuration and overlay
 - `configs/` - Buildroot configuration
 - `overlay/` - System files and binaries
- `buildroot/` - Embedded Linux build system
- `scripts/` - Build and simulation tools

## Building

### Prerequisites
```bash
sudo apt install build-essential git cmake
```

### Build Steps
```bash
# Clone repository
git clone https://github.com/yourusername/dendrophone.git
cd dendrophone

# Initialize buildroot submodule
git submodule update --init

# Build system
./scripts/build_image.sh
```

###Simulation
```bash
renode scripts/simulate.resc
```

###Development
 - Application code goes in src/
 - System configurations in dendrophone-board/
 - Build artifacts appear in buildroot/output/
