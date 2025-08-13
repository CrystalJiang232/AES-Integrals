#include "phase.h"
#include "gen_cipher.h"

#ifdef __GNUC__
#include <bits/stdc++.h>
#else
#include <charconv>
#include <print>
#endif

ArgParser::ArgParser(std::initializer_list<std::pair<const std::string_view,ArgType>> is) : type(is)
{
    
}

bool ArgParser::parse(int argc,char** argv)
{
    if(argc == 1)
    {
        return false; //No option specified
    }

    //Incremental
    std::optional<std::string_view> last;

    for(std::string_view sv : std::ranges::subrange(argv,argv+argc) | std::views::drop(1))
    {
        if(!last) //no last buffering option - scanning for options
        {
            if(sv.starts_with("--")) //not abbreviated
            {
                sv = sv.substr(2);
                if(!type.contains(sv))
                {
                    return false;
                }
            }
            else if(sv.starts_with("-")) //abbreviated
            {
                sv = sv.substr(1);
                if(sv.length() > 1 || !type.contains(sv))
                {
                    return false; //Invalid abbreviation
                }
            }
            else
            {
                return false;
            }   

            if(type[sv] == ArgType::Flag) //Leave numeric and string for latter processing
            {
                data[sv] = true;
            }
            else
            {
                last.emplace(sv);
            }
        }
        else
        {
            int x{};
            switch(type[*last]) //shouldn't throw
            {
            case ArgType::Numeric:
                if(auto [ptr,ec] = std::from_chars(sv.data(),sv.data()+sv.length(),x);ec != std::errc{})
                {
                    return false; //phasing numbers GG
                }
                data[*last] = x;
                break;
            case ArgType::String:
                data[*last] = std::string{sv};
                break;
            default: //Shouldn't reach this point
                throw std::runtime_error("Shouldn't reach this point where all argtypes are considered");
            }
            last = std::nullopt;
        }
    }

    if(last) //Still has lingering options
    {
        return false;
    }

    return true;
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
	

