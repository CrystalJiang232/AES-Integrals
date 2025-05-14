#include "atk4_1.h"
#include "atk4_2.h"
#include "timer.h"
#include "gen_cipher.h"
#include "config.h"

#ifdef __GNUC__
#include<bits/stdc++.h>
#else
#include <filesystem>
#include <thread>
#include <future>
#include <fstream>
#include <iostream>
#endif
template<size_t Rounds = 4> requires (Rounds > 0 && Rounds <= 10)
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
    void Attack::single_thread(cipher_group_rvw v,keyrng_t rng,size_t idx)
    {
        for(word wd : rng)
        {
            if (verify(wd, v))
            {
                result_key.status[idx] = wd;
                break;
            }
        }
    }

    void Attack::solve()
    {
        //Process ciphertexts
        Func_Timer t{};
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

        //multithreading dispatch
        constexpr size_t thread_count = 20;
        std::array<std::jthread, thread_count> jt{};
        
        if(int(cfg.ech) != int(config::echo::noecho))
        {
            std::println("Decryption started with {} threads...", thread_count);
        }

        auto last = t.elapsed_time();

        for (size_t id = 0,alloc = 0;id < 4;)
        {
            for (size_t i : std::views::iota(0ull,thread_count))
            {
                jt[i] = std::jthread{ single_thread,total[id],gen_keyrng(i + alloc),id };
            }

            jt = {}; //Implicitly join all threads dispatched
            
            if(int(cfg.ech) == int(config::echo::all))
            {
                std::println("Last {} * 2^24 parallel decryption time spent = {} ms", thread_count, std::chrono::duration_cast<std::chrono::milliseconds>(t.elapsed_time() - last).count());
            }
            
            last = t.elapsed_time();
            if (result_key.status[id]) //Move on to next idx
            {
                auto val = (t.count_nanos() / 1'000'000) / 1000.0; //Explicitly truncate trailing floats
                if(int(cfg.ech) <= int(config::echo::group))
                {
                    std::println("Key group {} / 4 deciphered, time elapsed = {:.3}s", id + 1, val);
                }
                ++id;
                alloc = 0;
            }
            else
            {
                alloc += thread_count;
            }
        }
        
        if(!result_key.ready())
        {
            std::println(stderr,"Decipher failed!");
            exit(1);
        }

        block final = inv_key_expansion<4>(*result_key.exp_key());

        if(int(cfg.ech) <= int(config::echo::total))
        {
            std::println("Decipher complete, time elapsed = {:.3}s",(t.count_nanos() / 1'000'000) / 1000.0);
            std::println("Result key: {}",final);
        }
        
        if(cfg.outputname != "")
        {
            if(int(cfg.output) == int(config::io_t::hextest))
            {
                std::ofstream os{"key.txt"};
                std::println(os,"{}",final);
            }
        }
        
        t.reset(); //Disable final output
    }

    void atk4_2(config c)
    {
        using group_t = std::vector<block>; //partial delta-set
        
        static_assert(std::same_as <group_t, std::vector<block>>, "group_t must be defined as alias for std::vector<block> for text-reading function to operate");
        Attack atk{c};
        block sample{};
        
        if (!std::filesystem::exists("delta.txt"))
        {
            gen_cipher_set();
        }

        group_t ciphers = read_ciphertexts("pdelta.txt");
        atk.assign_ciphertext(ciphers);
        atk.solve();
    }

}

