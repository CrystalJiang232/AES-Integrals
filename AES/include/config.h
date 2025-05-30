#pragma once
#include<string>

struct config
{
	enum class io_t
	{
		hextest,
		binary,
		disabled
	};

	enum class echo
	{
		noecho = 0,
		total = 1,
		group = 2,
		all = 3
	};


	io_t input{ io_t::disabled };
	std::string inputname;
	io_t output{ io_t::disabled };
	std::string outputname;
	int count{ 66 };
	echo ech{ echo::total };
	bool threading{ true };
};