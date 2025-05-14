#ifdef __GNUC__
#include<bits/stdc++.h>
#else
#include <filesystem>
#include <fstream>
#endif

#include"phase.h"
#include"atk4_2.h"
#include"config.h"


void help()
{
	std::println
	   ("Usage: -i|--it|--ib <filename1> [-o|--ot|--ob <filename2>] [--count xxx] [--echo xxx] [--nothread]\n\n"
		"-i,--it        Use <filename1> as input in hexidecimal-text format.\n"
		"--ib           Use <filename1> as input in binary format.\n"
		"               One of the input options must be specified.\n"
		"-o,--ot        Use <filename2> as output in hexidecimal-text format.\n"
		"--ob           Use <filename2> as output in binary format.\n"
		"               The key in hex format will be printed to the console disregarding the specification(or omit) of this option, unless provided -echo=0.\n"
		"--count xxx     Read at most xxx valid ciphertexts from input file. Valid range for xxx is 10-256. Default value is 66.\n"
		"--echo xxx      Specify the echo contents:\n"
		"               3 - Display time elapsed for every round of decryption attempt.\n"
		"               2 - Display time elapsed for every group of key deciphered(Default option).\n"
		"               1 - Display time elapsed after completing the entire decryption process.\n"
		"               0 - Disable echo to the terminal.\n"
		"               Default value is 2.\n"
		"--nothread     Disable multithreading.This may prolong the execution time by several magnitude.\n");
}

void usage()
{
	std::println("Usage: -i|--it|--ib <filename1> [-o|--ot|--ob <filename2>] [-count xxx] [-echo xxx] [--nothread]\nFor help specify \"-h\"");
}

std::expected<config,int> parse_configurate(int argc,char** argv)
{
	ArgParser ap;
	config cfg;
	ap.addOption("h");
	ap.addOption("i",false);
	ap.addOption("o",false);
	ap.addOption("nothread");
	ap.addOption("count",false);
	ap.addOption("echo",false);


	if(!ap.parse(argc,argv))
	{
		//No argument specified
		usage();
		return std::unexpected(1);
	}

	if(ap.getFlag("h")) 
	{
		//help
		help();
		return std::unexpected(0);
	}

	//input file logic

	switch(ap.getFlag("i") + (ap.getFlag("it") << 1) + (ap.getFlag("ib") << 2))
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

	switch(ap.getFlag("o") + (ap.getFlag("ot") << 1) + (ap.getFlag("ob") << 2))
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

	if(ap.getFlag("count"))
	{
		std::string temp(ap.getValue("count"));
		if(auto [x,y] = std::from_chars(temp.data(),temp.data() + temp.length(),cfg.count);y != std::errc{})
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

	if(ap.getFlag("echo"))
	{
		std::string temp(ap.getValue("echo"));
		int op{ -2 };
		if(auto [x,y] = std::from_chars(temp.data(),temp.data() + temp.length(),op);y != std::errc{} || op < 0 || op > 3)
		{
			//Invalid numeric string for `echo`
			usage();
			return std::unexpected(1);
		}
		cfg.ech = config::echo(op);
	}

	return cfg;

}

std::expected<std::vector<block>,int> readfile(std::string_view filename)
{
	std::vector<block> res;
	std::ifstream is{ filename.data() };
	std::string buf(40, 0);
	while (is)
	{
		is.getline(buf.data(), 40);
		if (auto tp = block_fromhex(buf);tp)
		{
			res.push_back(*tp);
		}
	}

	return res;
}


int main(int argc, char** argv)
{
	auto cfg = parse_configurate(argc, argv);
	if(!cfg)
	{
		return cfg.error();
	}

	auto atk = atk4_2::Attack(*cfg);
	
	auto bk = read_ciphertexts(cfg->inputname); //TODO: Exception handling(invalid stuffs, etc.)
	if (bk.size() > cfg->count)
	{
		bk.resize(cfg->count);
	}

	atk.assign_ciphertext(bk);
	atk.solve();
	return 0;
}