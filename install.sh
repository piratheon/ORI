#!/bin/bash

# ORI Terminal Assistant Installation Script
# This script installs ORI Terminal Assistant and sets up shell integration

set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

echo -e "${BLUE}ORI Terminal Assistant v0.2 Installation Script${NC}"
echo "================================================"

# Check if we're in the right directory
if [ ! -f "CMakeLists.txt" ] || [ ! -f "src/main.cpp" ]; then
    echo -e "${RED}Error: Please run this script from the ORI-CPP directory${NC}"
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

# Install to system
echo -e "${YELLOW}Installing ORI Terminal Assistant to system...${NC}"

# Create directories
sudo mkdir -p /usr/local/bin
sudo mkdir -p /usr/local/share/ori
sudo mkdir -p /etc/ori

# Copy executable
sudo cp build/ori /usr/local/bin/ori
sudo chmod +x /usr/local/bin/ori

# Copy plugin database
if [ -d "plugins" ]; then
    sudo cp -r plugins/* /usr/local/share/ori/
fi

# Create configuration directory for user
mkdir -p ~/.config/ori

echo -e "${GREEN}Installation complete!${NC}"

# Setup shell integration
echo -e "${YELLOW}Setting up shell integration...${NC}"

# Detect shell
SHELL_NAME=$(basename "$SHELL")
SHELL_RC=""

case "$SHELL_NAME" in
    "bash")
        SHELL_RC="$HOME/.bashrc"
        ;;
    "zsh")
        SHELL_RC="$HOME/.zshrc"
        ;;
    "fish")
        SHELL_RC="$HOME/.config/fish/config.fish"
        ;;
    *)
        echo -e "${YELLOW}Warning: Unsupported shell '$SHELL_NAME'. You may need to manually add the alias.${NC}"
        ;;
esac

if [ -n "$SHELL_RC" ] && [ -f "$SHELL_RC" ]; then
    # Check if alias already exists
    if ! grep -q "alias ori=" "$SHELL_RC"; then
        echo "" >> "$SHELL_RC"
        echo "# ORI Terminal Assistant alias" >> "$SHELL_RC"
        echo "alias ori='/usr/local/bin/ori'" >> "$SHELL_RC"
        echo -e "${GREEN}Added 'ori' alias to $SHELL_RC${NC}"
    else
        echo -e "${YELLOW}Alias 'ori' already exists in $SHELL_RC${NC}"
    fi
fi

echo ""
echo -e "${GREEN}================================================"
echo -e "ORI Terminal Assistant v0.2 Installation Complete!"
echo -e "================================================${NC}"
echo ""
echo -e "${BLUE}Usage Examples:${NC}"
echo "  ori 'print current active username'"
echo "  ori print current active username"
echo "  ori --load-prompt /path/to/prompt.md"
echo "  ori --help"
echo ""
echo -e "${YELLOW}Note: You may need to restart your shell or run 'source $SHELL_RC' to use the 'ori' command.${NC}"
echo ""
echo -e "${BLUE}Configuration:${NC}"
echo "  Set your OpenRouter API key:"
echo "    export OPENROUTER_API_KEY='your-api-key-here'"
echo "  Or create ~/.config/ori/Openrouter_api_key.txt with your API key"
echo ""
