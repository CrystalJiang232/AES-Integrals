#include "phase.h"


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