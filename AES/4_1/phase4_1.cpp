#include "phase.h"

const char* hint = 
        "Usage: -i <filename1>\n";

const char* full = hint;

std::expected<config,int> parse_configurate(int argc, char** argv)
{
    ArgParser ap{{"i",ArgType::String},{"h",ArgType::Flag}};
    config cfg;

    if(!ap.parse(argc,argv))
    {
        usage();
        return std::unexpected(1);
    }

    if(ap.get<bool>("h").value_or(false))
    {
        help();
        return std::unexpected(0);
    }

    if(auto val = ap.get<std::string>("i");val)
    {
        cfg.inputname = *val;
    }
    else
    {
        usage();
        return std::unexpected(1);
    }

    return cfg;
}
