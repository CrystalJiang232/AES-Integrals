#include "phase.h"
#include <charconv>
/*Implement character array hint & full here*/

const char* hint = 
        "Usage: -i|--it|--ib <filename1> [-o|--ot|--ob <filename2>] [--count xxx] [--echo xxx] [--nothread]\n"
	 	"       -h\n\n"
		"For help,specify \"-h\" option";

const char* full = 
        "Usage: -i|--it|--ib <inputfile> [-o|--ot|--ob <outputfile>] [--count xxx] [--echo xxx] [--nothread]\n"
	    "       -g|-h\n\n"
		"-i,--it        Use <inputfile> as input in hexidecimal-text format.\n"
		"--ib           Use <inputfile> as input in binary format.\n"
		"               One of the input options must be specified.\n"
		"-o,--ot        Use <outputfile> as output in hexidecimal-text format.\n"
		"--ob           Use <outputfile> as output in binary format.\n"
		"               The key in hex format will be printed to the console disregarding this option unless -echo=0.\n"
		"--count xxx    Read at most xxx valid ciphertexts from input file. Valid range for xxx is 30-256. Default value is 66.\n"
		"               Note that ciphertext count lower than 50 can cause significant loss in decryption accuracy.\n"
		"--echo xxx     Specify the echo(output to the terminal) options:\n"
		"               3 - Display time elapsed for every round of decryption attempt.\n"
		"               2 - Display time elapsed for every group of key deciphered(Default option).\n"
		"               1 - Display time elapsed after completing the entire decryption process.\n"
		"               0 - Disable echo to the terminal.\n"
		"               Default value is 2.\n"
		"--nothread     Disable multithreading. This may significantly prolong the execution time.\n";

std::expected<config, int> parse_configurate(int argc, char** argv)
{
	ArgParser ap{
		{"h",true}, //help
		{"i",false}, //inputfile
		{"o",false}, //outputfile
		{"nothread",true}, //whether to disable multithreading
		{"count",false}, //maximum ciphertext count
		{"echo",false} //extent of echoing messages to the terminal
	};

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
