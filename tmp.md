## Release Notes

This release focuses on improving the user experience with a more intuitive command structure, enhanced security for API keys, and a more robust update process.

### ✅ Highlights
- **Slash Commands**: Internal commands are now prefixed with a forward slash (e.g., `/help`, `/quit`).
- **Secure API Key Handling**: API key is encrypted and stored in `~/.config/ori/key` using the motherboard's fingerprint for added security.
- **`/clear` Command**: A `/clear` command has been added to clear the terminal screen.
- **Update Check**: The assistant now silently checks for updates on startup and provides a `--check-for-updates` flag for manual checks. The update process now downloads the pre-compiled binary.
- **`orpm` Script**: `orpm` is now a separate bash script that wraps the `ori --orpm` command.

### Implemented Features & Fixes

#### Command Handling
- Internal commands now require a `/` prefix in interactive mode (e.g., `/help`, `/quit`). This provides a clear distinction between commands and prompts.
- Added a `/clear` command to clear the terminal screen.

#### Security
- The OpenRouter API key is now encrypted using the motherboard's serial number and stored in `~/.config/ori/key`.
- The user is prompted for the key on the first run, and it is automatically encrypted and saved.
- If the key is invalid or the motherboard has been changed, the user is prompted for a new key.

#### Update Process
- The update check is now performed silently in the background on startup.
- A `--check-for-updates` flag has been added to manually trigger an update check.
- The update process now downloads the pre-compiled binary from the GitHub releases page, making it faster and more reliable.

#### Plugin Manager
- The `orpm` plugin manager is now a separate bash script that can be called directly from the command line.
- The `orpm` script wraps the `ori --orpm` command, providing a more convenient way to manage plugins.

### Upgrade Notes
- Users will be prompted to enter their OpenRouter API key on the first run after upgrading.
- The `orpm` command should now be used instead of `ori --orpm` to manage plugins.
- The `help` command is now `/help`.
- The `quit` command is now `/quit`.
- The `exit` command is now `/exit`.
