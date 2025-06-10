#include "aes.h"
#include "atk4_1.h"
#include "atkinterface.h"
#include "timer.h"
#include <coroutine>
#include <optional>
#include <array>
#include <unordered_set>
#include <span>

struct KeyGenerator 
{
    struct promise_type;
    using handle_type = std::coroutine_handle<promise_type>;
    
    struct promise_type 
    {
        block current_key;
        
        KeyGenerator get_return_object() 
        { 
            return KeyGenerator{handle_type::from_promise(*this)}; 
        }
        std::suspend_always initial_suspend() noexcept { return {}; }
        std::suspend_always final_suspend() noexcept { return {}; }
        void unhandled_exception() { std::terminate(); }
        
        std::suspend_always yield_value(block_rvw key) 
        {
            std::ranges::copy(key,current_key.data());
            return {};
        }

        void return_void() {}
    };

    handle_type coro_handle;
    
    explicit KeyGenerator(handle_type h) : coro_handle(h) {}

    ~KeyGenerator()
    {
        if(coro_handle)
        {
            coro_handle.destroy();
        }
    }
    
    struct iterator 
    {
        handle_type coro_handle;
        
        bool operator!=(std::default_sentinel_t) const 
        { 
            return !coro_handle.done(); 
        }
        iterator& operator++() 
        {
            coro_handle.resume();
            return *this;
        }
        auto operator*() const 
        {
            return coro_handle.promise().current_key;
        }
    };
    
    iterator begin() 
    {
        if (coro_handle) 
        {
            coro_handle.resume();
        }   
        return iterator{coro_handle};
    }

    std::default_sentinel_t end()
    {
        return {};
    }
};

KeyGenerator generate_keys(std::span<const std::vector<byte>, 16> opt) 
{
    block key{};
    std::array<uc, 16> indices{};
    
    while (true) 
    {
        // Build current key - now using direct indexing
        for (int i : std::views::iota(0,16)) 
        {
            key[i] = opt[i][indices[i]];
        }

        co_yield key;

        for (int i : std::views::iota(0,16)) 
        {
            if (++indices[i] < opt[i].size())
            {
                break;
            }
            if(i == 15) //Indices limit
            {
                co_return;
            }
            indices[i] = 0;
        }
    }
}

namespace atk4_1
{
    Attack::Attack(ciphertext_rvw vw):copy{}
    {
        std::ranges::copy(vw,copy.data());
    }


    void Attack::solve()
    {
        Func_Timer t{};

        keyls ret;
        block sample_key;
        for(auto idx : std::views::iota(0,16))
        {
            uc keyidx = Attack_Interface::inv_shift_rows_idx[idx];

            for(auto i : Attack_Interface::iota_byte)
            {
                sample_key[keyidx] = i;
                uc sumval{0};
                for(block bk : this->copy)
                {
                    Attack_Interface::bytes_xor(bk,sample_key);
                    Attack_Interface::inv_shift_rows(bk);
                    Attack_Interface::inv_sub_bytes(bk);
                    sumval ^= bk[idx];
                }

                if(sumval == 0)
                {
                    ret[keyidx].push_back(i);
                }
            }
        }

        bool flag{false};
        for(block r4 : generate_keys(ret))
        {
            auto trykey = inv_key_expansion<4>(r4);
            if((flag = verify(trykey,this->copy)))
            {
                std::println("Key resolved: {}",trykey);
                break;
            }
        }

        if(!flag)
        {
            std::println("Decipher failed!");
            exit(1);
        }

        std::println("Time spent = {}",t.current_str());
    }

    bool Attack::verify(block key,Attack::ciphertext_rvw v) /*key in argument is the original key, not round key*/
    {    
        //Verify if we can retrieve a delta-set-based plaintext
        //This time the key will be verified via double-rolling back to the original status
        AES<4> aes{key};

        Attack::ciphertexts cp_set;

        for(auto i : Attack_Interface::iota_byte)
        {
            cp_set[i] = aes.decrypt(v[i]);
        }
        
        std::array<int,16> xor_val;
        for(auto i : std::views::iota(0,16))
        {
            int tp{};
            for(auto idx : Attack_Interface::iota_byte)
            {
                tp += cp_set[idx][i]; //add unsigned char
            }
            xor_val[i] = tp; //
        }

        int iterflag{0},sumflag{0};
        for(int x : xor_val)
        {
            if((x & 0xff) == 0)
            {
                sumflag += 1;
            }
            else if(x == 32640)
            {
                iterflag += 1;
            }
            else //Directly GG
            {
                return false;
            }
        }

        if(iterflag != 1 || sumflag != 15)
        {
            return false;
        }

        return true;
    }
}
