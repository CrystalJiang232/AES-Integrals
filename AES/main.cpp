#ifdef __GNUC__
#include<bits/stdc++.h>
#else
#include <filesystem>
#include <fstream>
#endif
#include"phase.h"
#include"atk4_2.h"

int main(int argc, char** argv)
{
	auto cfg = parse_configurate(argc, argv);
	if(!cfg)
	{
		return cfg.error();
	}

	auto atk = atk4_2::Attack(*cfg);
	
	auto bk = read_ciphertexts(cfg->inputname);

	atk.assign_ciphertext(bk);
	atk.solve();
	return 0;
}