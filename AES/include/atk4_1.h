#pragma once
#include "atkinterface.h"

namespace atk4_1
{
    class Attack
    {
        using ciphertexts = std::array<block,256>;
        using ciphertext_rvw = std::span<const block,256>;
        using keyls = std::array<std::vector<byte>, 16>;

    public:
        Attack(ciphertext_rvw);
        Attack(const Attack&) = delete;
        Attack& operator=(const Attack&) = delete;
        void solve();
        
    private:
        ciphertexts copy;
        static bool verify(block,ciphertext_rvw);
    };     
}