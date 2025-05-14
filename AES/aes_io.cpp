#include "aes.h"
#include "atkinterface.h"
#include <iostream>
#include <format>
#include <fstream>
#include <random>
#include <algorithm>

std::ostream& operator<<(std::ostream& out, byte b) 
{
    return out << std::format("{:02x}", b.value);
}

std::istream& operator>>(std::istream& in, byte& b) 
{
    int temp{};
    in >> temp;
    b.value = uc(temp & 0xff);
    return in;
}

std::ostream& operator<<(std::ostream& out, block_rvw blk)
{
    return out << std::format("{}", blk);
}

std::vector<block> read_ciphertexts(std::string_view filename)
{
    constexpr size_t bufsz = 77;
    
    std::ifstream is{ filename.data() };
    std::string buf(bufsz,{});
    
    std::vector<block> ret;

    while (is)
    {
        is.getline(buf.data(), bufsz);
        if (auto val = block_fromhex(buf);val)
        {
            ret.push_back(*val);
        }

    }
    return ret;
}

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

void gen_cipher_set()
/*Generates a ciphertext named "pdelta.txt" and key stored in "secret.txt"*/
{
    constexpr bool opt_ans = true; //whether to output the answer key
    
    auto key = randblock();
    auto aes = AES<4>(key);

    auto sample = randblock();
    std::vector<block> vc(256, sample);
    for (int i : std::views::iota(0, 256))
    {
        vc[i][0] = byte(i);
    }
    std::ofstream os{ "delta.txt" }, os2{ "pdelta.txt" };

    for (block_rvw bk : vc)
    {
        std::println(os, "{}", aes.encrypt(bk));
    }

    for (block_rvw bk : vc | std::views::take(66))
    {
        std::println(os2, "{}", aes.encrypt(bk));
    }

    if (opt_ans)
    {
        std::ofstream o3{ "secret.txt" };
        std::println(o3, "{}", key);
    }
}
