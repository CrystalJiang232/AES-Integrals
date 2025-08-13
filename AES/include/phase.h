#pragma once
#include <string>
#include <unordered_map>
#include <span>
#include <expected>
#include <variant>
#include "config.h"

enum class ArgType : char
{
    Flag,
    Numeric,
    String
};

template<class Ty,class ...Types>
concept valid_type_range = (std::same_as<Ty,Types> || ...) && sizeof...(Types) > 0;

class ArgParser
{
public:
    using variant_t = std::variant<bool,int,std::string>;    

    enum class access_err
    {
        invalid_key,
        no_data,
        bad_type
    };

    ArgParser(std::initializer_list<std::pair<const std::string_view,ArgType>>);
    bool parse(int,char**);

    template<valid_type_range<int,std::string,bool> Ty>
    std::expected<Ty,access_err> get(std::string_view key) noexcept
    {
        if(!type.contains(key))
        {
            return std::unexpected(access_err::invalid_key);
        }
        if(!data.contains(key))
        {
            return std::unexpected(access_err::no_data);
        }
        try
        {
            return std::get<Ty>(data[key]);       
        }
        catch(const std::bad_variant_access&)
        {
            return std::unexpected(access_err::bad_type);            
        }
    }

private:
    std::unordered_map<std::string_view,ArgType> type;
    std::unordered_map<std::string_view, std::variant<bool,int,std::string>> data;
};

std::expected<config, int> parse_configurate(int, char**);

void usage();
void help();
