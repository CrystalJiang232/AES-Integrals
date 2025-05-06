#pragma once
#include "atkinterface.h"
#include <unordered_set>

namespace atk4_1
{
    class Attack
    {
        using ciphertexts = std::vector<block>;
        using ciphertext_rvw = std::span<const block>;
        using keyls = std::array<std::unordered_set<byte>, 16>;

    public:
        constexpr Attack() :copy{}
        {

        }

        constexpr Attack(const Attack&) = delete;
        constexpr Attack& operator=(const Attack&) = delete;

        constexpr void append_ciphertexts(ciphertext_rvw vw)
        {
            if (vw.size() != 256)
            {
                throw std::invalid_argument{"Each group of ciphertext must be a delta-set of length 256"};
            }
            copy.push_back({ std::from_range,vw });
        }

        void solve()
        {
            if (copy.empty())
            {
                throw std::runtime_error{ "No ciphertexts provided yet!" };
            }

            for (ciphertext_rvw v: copy)
            {
                if (!status)
                {
                    status = verify(v);
                }
                else
                {
                    auto temp = verify(v);
                    for (auto&& [s1,s2] : std::views::zip(*status,temp))
                    {
                        for (byte i : s1)
                        {
                            if (!s2.contains(i))
                            {
                                s1.erase(i);
                            }
                        }
                    }
                }
            }

            if (std::ranges::all_of(*status, [](auto x) {return x == 1;}, &std::unordered_set<byte>::size)) //Only key combination possible
            {
                std::println("Key resolved:");
                for (const auto& i : *status)
                {
                    for (auto t : i)
                    {
                        std::print("{}", t);
                    }
                }

                std::println();
            }
        }


    private:
        
        constexpr keyls verify(ciphertext_rvw vw) const
        {
            keyls ret;
            block sample_key;

            for (auto idx : std::views::iota(0, 16))
            {
                uc keyidx = Attack_Interface::inv_shift_rows_idx[idx];
                //Checking at index 0-15 through the ciphertexts rewinded to round 3
                //Keys that await to be enumerated should be forward-mapped to round 4(the keys trying to ...

                for (auto i : Attack_Interface::iota_byte)
                {
                    sample_key[keyidx] = i;
                    uc sumval = 0;
                    for (block bk : vw)
                    {
                        Attack_Interface::bytes_xor(bk, sample_key); //AddRoundKey
                        Attack_Interface::inv_shift_rows(bk);
                        Attack_Interface::inv_sub_bytes(bk);

                        sumval ^= bk[idx];
                    }

                    if (sumval == 0)
                    {
                        ret[keyidx].insert(i);
                    }
                }
            }
            
            return ret;
        }
        
        std::vector<ciphertexts> copy;
        std::optional<keyls> status;
    };


    void test_atk4_1();
}