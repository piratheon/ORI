#!/bin/bash

# ORI Terminal Assistant Installation Script
# This script installs ORI Terminal Assistant to /usr/share/ori and creates a symlink in /usr/bin

set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

echo -e "${BLUE}ORI Terminal Assistant v0.4 Installation Script${NC}"
echo "This script will prompt for your password to install files into /usr."
echo "================================================"

# Check if we're in the right directory
if [ ! -f "CMakeLists.txt" ] || [ ! -f "src/main.cpp" ]; then
    echo -e "${RED}Error: Please run this script from the project's root directory${NC}"
    exit 1
fi

# Build the project
echo -e "${YELLOW}Building ORI Terminal Assistant...${NC}"
if [ -f "build.sh" ]; then
    ./build.sh
else
    mkdir -p build
    cd build
    cmake ..
    make -j$(nproc)
    cd ..
fi

# Check if build was successful
if [ ! -f "build/ori" ]; then
    echo -e "${RED}Error: Build failed. Please check the build output above.${NC}"
    exit 1
fi

echo -e "${GREEN}Build successful!${NC}"

# --- Installation ---
INSTALL_DIR="/usr/share/ori"
BIN_DIR="/usr/bin"

echo -e "${YELLOW}Installing ORI to $INSTALL_DIR...${NC}"

# Create installation directory
sudo mkdir -p "$INSTALL_DIR"

# Copy executable and prompt file
sudo cp build/ori "$INSTALL_DIR/ori"

# Copy plugin database
if [ -d "plugins" ]; then
    sudo cp -r plugins/* "$INSTALL_DIR/"
fi

# Create symbolic link
echo -e "${YELLOW}Creating symbolic link in $BIN_DIR...${NC}"
sudo ln -sf "$INSTALL_DIR/ori" "$BIN_DIR/ori"
install -D orpm /usr/bin/orpm

# Create configuration directory for user (as the user, not root)
echo -e "${YELLOW}Creating user configuration directory...${NC}"
mkdir -p "$HOME/.config/ori"

echo ""
echo -e "${GREEN}================================================"
echo -e "ORI Terminal Assistant Installation Complete!${NC}"
echo -e "================================================${NC}"
echo ""
echo -e "${BLUE}Usage:${NC}"
ori --help
