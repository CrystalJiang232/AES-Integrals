#ifdef __GNUC__
#include <bits/stdc++.h>
#else
#include <filesystem>
#include <fstream>
#endif

#include "atk4_1.h"
#include "phase.h"

int main(int argc,char** argv)
{
    auto cfg = parse_configurate(argc,argv);
    if(!cfg)
    {
        return cfg.error();
    }

    auto bk = read_ciphertexts(cfg->inputname);
    if(bk.size() != 256)
    {
        std::println("Delta-set condition not satisfied");
        return 1;
    }

    std::span<block,256> sp(bk);
    atk4_1::Attack atk(sp); //explicit conversion required - intriguing constructor?

    atk.solve();
    
}