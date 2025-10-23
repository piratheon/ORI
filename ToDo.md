# Ori Assistant Changelog

## Version 0.3

This version introduces major new features focused on making Ori a true agentic assistant capable of executing commands and operating non-interactively.

### ✅ Implemented Features & Fixes

- **Agentic Command Execution**
  - [x] The AI can now decide to execute shell commands to answer prompts.
  - [x] A special `[exec]command[/exec]` format is used by the AI to request execution.
  - [x] The application can parse and handle multiple `[exec]` tags in a single response.

- **Interactive & Non-Interactive Modes**
  - [x] In interactive mode, Ori will always ask for user confirmation (`y/n`) before running a command.
  - [x] Added a `-y` / `--yes` command-line flag to allow for non-interactive execution by pre-approving all commands.
  - [x] The agentic logic now works in both interactive mode and direct mode (e.g., `ori -y "..."`).

- **Core Enhancements**
  - [x] **Conversation History**: The assistant now remembers previous messages in an interactive session, allowing for conversational follow-ups.
  - [x] **Enhanced Error Handling**: The application now correctly parses JSON error responses from the OpenRouter API and displays a user-friendly message, including the error code and details.

- **Bug Fixes**
  - [x] Fixed a critical bug where the application would enter an infinite loop when receiving piped input (e.g., from `echo`).
  - [x] Corrected several compilation errors caused by faulty `replace` operations.

### 🚧 Future Work (Post v0.3)

- [ ] Add shell completion scripts (bash, zsh, fish).
- [ ] Implement more advanced command-line options (`--model`, `--temperature`, etc.).
- [ ] Enhance the plugin system with a remote plugin store and dependency management.
- [ ] Improve interactive mode with multi-line input, command history, and syntax highlighting.
- [ ] Add a caching system to reduce API calls for repeated queries.