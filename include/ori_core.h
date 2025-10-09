#ifndef ORI_CORE_H
#define ORI_CORE_H

#include <string>
#include <vector>
#include <memory>

#ifdef CURL_FOUND
#include <curl/curl.h>
#endif

class OpenRouterAPI {
private:
    std::string api_key;
    std::string default_model;
    
public:
    OpenRouterAPI();
    ~OpenRouterAPI();
    
    bool loadApiKey();
    bool setApiKey(const std::string& key);
    std::string getApiKey() const;
    
    std::string sendQuery(const std::string& prompt);
    std::string sendComplexQuery(const std::string& prompt);
};

class OriAssistant {
private:
    std::unique_ptr<OpenRouterAPI> api;
    
public:
    OriAssistant();
    ~OriAssistant();
    
    bool initialize();
    void run();
    void showHelp();
    std::string processPrompt(const std::string& prompt);
};

#endif // ORI_CORE_H
