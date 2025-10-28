#include "ori_core.h"
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <sys/stat.h>
#include <sstream>

// ANSI Color Codes
const std::string RESET = "\033[0m";
const std::string BOLD = "\033[1m";
const std::string RED = "\033[31m";
const std::string GREEN = "\033[32m";
const std::string YELLOW = "\033[33m";
const std::string BLUE = "\033[34m";
const std::string MAGENTA = "\033[35m";
const std::string CYAN = "\033[36m";

#ifdef CURL_FOUND
#include <curl/curl.h>
#include <iomanip>
#include <json/json.h>

// Callback function to write response data to a string
static size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* response) {
    size_t total_size = size * nmemb;
    response->append((char*)contents, total_size);
    return total_size;
}
#endif

OpenRouterAPI::OpenRouterAPI() {
    // Constructor
}

OpenRouterAPI::~OpenRouterAPI() {
    // Destructor
}

std::string OpenRouterAPI::getMotherboardFingerprint() {
    std::string result = "";
    FILE* pipe = popen("sudo dmidecode -s baseboard-serial-number", "r");
    if (!pipe) {
        return "";
    }
    char buffer[128];
    while (fgets(buffer, sizeof(buffer), pipe) != NULL) {
        result += buffer;
    }
    pclose(pipe);
    result.erase(result.find_last_not_of(" \n\r\t")+1);
    return result;
}

std::string OpenRouterAPI::encrypt(const std::string& data, const std::string& key) {
    std::string result = data;
    for (size_t i = 0; i < data.size(); ++i) {
        result[i] = data[i] ^ key[i % key.size()];
    }
    return result;
}

std::string OpenRouterAPI::decrypt(const std::string& data, const std::string& key) {
    return encrypt(data, key);
}

void OpenRouterAPI::setSystemPrompt(const std::string& prompt) {
    conversation_history.clear();
    conversation_history.push_back({"system", prompt});
}

bool OpenRouterAPI::loadApiKey() {
    // First try to load from environment variable
    const char* env_key = std::getenv("OPENROUTER_API_KEY");
    if (env_key != nullptr && std::strlen(env_key) > 0) {
        api_key = std::string(env_key);
        return true;
    }
    
    // Then try to load from file
    const char* home_dir = std::getenv("HOME");
    if (home_dir != nullptr) {
        std::string key_file = std::string(home_dir) + "/.config/ori/key";
        std::ifstream file(key_file);
        if (file.is_open()) {
            std::string encrypted_key;
            std::getline(file, encrypted_key);
            file.close();
            std::string fingerprint = getMotherboardFingerprint();
            if (!fingerprint.empty()) {
                api_key = decrypt(encrypted_key, fingerprint);
                if (api_key.length() > 0) { 
                    return true;
                }
            }
            std::cout << YELLOW << "Failed to decrypt API key. It might be corrupted or the motherboard has been changed." << RESET << std::endl;
        }
    }
    
    // If no key is found or decryption fails, ask the user for it
    std::cout << "Please enter your OpenRouter API key: ";
    std::string key;
    std::getline(std::cin, key);
    if (key.empty()) {
        return false;
    }

    std::string fingerprint = getMotherboardFingerprint();
    if (!fingerprint.empty()) {
        std::string encrypted_key = encrypt(key, fingerprint);
        if (home_dir != nullptr) {
            std::string key_file = std::string(home_dir) + "/.config/ori/key";
            std::ofstream file(key_file);
            if (file.is_open()) {
                file << encrypted_key;
                file.close();
                api_key = key;
                return true;
            }
        }
    }

    return false;
}

bool OpenRouterAPI::setApiKey(const std::string& key) {
    api_key = key;
    return true;
}

std::string OpenRouterAPI::getApiKey() const {
    return api_key;
}

std::string OpenRouterAPI::sendQuery(const std::string& prompt) {
#ifdef CURL_FOUND
    // Add user's message to history
    conversation_history.push_back({"user", prompt});

    // Initialize curl
    CURL* curl = curl_easy_init();
    if (!curl) {
        return RED + "Error: Failed to initialize curl" + RESET;
    }
    
    // Prepare the request data
    Json::Value request_data;
    request_data["model"] = "cognitivecomputations/dolphin-mistral-24b-venice-edition:free";
    
    Json::Value messages(Json::arrayValue);
    for (const auto& msg : conversation_history) {
        Json::Value message;
        message["role"] = msg.role;
        message["content"] = msg.content;
        messages.append(message);
    }
    request_data["messages"] = messages;
    
    Json::StreamWriterBuilder builder;
    builder["indentation"] = ""; // Compact output
    std::string json_data = Json::writeString(builder, request_data);
    
    // Set up curl options
    std::string response_data;
    struct curl_slist* headers = NULL;
    
    headers = curl_slist_append(headers, "Content-Type: application/json");
    std::string auth_header = "Authorization: Bearer " + api_key;
    headers = curl_slist_append(headers, auth_header.c_str());
    
    curl_easy_setopt(curl, CURLOPT_URL, "https://openrouter.ai/api/v1/chat/completions");
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, json_data.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_data);
    curl_easy_setopt(curl, CURLOPT_USERAGENT, "OriAssistant/1.0");
    
    // Perform the request
    CURLcode res = curl_easy_perform(curl);
    
    // Clean up
    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);
    
    if (res != CURLE_OK) {
        return RED + "Error: Failed to connect to OpenRouter API - " + std::string(curl_easy_strerror(res)) + RESET;
    }
    
    // Parse the response
    Json::Value response_json;
    Json::Reader reader;
    if (!reader.parse(response_data, response_json)) {
        return RED + "Error: Failed to parse API response - " + response_data + RESET;
    }
    
    // Check for a structured error response
    if (response_json.isMember("error")) {
        std::string error_message = RED + "API Error";
        const Json::Value& error_obj = response_json["error"];

        if (error_obj.isMember("code") && error_obj["code"].isNumeric()) {
            error_message += " (Code: " + error_obj["code"].asString() + ")";
        }
        if (error_obj.isMember("message") && error_obj["message"].isString()) {
            error_message += ": " + error_obj["message"].asString();
        }
        if (error_obj.isMember("metadata") && error_obj["metadata"].isObject() &&
            error_obj["metadata"].isMember("raw") && error_obj["metadata"]["raw"].isString()) {
            error_message += " (Details: " + error_obj["metadata"]["raw"].asString() + ")";
        }
        return error_message + RESET;
    }
    
    // Extract the response text
    if (response_json.isMember("choices") && response_json["choices"].isArray() && 
        response_json["choices"].size() > 0 && 
        response_json["choices"][0].isMember("message") && 
        response_json["choices"][0]["message"].isMember("content")) {
        
        std::string assistant_response = response_json["choices"][0]["message"]["content"].asString();
        // Add assistant's response to history
        conversation_history.push_back({"assistant", assistant_response});
        return assistant_response;
    } else {
        return RED + "Error: Unexpected API response format - " + response_data + RESET;
    }
#else
    // Fallback to placeholder implementation
    return "I received your message: '" + prompt + "'. In a full implementation, I would provide a more detailed response based on the AI model.";
#endif
}

std::string OpenRouterAPI::sendComplexQuery(const std::string& prompt) {
    // This is a placeholder implementation for complex queries
    // In a real implementation, this would make an HTTP request to OpenRouter API
    // with Google Gemini 2.0 (flash experimental) model
    return "Complex response from gemini-2.0-flash-exp for prompt: " + prompt;
}

#ifdef CURL_FOUND
static bool curl_initialized = false;
#endif

OriAssistant::OriAssistant() {
#ifdef CURL_FOUND
    if (!curl_initialized) {
        curl_global_init(CURL_GLOBAL_DEFAULT);
        curl_initialized = true;
    }
#endif
    api = std::make_unique<OpenRouterAPI>();
}

OriAssistant::~OriAssistant() {
    // Destructor
#ifdef CURL_FOUND
    curl_global_cleanup();
#endif
}



bool OriAssistant::initialize() {
    // Create config directory if it doesn't exist
    const char* home_dir = std::getenv("HOME");
    if (home_dir != nullptr) {
        std::string config_dir = std::string(home_dir) + "/.config/ori";
        struct stat st;
        if (stat(config_dir.c_str(), &st) == -1) {
            mkdir(config_dir.c_str(), 0755);
        }
    }
    
    if (!api->loadApiKey()) {
        std::cerr << RED << "Error: Failed to load API key. Please set OPENROUTER_API_KEY or create Openrouter_api_key.txt." << RESET << std::endl;
        return false;
    }
    return true;
}

void OriAssistant::run() {
    checkForUpdates(true);
    std::cout << BLUE << R"( 
    ███████    ███████████   █████            ███████████ █████  █████ █████
  ███▒▒▒▒▒███ ▒▒███▒▒▒▒▒███ ▒▒███            ▒█▒▒▒███▒▒▒█▒▒███  ▒▒███ ▒▒███ 
 ███     ▒▒███ ▒███    ▒███  ▒███            ▒   ▒███  ▒  ▒███   ▒███  ▒███ 
▒███      ▒███ ▒██████████   ▒███  ██████████    ▒███     ▒███   ▒███  ▒███ 
▒███      ▒███ ▒███▒▒▒▒▒███  ▒███ ▒▒▒▒▒▒▒▒▒▒     ▒███     ▒███   ▒███  ▒███ 
▒▒███     ███  ▒███    ▒███  ▒███                ▒███     ▒███   ▒███  ▒███ 
 ▒▒▒███████▒   █████   █████ █████               █████    ▒▒████████   █████
   ▒▒▒▒▒▒▒    ▒▒▒▒▒   ▒▒▒▒▒ ▒▒▒▒▒               ▒▒▒▒▒      ▒▒▒▒▒▒▒▒   ▒▒▒▒▒
)" << RESET << std::endl;
    std::cout << BOLD << BLUE << "ORI Terminal Assistant v0.4" << RESET << "\n";
    std::cout << "Type '/help' for available commands or '/quit' to exit.\n\n";
    
    std::string input;
    while (true) {
        std::cout << BOLD << GREEN << "> " << RESET;
        std::getline(std::cin, input);

        if (std::cin.fail() || std::cin.eof()) {
            break;
        }
        
        if (input.rfind('/', 0) == 0) {
            if (input == "/quit" || input == "/exit") {
                break;
            } else if (input == "/help") {
                showHelp();
            } else if (input == "/clear") {
                std::system("clear");
            } else {
                std::cout << RED << "Unknown command: " << input << RESET << std::endl;
            }
        } else if (!input.empty()) {
            processSingleRequest(input, false); // Interactive mode, no auto-confirm
        }
    }
}

void OriAssistant::processSingleRequest(const std::string& prompt, bool auto_confirm) {
    if (!api) {
        std::cout << RED << "Error: API not initialized" << RESET << std::endl;
        return;
    }
    std::string response = api->sendQuery(prompt);

    size_t current_pos = 0;
    while (true) {
        size_t start_pos = response.find("[exec]", current_pos);
        size_t end_pos = response.find("[/exec]", start_pos);

        if (start_pos != std::string::npos && end_pos != std::string::npos) {
            // Print any text before the command
            if (start_pos > current_pos) {
                std::cout << response.substr(current_pos, start_pos - current_pos);
            }

            start_pos += 6; // Move past "[exec]"
            std::string command = response.substr(start_pos, end_pos - start_pos);
            handleCommandExecution(command, auto_confirm);

            current_pos = end_pos + 7; // Move past "[/exec]"
        } else {
            // Print any remaining text after the last command
            if (current_pos < response.length()) {
                std::cout << response.substr(current_pos) << "\n\n";
            }
            break;
        }
    }
}

void OriAssistant::handleCommandExecution(const std::string& command, bool auto_confirm) {
    bool confirmed = false;
    if (auto_confirm) {
        confirmed = true;
    } else {
        std::cout << YELLOW << "Execute the following command? (y/n): " << RESET << BOLD << CYAN << "<< " << command << " >> " << RESET;
        std::string confirmation;
        std::getline(std::cin, confirmation);
        if (confirmation == "y" || confirmation == "Y") {
            confirmed = true;
        }
    }

    if (confirmed) {
        std::cout << MAGENTA << "Executing command: " << command << RESET << std::endl;
        // Execute the command and capture output
        std::string result = "";
        FILE* pipe = popen(command.c_str(), "r");
        if (!pipe) {
            result = RED + "Error: could not execute command." + RESET;
        } else {
            char buffer[128];
            while (fgets(buffer, sizeof(buffer), pipe) != NULL) {
                result += buffer;
            }
            pclose(pipe);
        }

        // Formulate a new prompt and send it back to the AI
        std::string feedback_prompt = "The command \"" + command + "\" produced the following output:\n---\n" + result + "\n---\nPlease summarize this output or answer the original question based on it.";
        processSingleRequest(feedback_prompt, auto_confirm);

    } else {
        std::cout << YELLOW << "Command execution cancelled." << RESET << "\n\n";
        // Inform the AI that the command was cancelled.
        std::string cancel_prompt = "The user cancelled the command execution. Please inform the user that you cannot answer the question without running the command.";
        api->sendQuery(cancel_prompt);
    }
}

void OriAssistant::setExecutablePath(const std::string& path) {
    executable_path = path;
}

void OriAssistant::checkForUpdates(bool silent) {
    #ifdef CURL_FOUND
    CURL* curl = curl_easy_init();
    if (curl) {
        std::string version_url = "https://raw.githubusercontent.com/piratheon/ORI/refs/heads/main/.version";
        std::string remote_version;
        curl_easy_setopt(curl, CURLOPT_URL, version_url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &remote_version);
        CURLcode res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);

        if (res == CURLE_OK) {
            std::ifstream version_file(".version");
            std::string current_version = "0.4";
            if (version_file.is_open()) {
                std::getline(version_file, current_version);
                version_file.close();
            }
            remote_version.erase(remote_version.find_last_not_of(" \n\r\t")+1);
            if (current_version != remote_version) {
                if (silent) {
                    std::cout << YELLOW << "A new version of Ori is available: " << remote_version << RESET << std::endl;
                    std::cout << "Run " << BOLD << "ori --check-for-updates" << RESET << " to update." << std::endl;
                } else {
                    std::cout << YELLOW << "A new version of Ori is available: " << remote_version << RESET << std::endl;
                    std::cout << "Do you want to update? (y/n): ";
                    std::string confirmation;
                    std::getline(std::cin, confirmation);
                    if (confirmation == "y" || confirmation == "Y") {
                        std::string download_url = "https://github.com/piratheon/ORI/releases/download/v" + remote_version + "/ori-linux_x86-64-v" + remote_version + ".bin";
                        std::string temp_file = "/tmp/ori_update.bin";
                        CURL* download_curl = curl_easy_init();
                        if (download_curl) {
                            FILE* fp = fopen(temp_file.c_str(), "wb");
                            if (fp) {
                                curl_easy_setopt(download_curl, CURLOPT_URL, download_url.c_str());
                                curl_easy_setopt(download_curl, CURLOPT_WRITEFUNCTION, NULL);
                                curl_easy_setopt(download_curl, CURLOPT_WRITEDATA, fp);
                                CURLcode download_res = curl_easy_perform(download_curl);
                                fclose(fp);
                                if (download_res == CURLE_OK) {
                                    chmod(temp_file.c_str(), 0755);
                                    if (rename(temp_file.c_str(), executable_path.c_str()) == 0) {
                                        std::cout << GREEN << "Update successful! Restarting Ori..." << RESET << std::endl;
                                        char* const argv[] = {const_cast<char*>(executable_path.c_str()), NULL};
                                        execv(executable_path.c_str(), argv);
                                    } else {
                                        std::cout << RED << "Failed to replace the old binary." << RESET << std::endl;
                                    }
                                } else {
                                    std::cout << RED << "Failed to download the update." << RESET << std::endl;
                                }
                            }
                            curl_easy_cleanup(download_curl);
                        }
                    }
                }
            }
        }
    }
    #endif
}

void OriAssistant::showHelp() {
    std::cout << "Available commands:\n";
    std::cout << "  /help     - Show this help message\n";
    std::cout << "  /quit     - Exit the assistant\n";
    std::cout << "  /exit     - Exit the assistant\n";
    std::cout << "  /clear    - Clear the screen\n";
    std::cout << "  Or type any query to send to the AI assistant\n\n";
}
