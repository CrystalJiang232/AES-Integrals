#include "gen_cipher.h"
#include "phase.h"
#include <fstream>
#include <iostream>
const char* hint = 
                "Usage: -a [-q]\n"
                "       -d -p -k [-q]\n"
                "-a             Generate all; equivalent to '-d -p -k'.\n"
                "-d             Generate delta-set, that is, the 4-round AES-128 encrypted ciphertext of a random plaintext, with one particular byte traversing from 0x00 to 0x0f.\n"
                "-p             Generate partial delta-set, a size=66 subset of delta-set by definition.\n"
                "-k             Output the answer key.\n"
                "NOTE: The delta-set and partial delta-set, when generated simotaneously, are guaranteed of been encrypted by the same key(output via '-k' option, if designated), yet the sample plaintext used in generating the ciphertext isn't identical(Thus the partial delta-set generated IS NOT a subset of the delta-set generated).\n"
                "-q             Quiet mode.\n";


const char* full = hint;

block randblock()
{
    std::random_device rd;
    std::uniform_int_distribution<unsigned short> ds(0, 255);

    block res;
    
    for (auto& b : res)
    {
        b = byte(ds(rd));
    }
    return res;
}

static void delta(block key,std::string_view filename)
{
    AES<4> aes{key};

    auto sample = randblock();
    std::ofstream os{filename.data()};

    for(int i : std::views::iota(0,256))
    {
        sample[0] = byte(i);
        std::println(os,"{}",aes.encrypt(sample));
    }
}

static void pdelta(block key,std::string_view filename)
{
    AES<4> aes{key};

    auto sample = randblock();
    std::ofstream os{filename.data()};

    for(int i : std::views::iota(0,66))
    {
        sample[0] = byte(i);
        std::println(os,"{}",aes.encrypt(sample));
    }
}

static void pkey(block key,std::string_view filename)
{
    std::ofstream os{filename.data()};
    std::println(os,"{}",key);
}

int main(int argc,char** argv)
{
    ArgParser ap{
        {"a",ArgType::Flag},
        {"d",ArgType::Flag},
        {"p",ArgType::Flag},
        {"k",ArgType::Flag},
        {"q",ArgType::Flag}
    };

    if(!ap.parse(argc,argv))
    {
        usage();
        exit(1);
    }

    if(ap.get<bool>("h").value_or(false))
    {
        help();
        return 0;
    }

    auto flagv = ((ap.get<bool>("d").value_or(false) << 2) + (ap.get<bool>("p").value_or(false) << 1) + (ap.get<bool>("k").value_or(false))) | (ap.get<bool>("a").value_or(false) * 0x07);
    bool silent = ap.get<bool>("q").value_or(false);

    auto trykey = randblock();
    if(!flagv)
    {
        usage();
        return 1;
    }

    std::string fn1{"delta.txt"},fn2{"pdelta.txt"},fn3{"key.txt"};
    
    if(flagv & 0x04) //delta
    {
        delta(trykey,fn1);
        if(!silent)
        {
            std::println("Delta-set saved to {}",fn1);
        }
    }
    if(flagv & 0x02) //pdelta
    {
        pdelta(trykey,fn2);
        if(!silent)
        {
            std::println("Partial delta-set saved to {}",fn2);
        }
    }
    if(flagv & 0x01) //answer
    {
        pkey(trykey,fn3);
        if(!silent)
        {
            std::println("Key saved to {}",fn3);
        }
    }
    
}