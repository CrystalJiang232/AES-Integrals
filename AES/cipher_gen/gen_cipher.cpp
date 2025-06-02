#include "gen_cipher.h"

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
/*Generates ciphertext sets named "delta.txt" and "pdelta.txt" respectively, with key stored in "secret.txt"*/
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

    std::ofstream os1{"delta.txt"};

    for (block_rvw bk : vc)
    {
        std::println(os1, "{}", aes.encrypt(bk));
    }

    std::ofstream os2{ "pdelta.txt" };    
    
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

int main()
{
    gen_cipher_set();
    std::println("delta.txt and pdelta.txt saved under current directory");
}