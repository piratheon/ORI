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
    
public:
    OpenRouterAPI();
    ~OpenRouterAPI();
    
    bool loadApiKey();
    bool setApiKey(const std::string& key);
    std::string getApiKey() const;
    void setSystemPrompt(const std::string& prompt);
    
    std::string sendQuery(const std::string& prompt);
    std::string sendComplexQuery(const std::string& prompt);
};

class OriAssistant {
public:
    std::unique_ptr<OpenRouterAPI> api;
    
public:
    OriAssistant();
    ~OriAssistant();
    
    bool initialize();
    void run();
    void showHelp();
    void processSingleRequest(const std::string& prompt, bool auto_confirm);
    void handleCommandExecution(const std::string& command, bool auto_confirm);
};

#endif // ORI_CORE_H
