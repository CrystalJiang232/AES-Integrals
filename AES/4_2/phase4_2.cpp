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
		{"h",ArgType::Flag}, //help
		{"i",ArgType::String}, //inputfile
		{"o",ArgType::String}, //outputfile
		{"nothread",ArgType::Flag}, //whether to disable multithreading
		{"count",ArgType::Numeric}, //maximum ciphertext count
		{"echo",ArgType::Numeric} //extent of echoing messages to the terminal
	};

	config cfg;

	if (!ap.parse(argc, argv))
	{
		//No argument specified
		usage();
		return std::unexpected(1);
	}

	if (ap.get<bool>("h").value_or(false)) //Defaults to false(???)
	{
		//help
		help();
		return std::unexpected(0);
	}
	//input file logic

	if(auto val = ap.get<std::string>("i");val)
	{
		cfg.inputname = *val;
	}
	else
	{
		return std::unexpected(1);
	}
	//output file logic

	if(auto val = ap.get<std::string>("o");val)
	{
		cfg.outputname = *val;
	}
	else
	{
		//return std::unexpected(1);
	}

	if (auto val = ap.get<int>("count");val)
	{
		cfg.count = *val;
	}
	else //Default
	{
		cfg.count = 66;
	}

	cfg.threading = !ap.get<bool>("nothread").value_or(false);

	if (auto val = ap.get<int>("echo");val)
	{
		cfg.ech = config::echo(*val);
	}
	return cfg;

}
