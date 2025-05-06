#include "atk4_1.h"
#include "atk4_2.h"
#include "timer.h"

namespace atk4_1
{
    void atk4_1::test_atk4_1()
    {
        using group_t = std::array<block, 256>; //delta-set

        Func_Timer t{};

        std::string str{};

        auto key = block_fromhex(str).value_or(block{});
        auto aes = AES<4>(key);

        std::println("4th round key:\n{}", aes.output_rk4());

        Attack atk{};
        block sample{};
        group_t ciphers{};

        for (auto q : Attack_Interface::iota_byte | std::views::take(2))
        {
            sample[15] = byte(q);

            for (auto i : Attack_Interface::iota_byte)
            {
                sample[0] = byte(i);
                ciphers[i] = aes.encrypt(sample);
            }
            atk.append_ciphertexts(ciphers);
        }

        atk.solve();

    }
}


namespace atk4_2
{
    void atk4_2::Attack::single_thread(int idx,cipher_group_rvw v)
    {
        Func_Timer t{};
        auto iota_byte = Attack_Interface::iota_byte;
        auto key_rng = std::views::cartesian_product(iota_byte, iota_byte, iota_byte, iota_byte)
            | std::views::transform([](auto&& x) {auto&& [a, b, c, d] = x; return word{ a,b,c,d };});

        for (word ky : key_rng | std::views::take(0x100000))
        {
            if (verify(ky, v))
            {
                std::println("Group {} key resolved = {}", idx, ky);
                break;
            }

        }

        std::print("Thread {} execution complete with t = ", idx);
    }

    void atk4_2::Attack::solve()
    {
        //Probability



        //Solution
        

        //Process ciphertexts
        std::array<cipher_group, 4> total;
        for (block_rvw v : copy)
        {
            for (auto i : iota_word)
            {
                total[i].push_back({}); //Append something
                for (auto x : iota_word)
                {
                    total[i].back()[x] = v[mix[i * 4 + x]];
                }
            }
        }

        //4 threads
        std::array<std::jthread, 4> jt{};
        for (auto i : iota_word)
        {
            jt[i] = std::jthread{ single_thread,i ,total[i]};
        }

       
        std::println("Thread execution started...");
        

    }

    void atk4_2::test_atk4_2()
    {
        using group_t = std::vector<block>; //partial delta-set

        Func_Timer t{};

        std::string str{};

        auto key = block_fromhex(str).value_or(block{});
        auto aes = AES<4>(key);

        std::println("4th round key:\n{}", aes.output_rk4());

        Attack atk{};
        block sample{};

        group_t ciphers(63, {});

        for (auto i : std::views::iota(0, 63))
        {
            sample[0] = byte(i);
            ciphers[i] = aes.encrypt(sample);
        }

        atk.assign_ciphertext(ciphers);
        atk.solve();


    }
}

