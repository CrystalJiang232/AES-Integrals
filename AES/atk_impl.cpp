#include "atk4_1.h"
#include "atk4_2.h"
#include "timer.h"
#include "gen_cipher.h"
#include <filesystem>
#include <thread>
#include <future>

template<size_t Rounds = 4> requires (Rounds > 0 && Rounds < 16)
block inv_key_expansion(block rnd_keys)
{
    block cur = rnd_keys;
    for (auto i : std::views::iota(1ull, Rounds + 1) | std::views::reverse)
    {
        block prv;
        
        for (int j : std::views::iota(4, 16))
        {
            prv[j] = cur[j] ^ cur[j - 4];
        }

        cur[0] ^= Attack_Interface::r_con[i - 1];

        for (int j : std::views::iota(0, 4))
        {
            prv[j] = cur[j] ^ Attack_Interface::s_box[prv[12 + (j + 1) % 4]];
        }

        cur = prv;
    }

    return cur;
}

namespace atk4_1
{
    void test_atk4_1()
    {
        using group_t = std::array<block, 256>; //delta-set


        Func_Timer t{};

        Attack atk{};
        block sample{};
        
        gen_cipher_set(); //Generate delta-set

        atk.append_ciphertexts(read_ciphertexts("delta.txt"));
        atk.solve();
    }
}


namespace atk4_2
{
    void Attack::single_thread(int idx,cipher_group_rvw v,int init)
    {
        Func_Timer t{};
        auto iota_byte = Attack_Interface::iota_byte;
        auto key_rng = std::views::cartesian_product(iota_byte | std::views::drop(init) | std::views::take(64), iota_byte, iota_byte, iota_byte)
            | std::views::transform([](auto&& x) {auto&& [a, b, c, d] = x; return word{ a,b,c,d };});

        byte last;
        for(word wd : key_rng)
        {
            if (verify(wd, v))
            {
                std::println("Group {} key resolved!", idx / 4);
                result_key.status[idx / 4] = wd;
                break;
            }
        }
        //std::print("Thread {} execution complete with t = ", idx);
    }

    void Attack::solve()
    {
        //Probability
        

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

        //multithreading
        constexpr int thread_count = 16;

        std::array<std::jthread, thread_count> jt{};
        for (auto i : std::views::iota(0,thread_count))
        {
            jt[i] = std::jthread{ single_thread,i ,total[i / 4],(i % 4) * 64 };
        }

        /*
        std::array<bool, 4> cp{};
        while (!result_key.ready())
        {
            for (int i : std::views::iota(0, 4) | std::views::filter([&cp](int x){return cp[x] == false;}))
            {
                if (result_key.idx_complete(i))
                {
                    cp[i] = true;
                    for (int j : std::views::iota(0, 4))
                    {
                        jt[i * 4 + j].request_stop();
                    }
                }
            }
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
        */
        /*
        std::array <std::future<void>, thread_count> jt{};
        for (auto i : std::views::iota(0, thread_count))
        {
            jt[i] = std::async(&Attack::single_thread,std::ref(*this),i,total[i / 4],(i % 4) * 64);
        }
        */
        std::println("Thread execution started...");
    }

    void test_atk4_2()
    {
        using group_t = std::vector<block>; //partial delta-set
        
        static_assert(std::same_as <group_t, std::vector<block>>, "group_t must be defined as alias for std::vector<block> for text-reading function to operate");

        Func_Timer t{};
        Attack atk{};
        block sample{};
        
        if (!std::filesystem::exists("delta.txt"))
        {
            gen_cipher_set();
        }

        group_t ciphers = read_ciphertexts("pdelta.txt");
        atk.assign_ciphertext(ciphers);
        atk.solve();
        atk.printkey();
    }

    void Attack::printkey()
    {
        std::println("Original key: {}",inv_key_expansion<4>(*result_key.exp_key()));
    }
}

