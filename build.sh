#!/bin/bash

# Configuration
GODOT_BINARY="godot_app"
BUILDROOT_DIR="buildroot"
OVERLAY_BIN_DIR="image/overlay/bin"
CONFIG_SOURCE="image/configs/cm4_godot_kiosk_defconfig"
CONFIG_DEST="configs/cm4_godot_kiosk_defconfig"
INIT_SCRIPT="image/overlay/etc/init.d/S99godot-app"

# Function to display help
show_help() {
    cat << EOF
Buildroot Godot Kiosk Build Script

Usage: $(basename "$0") [OPTION]

Options:
  (no option)  Perform full build process
  clean        Clean RPi firmware and Linux builds (make rpi-firmware-dirclean & linux-dirclean)
  cleanall     Perform complete clean (make clean)
  --help       Display this help message

Requirements:
  - Godot binary must be present at ${OVERLAY_BIN_DIR}/${GODOT_BINARY}
  - Config file must exist at ${CONFIG_SOURCE}
  - Buildroot directory must be present at ${BUILDROOT_DIR}

Build Process:
  1. Verifies required files and directories
  2. Copies configuration files
  3. Generates init.d script
  4. Runs buildroot make commands
EOF
}

# Function to check if a file exists
check_file() {
    if [ ! -f "$1" ]; then
        echo "Error: Required file not found: $1"
        exit 1
    fi
}

# Function to check if a directory exists
check_dir() {
    if [ ! -d "$1" ]; then
        echo "Error: Required directory not found: $1"
        exit 1
    fi
}

# Function to perform the main build
do_build() {
    # Verify required directories exist
    check_dir "${BUILDROOT_DIR}"
    check_dir "${OVERLAY_BIN_DIR}"

    # Check for Godot binary
    check_file "${OVERLAY_BIN_DIR}/${GODOT_BINARY}"
    echo "Found Godot binary: ${OVERLAY_BIN_DIR}/${GODOT_BINARY}"

    # Check for config file
    check_file "${CONFIG_SOURCE}"
    echo "Found config file: ${CONFIG_SOURCE}"

    # Copy config file to buildroot
    echo "Copying config file to buildroot..."
    cp "${CONFIG_SOURCE}" "${BUILDROOT_DIR}/${CONFIG_DEST}" || {
        echo "Error: Failed to copy config file"
        exit 1
    }

    # Generate init.d script
    echo "Generating init.d script..."
    cat > "${INIT_SCRIPT}" << EOF
#!/bin/sh

case "\$1" in
  start)
    # Wait for DRM to be ready
    while [ ! -e /dev/dri/card0 ]; do sleep 1; done

    # Setup runtime directory
    mkdir -p /tmp/runtime-root
    chmod 700 /tmp/runtime-root

    # Export required variables
    export XDG_RUNTIME_DIR=/tmp/runtime-root

    # Launch cage in foreground
    /usr/bin/cage -- /bin/${GODOT_BINARY}
    ;;

  stop)
    killall cage
    ;;

  restart)
    \$0 stop
    \$0 start
    ;;

  *)
    echo "Usage: \$0 {start|stop|restart}"
    exit 1
esac
EOF

    chmod +x "${INIT_SCRIPT}"

    # Build
    echo "Starting build process..."
    cd "${BUILDROOT_DIR}" || {
        echo "Error: Failed to change to buildroot directory"
        exit 1
    }

    echo "Running make cm4_godot_kiosk_defconfig..."
    make cm4_godot_kiosk_defconfig || {
        echo "Error: Failed to configure buildroot"
        exit 1
    }

    echo "Running make..."
    make || {
        echo "Error: Build failed"
        exit 1
    }

    echo "Build completed successfully!"
}

# Function to perform targeted clean
do_clean() {
    cd "${BUILDROOT_DIR}" || {
        echo "Error: Failed to change to buildroot directory"
        exit 1
    }
    echo "Cleaning RPi firmware and Linux..."
    make rpi-firmware-dirclean
    make linux-dirclean
    echo "Clean completed!"
    cd - > /dev/null
}

# Function to perform full clean
do_cleanall() {
    cd "${BUILDROOT_DIR}" || {
        echo "Error: Failed to change to buildroot directory"
        exit 1
    }
    echo "Performing full clean..."
    make clean
    echo "Full clean completed!"
    cd - > /dev/null
}

# Command line argument handling
case "$1" in
    clean)
        do_clean
        ;;
    cleanall)
        do_cleanall
        ;;
    --help|-h)
        show_help
        ;;
    "")
        do_build
        ;;
    *)
        echo "Error: Unknown option '$1'"
        echo "Use '$(basename "$0") --help' for usage information"
        exit 1
        ;;
esac