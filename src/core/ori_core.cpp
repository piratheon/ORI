#include "ori_core.h"
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <sys/stat.h>

#ifdef CURL_FOUND
#include <curl/curl.h>
#include <sstream>
#include <iomanip>
#include <json/json.h>

// Callback function to write response data to a string
static size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* response) {
    size_t total_size = size * nmemb;
    response->append((char*)contents, total_size);
    return total_size;
}
#endif

OpenRouterAPI::OpenRouterAPI() : default_model("deepseek-chat") {
    // Constructor
}

OpenRouterAPI::~OpenRouterAPI() {
    // Destructor
}

bool OpenRouterAPI::loadApiKey() {
    // First try to load from environment variable
    const char* env_key = std::getenv("OPENROUTER_API_KEY");
    if (env_key != nullptr && std::strlen(env_key) > 0) {
        api_key = std::string(env_key);
        std::cout << "Loaded API key from environment variable" << std::endl;
        return true;
    }
    
    // Then try to load from file
    std::string key_file = "Openrouter_api_key.txt";
    std::ifstream file(key_file);
    if (file.is_open()) {
        std::getline(file, api_key);
        file.close();
        std::cout << "Loaded API key from current directory file" << std::endl;
        return true;
    }
    
    // Try to load from home directory
    const char* home_dir = std::getenv("HOME");
    if (home_dir != nullptr) {
        std::string home_key_file = std::string(home_dir) + "/.config/ori/Openrouter_api_key.txt";
        std::ifstream home_file(home_key_file);
        if (home_file.is_open()) {
            std::getline(home_file, api_key);
            home_file.close();
            std::cout << "Loaded API key from home directory file" << std::endl;
            return true;
        }
    }
    
    std::cout << "Failed to load API key" << std::endl;
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
    // Initialize curl
    CURL* curl = curl_easy_init();
    if (!curl) {
        return "Error: Failed to initialize curl";
    }
    
    // Prepare the request data
    Json::Value request_data;
    request_data["model"] = "tngtech/deepseek-r1t2-chimera:free";
    
    Json::Value message;
    message["role"] = "user";
    message["content"] = prompt;
    
    Json::Value messages(Json::arrayValue);
    messages.append(message);
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
        return "Error: Failed to connect to OpenRouter API - " + std::string(curl_easy_strerror(res));
    }
    
    // Parse the response
    Json::Value response_json;
    Json::Reader reader;
    if (!reader.parse(response_data, response_json)) {
        return "Error: Failed to parse API response - " + response_data;
    }
    
    // Extract the response text
    if (response_json.isMember("choices") && response_json["choices"].isArray() && 
        response_json["choices"].size() > 0 && 
        response_json["choices"][0].isMember("message") && 
        response_json["choices"][0]["message"].isMember("content")) {
        return response_json["choices"][0]["message"]["content"].asString();
    } else {
        return "Error: Unexpected API response format - " + response_data;
    }
#else
    // Fallback to placeholder implementation
    if (prompt == "hello") {
        return "Hello! How can I assist you today?";
    } else if (prompt == "help") {
        return "I can help you with various tasks. Try asking me questions or giving me commands.";
    } else if (prompt == "test") {
        return "This is a test response from the AI assistant.";
    } else {
        return "I received your message: '" + prompt + "'. In a full implementation, I would provide a more detailed response based on the AI model.";
    }
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
    
    return api->loadApiKey();
}

void OriAssistant::run() {
    std::cout << "ORI Terminal Assistant v0.2\n";
    std::cout << "Type 'help' for available commands or 'quit' to exit.\n\n";
    
    std::string input;
    while (true) {
        std::cout << "> ";
        std::getline(std::cin, input);
        
        if (input == "quit" || input == "exit") {
            break;
        } else if (input == "help") {
            showHelp();
        } else if (!input.empty()) {
            std::string response = api->sendQuery(input);
            std::cout << response << "\n\n";
        }
    }
}

void OriAssistant::showHelp() {
    std::cout << "Available commands:\n";
    std::cout << "  help     - Show this help message\n";
    std::cout << "  quit     - Exit the assistant\n";
    std::cout << "  exit     - Exit the assistant\n";
    std::cout << "  Or type any query to send to the AI assistant\n\n";
}

std::string OriAssistant::processPrompt(const std::string& prompt) {
    if (!api) {
        return "Error: API not initialized";
    }
    return api->sendQuery(prompt);
}
