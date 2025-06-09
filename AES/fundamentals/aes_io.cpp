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
    if (!is)
    {
        std::println("Failed to open file {}", filename);
        exit(1);
    }

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
