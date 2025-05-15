#pragma once
#include <string>
#include <unordered_map>
#include <span>
#include <expected>
#include "config.h"

class ArgParser
{
public:
    ArgParser();
    ArgParser(std::initializer_list<std::pair<const std::string_view,bool>>);
    void addOption(std::string_view name, bool isFlag = false);
    bool parse(int,char**);
    std::string_view getValue(std::string_view name) const;
    bool getFlag(std::string_view name) const;

private:
    std::unordered_map<std::string_view, bool> options;
    std::unordered_map<std::string_view, std::string_view> values;
};

std::expected<config, int> parse_configurate(int, char**);
ArgParser make_ArgParser();

void usage();
void help();