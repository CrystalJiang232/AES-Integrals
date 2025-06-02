#include "phase.h"
#include "gen_cipher.h"

#ifdef __GNUC__
#include <bits/stdc++.h>
#else
#include <charconv>
#include <print>
#endif

ArgParser::ArgParser() = default;

ArgParser::ArgParser(std::initializer_list<std::pair<const std::string_view,bool>> is) : options{is}
{

}

void ArgParser::addOption(std::string_view name, bool isFlag)
{
    options[name] = isFlag;
}

bool ArgParser::parse(int argc,char** argv)
{
    for (int i = 1; i < argc; )
    {
        std::string_view arg(argv[i]);
        
        if (arg.substr(0, 2) == "--" && options.count(arg.substr(2)))
        {
            if (options[arg.substr(2)])
            {
                values[arg.substr(2)] = "true";
                i++;
            }
            else
            {
                if (i + 1 >= argc) return false;
                values[arg.substr(2)] = argv[i + 1];
                i += 2;
            }
        }
        else if (arg.substr(0, 1) == "-" && arg.size() == 2 && options.count(arg.substr(1, 1)))
        {
            if (options[arg.substr(1, 1)])
            {
                values[arg.substr(1, 1)] = "true";
                i++;
            }
            else
            {
                if (i + 1 >= argc) return false;
                values[arg.substr(1, 1)] = argv[i + 1];
                i += 2;
            }
        }
        else
        {
            return false;
        }
    }
    return true;
}

std::string_view ArgParser::getValue(std::string_view name) const
{
    auto it = values.find(name);
    return it != values.end() ? it->second : "";
}

bool ArgParser::getFlag(std::string_view name) const
{
    return values.count(name) > 0;
}

//parse_configurate function's implementation moved to each specific program

extern const char* hint;

void usage() /*Implementation of hint & full character array required*/
{
	std::println("{}",hint);
}

extern const char* full;

void help()
{
	std::println("{}",full);

	std::println
	("-h             Display this help screen.");
}
	

