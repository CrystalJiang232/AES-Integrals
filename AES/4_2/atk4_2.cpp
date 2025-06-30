#include "atk4_2.h"
#include "timer.h"
#include <iostream>
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
        for (block_rvw v : this->copy) //In case that someone lost track of `copy` XD
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

        if(cfg.threading)
        {
            size_t thread_count = std::thread::hardware_concurrency();
            std::vector<std::jthread> jt{};
            if(cfg.ech >= config::echo::total)
            {
                std::println("Decryption started with {} threads...", thread_count);
            }

            auto last = t.point();
            

            for (size_t id = 0,alloc = 0;id < 4;)
            //id: Current cipher group(range(0,4))
            //alloc: First byte of key that's currently being brute-force tested
            {
                for (size_t i : std::views::iota(0ull,thread_count))
                {
                    jt.push_back(std::jthread{ single_thread,total[id],gen_keyrng(i + alloc),id });
                }
                jt.clear(); //Implicitly join all threads dispatched
                
                if(cfg.ech == config::echo::all)
                {
                    std::println("Last {} * 2^24 parallel decryption time spent = {}", thread_count,t.lap_str());
                }
                
                last = t.point();
                if (result_key.status[id]) //Move on to next idx
                {
                    if(cfg.ech >= config::echo::group)
                    {
                        std::println("Key group {} / 4 deciphered, time elapsed = {}", id + 1, t.current_str());
                    }
                    ++id;
                    alloc = 0;
                }
                else
                {
                    alloc += thread_count;
                }
            }
            
        }
        else
        {
            //Single-thread execution
            if(cfg.ech >= config::echo::total)
            {
                std::println("Single thread decryption started...");
            }

            std::jthread th;
            auto last = t.point();

            for (size_t id = 0,alloc = 0;id < 4 && alloc < 256;)
            //id: Current cipher group(range(0,4))
            //alloc: First byte of key that's currently being brute-force tested
            
            //For the end-of-loop condition:
            //id == 4 -> Decryption completed
            //alloc >= 256 -> Iteration complete yet no key found, failed(if key found then alloc will be set to 0)
            {
                th = std::jthread{ single_thread,total[id],gen_keyrng(alloc),id };
                th.join();

                if(cfg.ech == config::echo::all)
                {
                    std::println("Last 2^24 decryption time spent = {}", t.lap_str());
                }
                
                last = t.point();
                if (result_key.status[id]) //Move on to next idx
                {
                    if(cfg.ech >= config::echo::group)
                    {
                        std::println("Key group {} / 4 deciphered, time elapsed = {}", id + 1, t.current_str());
                    }
                    ++id;
                    alloc = 0;
                }
                else
                {
                    ++alloc;
                }
            }
        }
        
        if(!result_key.ready())
        {
            std::println(stderr, "Decipher failed, verify the ciphertext provided");
            exit(1);
        }

        block final = inv_key_expansion<4>(*result_key.exp_key());

        if(cfg.ech >= config::echo::total)
        {
            std::println("Decipher complete, time elapsed = {}",t.current_str());
            std::println("Result key: {}",final);
        }
        
        std::ofstream os{};

        switch (cfg.output)
        {
        case config::io_t::hextest:
            os = std::ofstream{ "key.txt" };
            std::println(os, "{}", final);
            break;

        case config::io_t::binary:
            os = std::ofstream{ "key.key",std::ios_base::binary };
            os.write(reinterpret_cast<const char*>(final.data()), 16);
            break;
        case config::io_t::disabled:
            break; //explicitly
        default:
            break;
        }
    }

    void atk4_2(config c)
    {
        using group_t = std::vector<block>; //partial delta-set
        
        static_assert(std::same_as <group_t, std::vector<block>>, "group_t must be defined as alias for std::vector<block> for text-reading function to operate");
        Attack atk{c};
        group_t ciphers = read_ciphertexts("pdelta.txt");
        atk.assign_ciphertext(ciphers);
        atk.solve();
    }

}

