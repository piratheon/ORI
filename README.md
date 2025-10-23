```
    ███████    ███████████   █████            ███████████ █████  █████ █████
  ███▒▒▒▒▒███ ▒▒███▒▒▒▒▒███ ▒▒███            ▒█▒▒▒███▒▒▒█▒▒███  ▒▒███ ▒▒███ 
 ███     ▒▒███ ▒███    ▒███  ▒███            ▒   ▒███  ▒  ▒███   ▒███  ▒███ 
▒███      ▒███ ▒██████████   ▒███  ██████████    ▒███     ▒███   ▒███  ▒███ 
▒███      ▒███ ▒███▒▒▒▒▒███  ▒███ ▒▒▒▒▒▒▒▒▒▒     ▒███     ▒███   ▒███  ▒███ 
▒▒███     ███  ▒███    ▒███  ▒███                ▒███     ▒███   ▒███  ▒███ 
 ▒▒▒███████▒   █████   █████ █████               █████    ▒▒████████   █████
   ▒▒▒▒▒▒▒    ▒▒▒▒▒   ▒▒▒▒▒ ▒▒▒▒▒               ▒▒▒▒▒      ▒▒▒▒▒▒▒▒   ▒▒▒▒▒
```

Ori Assistant is a Linux-based TUI (Text User Interface) AI assistant that integrates with OpenRouter API, offering powerful plugins, an advanced command-line interface, and modern UI capabilities. It is designed for both beginner and advanced users, with plugin support and a modular structure for easy extensions.

## Features

- **Agentic Command Execution**: The AI can intelligently decide to run shell commands to answer questions or perform tasks. For safety, it always asks for user confirmation in interactive mode.
- **TUI AI Assistant**: Text-based interface for interacting with AI models
- **OpenRouter API Integration**: Connects to various AI models through OpenRouter
- **Default Model**: Deepseek R1 Free as the default AI model
- **Plugin System**: Extensible architecture with plugin support
- **Orpm Plugin Manager**: Built-in package manager for plugins with JSON-based plugin database
- **Secure API Key Handling**: Multiple options for secure API key storage
- **Session Context**: Remembers conversation history in interactive mode
- **System Prompt**: Loads a system prompt from `ori-system-prompt.md` to set the AI's persona
- **Cross-Platform Build System**: CMake-based build system with dependency management

## Current Implementation Status

This project is currently at version 0.3.

This is a prototype implementation that demonstrates the core architecture and functionality. The AI responses are currently simulated with placeholder text, but the structure is in place for integrating with the actual OpenRouter API.

### Core Components

1. **OriAssistant**: Main TUI interface with command processing
2. **OpenRouterAPI**: API interface (placeholder implementation)
3. **Orpm**: Plugin manager with full install/remove/search/list functionality
4. **Plugin Database**: JSON-based plugin metadata system

## Installation

### Prerequisites

- C++14 compatible compiler (GCC 5.0+ or Clang 3.4+)
- CMake 3.10+
- OpenRouter API key (for production use)

### Building from Source

```bash
# Clone the repository
git clone https://github.com/chafiq-dev/ORI.git
cd ORI

# Build using the provided script
./build.sh

# Or build manually
mkdir build
cd build
cmake ..
make
```

The build script will automatically handle the build process and create the executable in the `build/` directory.

## Configuration

### API Key Setup

Ori Assistant can load your OpenRouter API key from multiple sources:

1. **Environment Variable**: Set `OPENROUTER_API_KEY` in your shell profile
   ```bash
   export OPENROUTER_API_KEY="your-api-key-here"
   ```

2. **Configuration File**: Create a file named `Openrouter_api_key.txt` in:
   - Current directory
   - `~/.config/ori/` directory

## Usage

### Running the Assistant

```bash
# Start the interactive assistant (from build directory)
./build/ori

# Show help
./build/ori --help

# Show version
./build/ori --version

# Run a non-interactive command, auto-approving any shell execution
./build/ori -y "install nmap for me"
```

In the interactive mode:
- Type any query to get a response
- Type `help` to see available commands
- Type `quit` or `exit` to exit

### Using the Plugin Manager (orpm)

```bash
# List available plugins
./build/ori --orpm --orpm-list

# List installed plugins
./build/ori --orpm --orpm-list-installed

# Search for plugins
./build/ori --orpm --orpm-search "voice"

# Install a plugin (placeholder implementation)
./build/ori --orpm --orpm-install "voice-chat"

# Remove a plugin (placeholder implementation)
./build/ori --orpm --orpm-remove "voice-chat"
```

## Plugin System

### Plugin Locations

- **User Plugins**: `~/.local/share/Ori/plugins/`
- **System Plugins**: `/usr/share/Ori/plugins/` (requires sudo access)

### Plugin Database

Plugins are managed through a JSON database (`plugins/plugin_database.json`) that contains:
- Plugin name and version
- Description and developer information
- Git repository URL
- Download URL for plugin archives

### Available Plugins (Demo)

1. **voice-chat**: Voice interaction plugin
2. **shell-access**: Shell command execution with safety features
3. **custom-prompt**: Customizable shell prompt
4. **file-access**: File system access capabilities
5. **gtk4-ui**: Graphical user interface (planned)
6. **text-editor**: Integrated text editor (planned)

## Security Considerations

- API keys are loaded securely from environment variables or configuration files
- Plugins are sandboxed (planned implementation)
- User and system plugin directories are separated for security
- Plugin installation requires explicit user action

## Development

### Project Structure

```
ORI/
├── build.sh                 # Build script
├── CMakeLists.txt           # CMake configuration
├── include/                 # Header files
│   ├── ori_core.h           # Core assistant interface
│   └── orpm.h               # Plugin manager interface
├── plugins/                 # Plugin resources
│   └── plugin_database.json # Plugin metadata
├── src/                     # Source code
│   ├── main.cpp             # Entry point
│   ├── core/                # Core assistant implementation
│   │   └── ori_core.cpp
│   └── plugins/             # Plugin manager implementation
│       └── orpm.cpp
├── README.md                # This file
└── LICENSE                  # License information
```

### Core Components

1. **OriAssistant Class**: Main class for the TUI interface
   - Handles user input and command processing
   - Manages the main application loop
   - Integrates with OpenRouterAPI for responses

2. **OpenRouterAPI Class**: Handles communication with OpenRouter API
   - Manages API key loading and storage
   - Processes queries and returns responses
   - Supports multiple AI models

3. **Orpm Class**: Plugin manager for installing/removing plugins
   - Manages plugin database and metadata
   - Handles plugin installation and removal
   - Searches and lists available plugins

### Building and Testing

```bash
# Build the project
./build.sh

# Test the assistant
echo -e "hello\nexit" | ./build/ori

# Test the plugin manager
./build/ori --orpm --orpm-list
```

## Future Enhancements

### Core Features
- Full OpenRouter API integration with real AI responses
- Google Gemini 2.0 integration for complex queries
- Advanced command-line flag support
- Configuration file support

### Plugin System
- Plugin Store implementation with remote repository support
- Dependency management for plugins
- Plugin verification and sandboxing
- Automatic plugin updates

### Security Features
- GNOME Keyring/KWallet integration for API key storage
- Enhanced plugin signature verification
- Improved dangerous command blocking
- Network access controls

### UI/UX Improvements
- GTK4 UI plugin implementation
- Customizable themes and color schemes
- Autocompletion for commands and plugins
- Session history and context management

## License

This project is licensed under the GNU GPL-3.0 license - see the [LICENSE](LICENSE) file for details.

## Contributing

Contributions are welcome! Please feel free to submit pull requests or open issues for bugs and feature requests.
