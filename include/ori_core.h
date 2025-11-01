#ifndef ORI_CORE_H
#define ORI_CORE_H

#include <string>
#include <vector>
#include <memory>

#ifdef CURL_FOUND
#include <curl/curl.h>
#endif

struct ChatMessage {
    std::string role;
    std::string content;
};

class OpenRouterAPI {
private:
    std::string api_key;
    std::vector<ChatMessage> conversation_history;
    std::string getMotherboardFingerprint();
    
public:
    OpenRouterAPI();
    ~OpenRouterAPI();
    
    bool loadApiKey();
    bool setApiKey(const std::string& key);
    std::string getApiKey() const;
    void setSystemPrompt(const std::string& prompt);
    // No-op encryption functions (kept for API compatibility)
    std::string encrypt(const std::string& data, const std::string& key);
    std::string decrypt(const std::string& data, const std::string& key);
    // Build an encryption key from multiple non-privileged machine/user factors
    std::string buildEncryptionKey();
    
    std::string sendQuery(const std::string& prompt);
    std::string sendComplexQuery(const std::string& prompt);
};

class OriAssistant {
private:
    std::string executable_path;
    const size_t BANNER_HEIGHT = 12;  // Height of the banner in lines
    size_t current_output_lines = 0;  // Track number of lines output
public:
    std::unique_ptr<OpenRouterAPI> api;
    
public:
    OriAssistant();
    ~OriAssistant();
    
    void setExecutablePath(const std::string& path);
    bool initialize();
    void run();
    void showHelp();
    // Read a line (possibly multiline) from the user with basic editing support.
    // Supports Alt+Enter to insert a newline without submitting.
    std::string readInput();
    void processSingleRequest(const std::string& prompt, bool auto_confirm);
    void handleCommandExecution(const std::string& command, bool auto_confirm);
    void handleResponse(const std::string& response, bool auto_confirm);
    void checkForUpdates(bool silent);
};

#endif // ORI_CORE_H