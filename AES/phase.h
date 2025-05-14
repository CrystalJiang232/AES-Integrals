#pragma once
#include <string>
#include <unordered_map>
#include <span>

class ArgParser
{
public:
    void addOption(std::string_view name, bool isFlag = true);
    bool parse(int,char**);
    std::string_view getValue(std::string_view name) const;
    bool getFlag(std::string_view name) const;

private:
    std::unordered_map<std::string_view, bool> options;
    std::unordered_map<std::string_view, std::string_view> values;
};