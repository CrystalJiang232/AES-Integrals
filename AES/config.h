#pragma once
#include<string>

struct config
{
	enum class io_t
	{
		hextest,
		binary
	};

	enum class echo
	{
		all,
		group,
		total,
		noecho
	};


	io_t input;
	std::string inputname;
	io_t output;
	std::string outputname;
	int count;
	echo ech;
	bool threading;
};