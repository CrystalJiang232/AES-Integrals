#include "aes.h"
#include <iostream>
#include <format>


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
