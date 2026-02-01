#ifndef ORI_PROVIDER_H
#define ORI_PROVIDER_H

#include <string>
#include <vector>
#include <json/json.h>

class APIProvider {
public:
    virtual ~APIProvider() {}
    virtual std::string sendQuery(const std::string& prompt, const std::vector<std::pair<std::string, std::string>>& conversation_history) = 0;
    virtual void setModel(const std::string& model_name) = 0;
    virtual void setApiKey(const std::string& key) = 0;
};

class OpenRouterProvider : public APIProvider {
public:
    OpenRouterProvider();
    std::string sendQuery(const std::string& prompt, const std::vector<std::pair<std::string, std::string>>& conversation_history) override;
    void setModel(const std::string& model_name) override;
    void setApiKey(const std::string& key) override;

private:
    std::string model;
    std::string api_key;
};

class GoogleProvider : public APIProvider {
public:
    GoogleProvider();
    std::string sendQuery(const std::string& prompt, const std::vector<std::pair<std::string, std::string>>& conversation_history) override;
    void setModel(const std::string& model_name) override;
    void setApiKey(const std::string& key) override;

private:
    std::string model;
    std::string api_key;
};

class HuggingFaceProvider : public APIProvider {
public:
    HuggingFaceProvider();
    std::string sendQuery(const std::string& prompt, const std::vector<std::pair<std::string, std::string>>& conversation_history) override;
    void setModel(const std::string& model_name) override;
    void setApiKey(const std::string& key) override;

private:
    std::string model;
    std::string api_key;
};

#endif // ORI_PROVIDER_H
