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


ArgParser make_ArgParser() //ArgParser options added here
{
	return ArgParser{
		{"h",true}, //help
		{"g",true}, //generate cipher set
		{"i",false}, //inputfile
		{"o",false}, //outputfile
		{"nothread",true}, //whether to disable multithreading
		{"count",false}, //maximum ciphertext count
		{"echo",false} //extent of echoing messages to the terminal
	};
}


std::expected<config, int> parse_configurate(int argc, char** argv)
{
	ArgParser ap = make_ArgParser();
	config cfg;

	if (!ap.parse(argc, argv))
	{
		//No argument specified
		usage();
		return std::unexpected(1);
	}

	if (ap.getFlag("h"))
	{
		//help
		help();
		return std::unexpected(0);
	}

	if(ap.getFlag("g"))
	{
		//Generate cipher set
		gen_cipher_set();
		std::println("Cipher set has been saved to ./pdelta.txt");
		return std::unexpected(0);
	}

	//input file logic

	switch (ap.getFlag("i") + (ap.getFlag("it") << 1) + (ap.getFlag("ib") << 2))
	{
	case 1:
		cfg.inputname = ap.getValue("i");
		cfg.input = config::io_t::hextest;
		break;
	case 2:
		cfg.inputname = ap.getValue("it");
		cfg.input = config::io_t::hextest;
		break;
	case 4:
		cfg.inputname = ap.getValue("ib");
		cfg.input = config::io_t::binary;
		break;
	default:
		//Invalid specifier: input can be given once and once only
		usage();
		return std::unexpected(1);
	}

	//output file logic

	switch (ap.getFlag("o") + (ap.getFlag("ot") << 1) + (ap.getFlag("ob") << 2))
	{
	case 0:
		break;
	case 1:
		cfg.outputname = ap.getValue("o");
		cfg.output = config::io_t::hextest;
		break;
	case 2:
		cfg.outputname = ap.getValue("ot");
		cfg.output = config::io_t::hextest;
		break;
	case 4:
		cfg.outputname = ap.getValue("ob");
		cfg.output = config::io_t::binary;
		break;
	default:
		//Invalid specifier: output can be given no more than once
		usage();
		return std::unexpected(1);
	}

	if (ap.getFlag("count"))
	{
		std::string temp(ap.getValue("count"));
		if (auto [x, y] = std::from_chars(temp.data(), temp.data() + temp.length(), cfg.count);y != std::errc{})
		{
			//Invalid numeric string for `count`
			usage();
			return std::unexpected(1);
		}
	}
	else
	{
		cfg.count = 66;
	}

	cfg.threading = !ap.getFlag("nothread");

	if (ap.getFlag("echo"))
	{
		std::string temp(ap.getValue("echo"));
		int op{ -2 };
		if (auto [x, y] = std::from_chars(temp.data(), temp.data() + temp.length(), op);y != std::errc{} || op < 0 || op > 3)
		{
			//Invalid numeric string for `echo`
			usage();
			return std::unexpected(1);
		}
		cfg.ech = config::echo(op);
	}
	return cfg;

}

void usage()
{
	std::println("Usage: -i|--it|--ib <filename1> [-o|--ot|--ob <filename2>] [--count xxx] [--echo xxx] [--nothread]\n"
	 	"       -g|-h\n\n"
		"For help,specify \"-h\" option");
}

void help()
{
	std::println
	("Usage: -i|--it|--ib <inputfile> [-o|--ot|--ob <outputfile>] [--count xxx] [--echo xxx] [--nothread]\n"
	 "       -g|-h\n\n"
		"-i,--it        Use <inputfile> as input in hexidecimal-text format.\n"
		"--ib           Use <inputfile> as input in binary format.\n"
		"               One of the input options must be specified.\n"
		"-o,--ot        Use <outputfile> as output in hexidecimal-text format.\n"
		"--ob           Use <outputfile> as output in binary format.\n"
		"               The key in hex format will be printed to the console disregarding this option unless -echo=0.\n"
		"--count xxx    Read at most xxx valid ciphertexts from input file. Valid range for xxx is 30-256. Default value is 66.\n"
		"               Note that ciphertext count lower than 50 can cause significant loss in decryption accuracy."
		"--echo xxx     Specify the echo(output to the terminal) options:\n"
		"               3 - Display time elapsed for every round of decryption attempt.\n"
		"               2 - Display time elapsed for every group of key deciphered(Default option).\n"
		"               1 - Display time elapsed after completing the entire decryption process.\n"
		"               0 - Disable echo to the terminal.\n"
		"               Default value is 2.\n"
		"--nothread     Disable multithreading. This may significantly prolong the execution time.\n");

	std::println
	(   "-g             Generate a Δ'-set ciphertext(length = 66) under current directory for attack testing.\n"
	    "-h             Display this help screen.");
}
	

