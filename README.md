# Buildroot Godot Kiosk Build System for Raspberry Pi (4)

This build system creates a minimal Linux image designed to run a Godot 4 application in kiosk mode on a Raspberry Pi CM4. The system boots directly into your Godot application using the Cage Wayland compositor and Vulkan 1.2.

I have found Godot to be a solid UI for Embedded Linux applications. This is due to the fact its compiled for the native system (over something like ReactJS/electron) and has a minimal foot print for how powerful it is. The GDExtension feature of Godot is also extremely helpful, as you can call native C++ code and libraries like GPIO pins.

## Using this for other Linux boards

You can use this with any other Linux board that supports Vulkan 1.2. Just change the defconfig. Whatever defconfig you use, make sure to copy over the stuff that was added for this intended kiosk functionality. 

## Technical Requirements

- Godot 4.x application compiled for ARM64 and Vulkan 1.2
- Raspberry Pi 4
- Git
- Basic bash environment

## Setup Steps

1. Clone the repository
   ```bash
   git clone https://github.com/corykacal/RaspberryPi-Godot-UI-Kiosk-Image godot-kiosk
   cd godot-kiosk
   ```

2. Initialize and update Buildroot submodule
   ```bash
   git submodule init
   git submodule update
   ```

3. Copy your Godot binary
   ```bash   
   # Copy your Godot binary
   cp path/to/your/godot-binary image/overlay/bin/godot_app
   ```

4. Build the system
   ```bash
   chmod +x build.sh
   ./build.sh
   ```

## Build Commands

```bash
# Show help and usage information
./build.sh --help

# Perform full build
./build.sh

# Clean RPi firmware and Linux builds
./build.sh clean

# Perform complete clean (equivalent to make clean)
./build.sh cleanall
```

## Directory Structure

```
.
├── buildroot/           # Buildroot submodule.
├── image/
│   ├── configs/        # Contains the defconfig and linux firmware config file.
│   ├── fragments/      # Linux Kernel Fragment config files.
│   └── overlay/        # System overlay. These files are copied (overlayed) to the Buildroot build directory that the image comes from. 
│       ├── bin/        # Place your Godot binary here.
│       └── etc/        # System configuration, inittab, program daemon. 
│           └── init.d/ # The program daemon that runs on start.
├── build.sh            # Main build script.
└── README.md           # This file.
```

## Customization

1. To use your own Godot application:
   - Ensure your Godot 4.x application is compiled for ARM64 and Vulkan 1.2
   - Copy your ARM64 Godot binary to `image/overlay/bin/`
   - Either name it `godot_app` or update the `GODOT_BINARY` variable in `build.sh`

2. The init script (`S99godot`) is automatically generated and will:
   - Wait for DRM to be ready
   - Set up the runtime directory
   - Launch your application using Cage

## Build Output

After a successful build, you'll find the output image binary in:
```
buildroot/output/images/Image
```

This will include the bootable SD card image for your Raspberry Pi CM4.

## Troubleshooting

 - If the build fails with missing binary:
   - Ensure your Godot binary is in `image/overlay/bin/`
   - Verify the binary name matches `GODOT_BINARY` in `build.sh`

 - If your Godot application fails to start:
   - Verify your application is compiled for ARM64
   - Ensure Vulkan 1.2 compatibility
   - Check the system logs for graphics driver issues

