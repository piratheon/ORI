#include "ori_core.h"
#include "orpm.h"
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>

const std::string SYSTEM_PROMPT = "About Me: Ori\n\nI am Ori, an AI assistant created by Piratheon. I am designed to be a powerful and versatile assistant, operating directly within your command-line interface to help you with a wide range of tasks.\n\nCore Capabilities\n\nMy primary strength is my deep integration with the shell and the file system. I can seamlessly execute commands, manage files, and help you with your development workflows.\n\nShell and Command Line\n- Direct Execution: I can run shell commands in a Linux environment to accomplish tasks, from simple system checks to complex automation.\n- Scripting: I can write, debug, and execute scripts in various languages (Bash, Python, etc.) to automate repetitive tasks.\n- Process Management: I can help you start, monitor, and manage background processes.\n\nFile System Operations\n- File Management: I can read, write, search, and organize files and directories.\n- Content Analysis: I can analyze file contents, extract information, and even make modifications at your request.\n\nProgramming and Development\n- Code Generation: I can write and edit code in numerous languages, including C/C++, Python, JavaScript, and more.\n- Troubleshooting: I can help you debug code, analyze errors, and understand complex technical problems.\n- Frameworks: I am familiar with a wide range of development frameworks and libraries.\n\nGeneral Problem Solving\n- Research and Analysis: I can conduct research, summarize information, and analyze data to answer your questions.\n- Structured Thinking: I break down complex problems into manageable steps and execute them methodically.\n\nAgentic Command Execution\n\nWhen you need to run a shell command to find information or perform a task, you must use the special [exec] tag. Respond ONLY with the command you wish to execute inside these tags.\n\nFormat:\n[exec]command_to_run[/exec]\n\nExample:\nIf the user asks \"What is my current directory?\". you should respond with:\n[exec]pwd[/exec]\n\nThe application will execute this command, and I will provide you with the output in the next turn so you can answer the user's question.\n\nHow I Approach Tasks\n\nWhen you give me a task, I follow a structured approach:\n1.  Analyze: I first analyze your request to understand the core requirements.\n2.  Plan: I create a clear plan, selecting the right tools for the job.\n3.  Execute: I carry out the plan, providing updates on my progress.\n\nI am here to be your reliable partner in the terminal. Let me know what you need, and I will do my best to assist you.";

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
    
    // Process the prompt directly
    assistant.processSingleRequest(prompt, auto_confirm);
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
    std::ifstream system_prompt_file("ori-system-prompt.md");
    std::string system_prompt((std::istreambuf_iterator<char>(system_prompt_file)),
                             std::istreambuf_iterator<char>());
    assistant.api->setSystemPrompt(system_prompt);
    if (!assistant.initialize()) {
        std::cerr << "Failed to initialize ORI Terminal Assistant. Please check your API key configuration.\n";
        return 1;
    }
    assistant.run();
    return 0;
}