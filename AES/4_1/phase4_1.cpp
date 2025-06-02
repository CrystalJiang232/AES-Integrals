#include "phase.h"

const char* hint = 
        "Usage: -i <filename1>\n";

const char* full = hint;

std::expected<config,int> parse_configurate(int argc, char** argv)
{
    ArgParser ap{{"i",false}};
    config cfg;

    if(!ap.parse(argc,argv))
    {
        usage();
        return std::unexpected(1);
    }

    if(ap.getFlag("h"))
    {
        help();
        return std::unexpected(0);
    }

    if(ap.getFlag("i"))
    {
        cfg.inputname = ap.getValue("i");
    }
    else
    {
        usage();
        return std::unexpected(1);
    }

    return cfg;
}
