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
		all,
		group,
		total,
		noecho
	};


	io_t input{ io_t::disabled };
	std::string inputname;
	io_t output{ io_t::disabled };
	std::string outputname;
	int count{ 66 };
	echo ech{ echo::total };
	bool threading{ true };
};