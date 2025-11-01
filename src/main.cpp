#include "ori_core.h"
#include "orpm.h"
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>

const std::string SYSTEM_PROMPT = R"ORI_PROMPT(About Me: Ori

I am Ori, an AI assistant created by the developper named Piratheon. I am designed to be a powerful and versatile assistant, operating directly within your command-line interface to help you with a wide range of tasks.

Core Capabilities

My primary strength is my deep integration with the ORI system and the shell environment. I can help you with development tasks through both direct commands and structured file operations.

Command Execution
- Shell Integration: Execute system commands and scripts
- Process Management: Handle background processes and tasks
- System Operations: Perform system maintenance and checks

File Operations
- Text Editing: Structured file modifications and content management
- Content Analysis: File content examination and search
- File Management: Organization and maintenance of files
- Comparison Tools: File comparison and difference analysis

File System Operations
- File Management: I can read, write, search, and organize files and directories.
- Content Analysis: I can analyze file contents and extract information.
- Text Editing: I support structured text editing operations:
  - Pattern search and replace
  - Content modification and transformation
  - File comparison and analysis
  - Code refactoring and organization
  - File renaming and moving
  - Automated batch operations

Programming and Development
- Code Generation: I can write and edit code in numerous languages, including C/C++, Python, JavaScript, and more.
- Troubleshooting: I can help you debug code, analyze errors, and understand complex technical problems.
- Frameworks: I am familiar with a wide range of development frameworks and libraries.

General Problem Solving
- Research and Analysis: I can conduct research, summarize information, and analyze data to answer your questions.
- Structured Thinking: I break down complex problems into manageable steps and execute them methodically.

Command and Edit Operations

I support two types of operations:

1. Regular Command Execution
For regular shell commands and system operations, I use the [exec] tag.

Always return commands wrapped in the [exec]...[/exec] tag when you want Ori to run them.
Do NOT ask the user for explicit confirmation before returning an [exec] block — Ori will execute the command when it receives it.

If a command includes superuser escalation (for example it contains the word "sudo" or "su"), the assistant must still include the [exec] tag but should also include a short human-readable warning line explaining that the command requires elevated privileges and may prompt the user for a password. Ori itself will execute the command and will not ask for a separate "yes" confirmation; it will only warn the user about privilege implications.

Examples (assistant MUST return strictly escaped JSON for [edit] tags and must wrap shell commands in [exec]):
[exec]ls -la[/exec]
[exec]grep "pattern" file[/exec]
[exec]cat /etc/passwd[/exec]

2. ORI Text Edit Operations
For text editing and file modifications, I use the [edit] tag with JSON format. The [edit] block should contain the operation type, target file(s), and the content details or parameters.

Operations include: search, replace, modify, refactor, rename, compare.

Options that can be used with edits: preview, diff, backup, interactive, safe.

Examples of edit operations (JSON in plain text):
[edit]
{
    "operation": "search",
    "file": "src/main.cpp",
    "content": { "pattern": "TODO" }
}
[/edit]

[edit]
{
    "operation": "replace",
    "file": "config.json",
    "content": { "old": "development", "new": "production" }
}
[/edit]

[edit]
{
    "operation": "compare",
    "files": ["file1.txt", "file2.txt"]
}
[/edit]

Each edit operation specifies the type of operation, target file(s), and content or parameters to modify or analyze.

3. ORI File Write Operations
For creating new files, I use the [writefile(filename)] tag. The content of the file should be placed between the [writefile(filename)] and [/writefile] tags.
You must use the `[writefile]` tag to create files.

Example of a writefile operation:
[writefile(example.txt)]
This is the content of the file.
[/writefile]

ORI Integration Features

Safety mechanisms, previews, backups, and diffs are implemented in the ORI codebase. Edit operations are handled by ORI and may create backups, show diffs, and manage changes according to the configured behavior.

How I Approach Tasks

When you give me a task, I follow a structured approach:
1. Analyze the request to understand requirements.
2. Plan the steps and choose appropriate tools.
3. Execute the plan and report results.

I am here to be your reliable partner in the terminal. Let me know what you need, and I will do my best to assist you.
)ORI_PROMPT";

void showUsage() {
    std::cout << "ORI Terminal Assistant v0.4 - Linux TUI AI Assistant\n";
    std::cout << "Usage: ori [options] [prompt]\n\n";
    std::cout << "Options:\n";
    std::cout << "  -h, --help              Show this help message\n";
    std::cout << "  -v, --version           Show version information\n";
    std::cout << "  -y, --yes               Auto-confirm any command execution prompts\n";
    std::cout << "  --check-for-updates     Check for updates\n";
    std::cout << "  --orpm                  Run the plugin manager\n";
    std::cout << "  --orpm-install <plugin>      Install a plugin\n";
    std::cout << "  --orpm-remove <plugin>       Remove a plugin\n";
    std::cout << "  --orpm-search <query>        Search for plugins\n";
    std::cout << "  --orpm-list             List available plugins\n";
    std::cout << "  --orpm-list-installed      List installed plugins\n";
    std::cout << "\nShell Integration Examples:\n";
    std::cout << "  ori -y 'install nmap for me'\n";
    std::cout << "  ori print current active username\n";
    std::cout << "\nIf no options are provided, the interactive assistant will start.\n";
}

void showVersion() {
    std::cout << "ORI Terminal Assistant v0.4\n";
}

void processDirectPrompt(const std::string& prompt, bool auto_confirm) {
    OriAssistant assistant;
    assistant.api->setSystemPrompt(SYSTEM_PROMPT);
    if (!assistant.initialize()) {
        std::cerr << "Failed to initialize ORI Terminal Assistant. Please check your API key configuration.\n";
        exit(1);
    }
    
    // Get response directly without showing the prompt again
    std::string response = assistant.api->sendQuery(prompt);
    assistant.handleResponse(response, auto_confirm);
}

void runOrpm(int argc, char* argv[]) {
    Orpm orpm;
    if (!orpm.initialize()) {
        std::cerr << "Failed to initialize orpm.\n";
        return;
    }
    
    if (argc < 3) {
        std::cout << "Ori Plugin Manager (orpm)\n";
        std::cout << "Use --help for available commands.\n";
        return;
    }
    
    std::string command = argv[2];
    
    if (command == "--orpm-install" && argc >= 4) {
        std::string plugin_name = argv[3];
        orpm.installPlugin(plugin_name);
    } else if (command == "--orpm-remove" && argc >= 4) {
        std::string plugin_name = argv[3];
        orpm.removePlugin(plugin_name);
    } else if (command == "--orpm-search" && argc >= 4) {
        std::string query = argv[3];
        orpm.searchPlugins(query);
    } else if (command == "--orpm-list") {
        auto plugins = orpm.listAvailablePlugins();
        std::cout << "Available plugins:\n";
        for (const auto& plugin : plugins) {
            std::cout << "  " << plugin.name << " v" << plugin.version << " - " << plugin.description << "\n";
        }
    } else if (command == "--orpm-list-installed") {
        auto plugins = orpm.listInstalledPlugins();
        std::cout << "Installed plugins:\n";
        for (const auto& plugin : plugins) {
            std::cout << "  " << plugin.name << "\n";
        }
    } else {
        std::cout << "Unknown orpm command. Use --help for available commands.\n";
    }
}

int main(int argc, char* argv[]) {
    std::string executable_path = argv[0];
    if (argc <= 1) {
        // No arguments, run interactive assistant
        OriAssistant assistant;
        assistant.setExecutablePath(executable_path);
        assistant.api->setSystemPrompt(SYSTEM_PROMPT);
        if (!assistant.initialize()) {
            std::cerr << "Failed to initialize ORI Terminal Assistant. Please check your API key configuration.\n";
            return 1;
        }
        assistant.run();
        return 0;
    }

    bool auto_confirm = false;
    std::vector<std::string> args(argv + 1, argv + argc);
    std::string prompt = "";
    int prompt_start_index = -1;

    for (int i = 0; i < args.size(); ++i) {
        std::string arg = args[i];
        if (arg == "-h" || arg == "--help") {
            showUsage();
            return 0;
        } else if (arg == "-v" || arg == "--version") {
            showVersion();
            return 0;
        } else if (arg == "-y" || arg == "--yes") {
            auto_confirm = true;
        } else if (arg == "--check-for-updates") {
            OriAssistant assistant;
            assistant.setExecutablePath(executable_path);
            assistant.checkForUpdates(false);
            return 0;
        } else if (arg == "--orpm" || arg.substr(0, 6) == "--orpm") {
            runOrpm(argc, argv);
            return 0;
        } else {
            prompt_start_index = i;
            break;
        }
    }

    if (prompt_start_index != -1) {
        for (int i = prompt_start_index; i < args.size(); ++i) {
            if (!prompt.empty()) prompt += " ";
            prompt += args[i];
        }
    }

    if (!prompt.empty()) {
        processDirectPrompt(prompt, auto_confirm);
        return 0;
    }

    // If we are here, it means flags were passed but no prompt, or only -y was passed.
    // Run interactive mode.
    OriAssistant assistant;
    assistant.setExecutablePath(executable_path);
    assistant.api->setSystemPrompt(SYSTEM_PROMPT);
    if (!assistant.initialize()) {
        std::cerr << "Failed to initialize ORI Terminal Assistant. Please check your API key configuration.\n";
        return 1;
    }
    assistant.run();
    return 0;
}