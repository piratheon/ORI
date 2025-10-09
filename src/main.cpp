#include "ori_core.h"
#include "orpm.h"
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>

void showUsage() {
    std::cout << "ORI Terminal Assistant v0.2 - Linux TUI AI Assistant\n";
    std::cout << "Usage: ori [options] [prompt]\n\n";
    std::cout << "Options:\n";
    std::cout << "  -h, --help          Show this help message\n";
    std::cout << "  -v, --version       Show version information\n";
    std::cout << "  --load-prompt <file> Load prompt from file (supports .md, .txt, etc.)\n";
    std::cout << "  --orpm              Run the plugin manager\n";
    std::cout << "  --orpm-install <plugin>  Install a plugin\n";
    std::cout << "  --orpm-remove <plugin>   Remove a plugin\n";
    std::cout << "  --orpm-search <query>    Search for plugins\n";
    std::cout << "  --orpm-list         List available plugins\n";
    std::cout << "  --orpm-list-installed  List installed plugins\n";
    std::cout << "\nShell Integration Examples:\n";
    std::cout << "  ori 'print current active username'\n";
    std::cout << "  ori print current active username\n";
    std::cout << "  ori --load-prompt /path/to/prompt.md\n";
    std::cout << "\nIf no options are provided, the interactive assistant will start.\n";
}

void showVersion() {
    std::cout << "ORI Terminal Assistant v0.2\n";
}

std::string loadPromptFromFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file '" << filepath << "'\n";
        return "";
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    file.close();
    
    return buffer.str();
}

void processDirectPrompt(const std::string& prompt) {
    OriAssistant assistant;
    if (!assistant.initialize()) {
        std::cerr << "Failed to initialize ORI Terminal Assistant. Please check your API key configuration.\n";
        exit(1);
    }
    
    // Process the prompt directly
    std::string response = assistant.processPrompt(prompt);
    std::cout << response << std::endl;
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
    // Parse command line arguments
    if (argc > 1) {
        std::string arg = argv[1];
        if (arg == "-h" || arg == "--help") {
            showUsage();
            return 0;
        } else if (arg == "-v" || arg == "--version") {
            showVersion();
            return 0;
        } else if (arg == "--load-prompt") {
            if (argc < 3) {
                std::cerr << "Error: --load-prompt requires a file path\n";
                return 1;
            }
            std::string prompt = loadPromptFromFile(argv[2]);
            if (!prompt.empty()) {
                processDirectPrompt(prompt);
            }
            return 0;
        } else if (arg == "--orpm") {
            runOrpm(argc, argv);
            return 0;
        } else if (arg.substr(0, 6) == "--orpm") {
            runOrpm(argc, argv);
            return 0;
        } else {
            // Direct prompt processing - handle both quoted and unquoted arguments
            std::string prompt;
            if (argc == 2) {
                // Single argument - could be quoted or unquoted
                prompt = argv[1];
            } else {
                // Multiple arguments - join them
                for (int i = 1; i < argc; i++) {
                    if (i > 1) prompt += " ";
                    prompt += argv[i];
                }
            }
            processDirectPrompt(prompt);
            return 0;
        }
    }
    
    // Run the interactive assistant
    OriAssistant assistant;
    if (!assistant.initialize()) {
        std::cerr << "Failed to initialize ORI Terminal Assistant. Please check your API key configuration.\n";
        return 1;
    }
    
    assistant.run();
    return 0;
}
