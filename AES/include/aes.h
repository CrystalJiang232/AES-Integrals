#pragma once
#ifdef __GNUC__
#include <bits/stdc++.h>
#else
#include <array>
#include <span>
#include <algorithm>
#include <ranges>
#include <stdexcept>
#include <print>
#include <expected>
#include <vector>
#endif

#ifdef __MSC_VER__
#pragma warning(disable:4244)
#endif

using uc = unsigned char;

struct byte 
{
    explicit constexpr byte(unsigned char _val) : value{_val} {}
    constexpr byte() : byte(0) {}

    // Default special member functions
    constexpr byte(const byte&) = default;
    constexpr byte(byte&&) noexcept = default;
    constexpr byte& operator=(const byte&) = default;
    constexpr byte& operator=(byte&&) noexcept = default;
    constexpr ~byte() = default;

    // Conversion
    constexpr operator unsigned char() const noexcept { return value; }

    // Bitwise operators
    friend constexpr byte operator&(byte x, byte y) noexcept
    {
        return byte{ uc(x.value & y.value) };
    }

    // Compound assignments
    constexpr byte& operator^=(byte right) noexcept
    {
        value ^= right.value;
        return *this;
    }
    friend constexpr byte operator^(byte x, byte y) noexcept
    {
        return byte{ uc(x.value ^ y.value) };
    }

    friend constexpr byte operator|(byte x, byte y) noexcept
    {
        return byte{ uc(x.value | y.value) };
    }

    friend constexpr byte operator>>(byte x, byte y) noexcept
    {
        return byte{ uc(x.value >> y) };
    }

    friend constexpr byte operator<<(byte x, byte y) noexcept
    {
        return byte{ uc(x.value << y) };
    }

    //compound assignment operators

    constexpr byte& operator&=(byte right) noexcept
    {
        value &= right.value;
        return *this;
    }

    constexpr byte& operator|=(byte right) noexcept
    {
        value |= right.value;
        return *this;
    }

    constexpr byte& operator>>=(unsigned char shift) noexcept
    {
        value >>= shift;
        return *this;
    }

    constexpr byte& operator<<=(unsigned char shift) noexcept
    {
        value <<= shift;
        value &= 0xff;
        return *this;
    }

    // Comparison
    friend constexpr auto operator<=>(const byte&, const byte&) noexcept = default;

    // Literal operator
    friend constexpr byte operator""_t(unsigned long long) noexcept;

    unsigned char value;
};

template<>
struct std::hash<byte> : std::hash<unsigned char>{};

constexpr byte operator""_t(unsigned long long _val) noexcept 
{
    return byte{ _val & 0xff };
}

namespace _gmul 
{
    using table_t = std::array<byte, 2048>;
    constexpr byte gmul_fn(byte a, uc b) noexcept
    {
        byte p{};
        while (b)
        {
            if (b & 1)
            {
                p ^= a;
            }
            byte hi_bit{ a & 0x80_t };
            a <<= 1;
            if (hi_bit)
            {
                a ^= 0x1b_t;
            }
            b >>= 1;
        }
        return p;
    }

    static constexpr std::array<int, 8> val_idxs = { 2,3,1,1,0x0e,0x0b,0x0d,0x09 };

    consteval table_t table_gen() noexcept
    {
        table_t res{};

        for (int i : std::views::iota(0, 2048))
        {
            res[i] = gmul_fn(byte(i % 256), byte(val_idxs[i / 256]));
        }

        return res;
    }

    static constexpr table_t table = table_gen();

    struct gmul_t
    {
        constexpr byte operator()(byte a, int b) const noexcept
        {
            return table[b * 256 + a];
        }
    };

    static constexpr inline gmul_t gmul = {};

}

//Alias declarations: block, block_vw, block_rvw

using _gmul::gmul;
using block = std::array<byte, 16>;
using block_vw = std::span<byte, 16>;
using block_rvw = std::span<const byte, 16>;



template<size_t Rounds = 10> requires (Rounds >= 1)
class AES
{
public:
    using rnd_key_t = std::array<block, Rounds + 1>;
    constexpr AES(block_rvw key) :rnd_keys(key_expansion(key)) {}

    constexpr block encrypt(block_rvw plain)
    {
        //block state = std::ranges::to<block>(plain);
        block state{};
        for (int i : std::views::iota(0, 16))
        {
            state[i] = plain[i];
        }

        add_round_key(state, 0);

        for (auto i : std::views::iota(1ull, Rounds))
        {
            sub_bytes(state);
            shift_rows(state);
            mix_columns(state);
            add_round_key(state, i);
        }

        sub_bytes(state);
        shift_rows(state);
        add_round_key(state, Rounds);
        return state;
    }

    constexpr block decrypt(block_rvw cipher)
    {
        //block state = std::ranges::to<block>(cipher);

        block state{};
        for (int i : std::views::iota(0, 16))
        {
            state[i] = cipher[i];
        }

        add_round_key(state, Rounds);
        inv_shift_rows(state);
        inv_sub_bytes(state);

        for (int i : std::views::iota(1ull, Rounds) | std::views::reverse)
        {
            add_round_key(state, i);
            inv_mix_columns(state);
            inv_shift_rows(state);
            inv_sub_bytes(state);
        }

        add_round_key(state, 0);
        return state;
    }

    constexpr block output_rk4() const noexcept
    {
        return rnd_keys[4];
    }

private:
    static constexpr byte s_box[] =
    {
        0x63_t, 0x7c_t, 0x77_t, 0x7b_t, 0xf2_t, 0x6b_t, 0x6f_t, 0xc5_t, 0x30_t, 0x01_t, 0x67_t, 0x2b_t, 0xfe_t, 0xd7_t, 0xab_t, 0x76_t,
        0xca_t, 0x82_t, 0xc9_t, 0x7d_t, 0xfa_t, 0x59_t, 0x47_t, 0xf0_t, 0xad_t, 0xd4_t, 0xa2_t, 0xaf_t, 0x9c_t, 0xa4_t, 0x72_t, 0xc0_t,
        0xb7_t, 0xfd_t, 0x93_t, 0x26_t, 0x36_t, 0x3f_t, 0xf7_t, 0xcc_t, 0x34_t, 0xa5_t, 0xe5_t, 0xf1_t, 0x71_t, 0xd8_t, 0x31_t, 0x15_t,
        0x04_t, 0xc7_t, 0x23_t, 0xc3_t, 0x18_t, 0x96_t, 0x05_t, 0x9a_t, 0x07_t, 0x12_t, 0x80_t, 0xe2_t, 0xeb_t, 0x27_t, 0xb2_t, 0x75_t,
        0x09_t, 0x83_t, 0x2c_t, 0x1a_t, 0x1b_t, 0x6e_t, 0x5a_t, 0xa0_t, 0x52_t, 0x3b_t, 0xd6_t, 0xb3_t, 0x29_t, 0xe3_t, 0x2f_t, 0x84_t,
        0x53_t, 0xd1_t, 0x00_t, 0xed_t, 0x20_t, 0xfc_t, 0xb1_t, 0x5b_t, 0x6a_t, 0xcb_t, 0xbe_t, 0x39_t, 0x4a_t, 0x4c_t, 0x58_t, 0xcf_t,
        0xd0_t, 0xef_t, 0xaa_t, 0xfb_t, 0x43_t, 0x4d_t, 0x33_t, 0x85_t, 0x45_t, 0xf9_t, 0x02_t, 0x7f_t, 0x50_t, 0x3c_t, 0x9f_t, 0xa8_t,
        0x51_t, 0xa3_t, 0x40_t, 0x8f_t, 0x92_t, 0x9d_t, 0x38_t, 0xf5_t, 0xbc_t, 0xb6_t, 0xda_t, 0x21_t, 0x10_t, 0xff_t, 0xf3_t, 0xd2_t,
        0xcd_t, 0x0c_t, 0x13_t, 0xec_t, 0x5f_t, 0x97_t, 0x44_t, 0x17_t, 0xc4_t, 0xa7_t, 0x7e_t, 0x3d_t, 0x64_t, 0x5d_t, 0x19_t, 0x73_t,
        0x60_t, 0x81_t, 0x4f_t, 0xdc_t, 0x22_t, 0x2a_t, 0x90_t, 0x88_t, 0x46_t, 0xee_t, 0xb8_t, 0x14_t, 0xde_t, 0x5e_t, 0x0b_t, 0xdb_t,
        0xe0_t, 0x32_t, 0x3a_t, 0x0a_t, 0x49_t, 0x06_t, 0x24_t, 0x5c_t, 0xc2_t, 0xd3_t, 0xac_t, 0x62_t, 0x91_t, 0x95_t, 0xe4_t, 0x79_t,
        0xe7_t, 0xc8_t, 0x37_t, 0x6d_t, 0x8d_t, 0xd5_t, 0x4e_t, 0xa9_t, 0x6c_t, 0x56_t, 0xf4_t, 0xea_t, 0x65_t, 0x7a_t, 0xae_t, 0x08_t,
        0xba_t, 0x78_t, 0x25_t, 0x2e_t, 0x1c_t, 0xa6_t, 0xb4_t, 0xc6_t, 0xe8_t, 0xdd_t, 0x74_t, 0x1f_t, 0x4b_t, 0xbd_t, 0x8b_t, 0x8a_t,
        0x70_t, 0x3e_t, 0xb5_t, 0x66_t, 0x48_t, 0x03_t, 0xf6_t, 0x0e_t, 0x61_t, 0x35_t, 0x57_t, 0xb9_t, 0x86_t, 0xc1_t, 0x1d_t, 0x9e_t,
        0xe1_t, 0xf8_t, 0x98_t, 0x11_t, 0x69_t, 0xd9_t, 0x8e_t, 0x94_t, 0x9b_t, 0x1e_t, 0x87_t, 0xe9_t, 0xce_t, 0x55_t, 0x28_t, 0xdf_t,
        0x8c_t, 0xa1_t, 0x89_t, 0x0d_t, 0xbf_t, 0xe6_t, 0x42_t, 0x68_t, 0x41_t, 0x99_t, 0x2d_t, 0x0f_t, 0xb0_t, 0x54_t, 0xbb_t, 0x16_t
    };

    static constexpr byte inv_s_box[] =
    {
        0x52_t, 0x09_t, 0x6a_t, 0xd5_t, 0x30_t, 0x36_t, 0xa5_t, 0x38_t, 0xbf_t, 0x40_t, 0xa3_t, 0x9e_t, 0x81_t, 0xf3_t, 0xd7_t, 0xfb_t,
        0x7c_t, 0xe3_t, 0x39_t, 0x82_t, 0x9b_t, 0x2f_t, 0xff_t, 0x87_t, 0x34_t, 0x8e_t, 0x43_t, 0x44_t, 0xc4_t, 0xde_t, 0xe9_t, 0xcb_t,
        0x54_t, 0x7b_t, 0x94_t, 0x32_t, 0xa6_t, 0xc2_t, 0x23_t, 0x3d_t, 0xee_t, 0x4c_t, 0x95_t, 0x0b_t, 0x42_t, 0xfa_t, 0xc3_t, 0x4e_t,
        0x08_t, 0x2e_t, 0xa1_t, 0x66_t, 0x28_t, 0xd9_t, 0x24_t, 0xb2_t, 0x76_t, 0x5b_t, 0xa2_t, 0x49_t, 0x6d_t, 0x8b_t, 0xd1_t, 0x25_t,
        0x72_t, 0xf8_t, 0xf6_t, 0x64_t, 0x86_t, 0x68_t, 0x98_t, 0x16_t, 0xd4_t, 0xa4_t, 0x5c_t, 0xcc_t, 0x5d_t, 0x65_t, 0xb6_t, 0x92_t,
        0x6c_t, 0x70_t, 0x48_t, 0x50_t, 0xfd_t, 0xed_t, 0xb9_t, 0xda_t, 0x5e_t, 0x15_t, 0x46_t, 0x57_t, 0xa7_t, 0x8d_t, 0x9d_t, 0x84_t,
        0x90_t, 0xd8_t, 0xab_t, 0x00_t, 0x8c_t, 0xbc_t, 0xd3_t, 0x0a_t, 0xf7_t, 0xe4_t, 0x58_t, 0x05_t, 0xb8_t, 0xb3_t, 0x45_t, 0x06_t,
        0xd0_t, 0x2c_t, 0x1e_t, 0x8f_t, 0xca_t, 0x3f_t, 0x0f_t, 0x02_t, 0xc1_t, 0xaf_t, 0xbd_t, 0x03_t, 0x01_t, 0x13_t, 0x8a_t, 0x6b_t,
        0x3a_t, 0x91_t, 0x11_t, 0x41_t, 0x4f_t, 0x67_t, 0xdc_t, 0xea_t, 0x97_t, 0xf2_t, 0xcf_t, 0xce_t, 0xf0_t, 0xb4_t, 0xe6_t, 0x73_t,
        0x96_t, 0xac_t, 0x74_t, 0x22_t, 0xe7_t, 0xad_t, 0x35_t, 0x85_t, 0xe2_t, 0xf9_t, 0x37_t, 0xe8_t, 0x1c_t, 0x75_t, 0xdf_t, 0x6e_t,
        0x47_t, 0xf1_t, 0x1a_t, 0x71_t, 0x1d_t, 0x29_t, 0xc5_t, 0x89_t, 0x6f_t, 0xb7_t, 0x62_t, 0x0e_t, 0xaa_t, 0x18_t, 0xbe_t, 0x1b_t,
        0xfc_t, 0x56_t, 0x3e_t, 0x4b_t, 0xc6_t, 0xd2_t, 0x79_t, 0x20_t, 0x9a_t, 0xdb_t, 0xc0_t, 0xfe_t, 0x78_t, 0xcd_t, 0x5a_t, 0xf4_t,
        0x1f_t, 0xdd_t, 0xa8_t, 0x33_t, 0x88_t, 0x07_t, 0xc7_t, 0x31_t, 0xb1_t, 0x12_t, 0x10_t, 0x59_t, 0x27_t, 0x80_t, 0xec_t, 0x5f_t,
        0x60_t, 0x51_t, 0x7f_t, 0xa9_t, 0x19_t, 0xb5_t, 0x4a_t, 0x0d_t, 0x2d_t, 0xe5_t, 0x7a_t, 0x9f_t, 0x93_t, 0xc9_t, 0x9c_t, 0xef_t,
        0xa0_t, 0xe0_t, 0x3b_t, 0x4d_t, 0xae_t, 0x2a_t, 0xf5_t, 0xb0_t, 0xc8_t, 0xeb_t, 0xbb_t, 0x3c_t, 0x83_t, 0x53_t, 0x99_t, 0x61_t,
        0x17_t, 0x2b_t, 0x04_t, 0x7e_t, 0xba_t, 0x77_t, 0xd6_t, 0x26_t, 0xe1_t, 0x69_t, 0x14_t, 0x63_t, 0x55_t, 0x21_t, 0x0c_t, 0x7d_t
    };

    static constexpr byte r_con[] =
    {
        0x01_t, 0x02_t, 0x04_t, 0x08_t, 0x10_t, 0x20_t, 0x40_t, 0x80_t, 0x1b_t, 0x36_t, 0x6c_t
    };

    static constexpr rnd_key_t key_expansion(block_rvw init) noexcept
    {
        rnd_key_t rk{};
        std::ranges::copy(init, rk[0].begin());

        for (auto i : std::views::iota(1ull, Rounds + 1))
        {
            block_rvw prv = rk[i - 1];
            block_vw cur = rk[i]; //seperating to make read-only sematic more explicit

            for (int j : iota_word)
            {
                cur[j] = prv[j] ^ s_box[prv[12 + (j + 1) % 4]];
            }

            cur[0] ^= r_con[i - 1];

            for (int j : std::views::iota(4, 16))
            {
                cur[j] = prv[j] ^ cur[j - 4];
            }
        }

        return rk;
    }

    static constexpr void sub_bytes(block_vw s) noexcept
    {
        for (byte& i : s)
        {
            i = s_box[i];
        }
    }

    static constexpr void inv_sub_bytes(block_vw s) noexcept
    {
        for (byte& i : s)
        {
            i = inv_s_box[i];
        }
    }

    static constexpr void shift_rows(block_vw s) noexcept
    {
        std::ranges::copy(block{
           s[0], s[5], s[10], s[15],
           s[4], s[9], s[14], s[3],
           s[8], s[13], s[2], s[7],
           s[12], s[1], s[6], s[11]
            }, s.begin());
    }

    static constexpr uc shift_rows_idx[] = {
        0,5,10,15,
        4,9,14,3,
        8,13,2,7,
        12,1,6,11
    };

    static constexpr void inv_shift_rows(block_vw s) noexcept
    {
        std::ranges::copy(block{
            s[0], s[13], s[10], s[7],
            s[4], s[1], s[14], s[11],
            s[8], s[5], s[2], s[15],
            s[12], s[9], s[6], s[3]
            }, s.begin());
    }

    static constexpr uc inv_shift_rows_idx[] = {
        0,13,10,7,
        4,1,14,11,
        8,5,2,15,
        12,9,6,3
    };

    static constexpr void mix_columns(block_vw s) noexcept
    {
        block res{};

        for (int i : std::views::iota(0, 16) | std::views::stride(4))
        {
            for (int x : iota_word)
            {
                for (int k : iota_word)
                {
                    res[i + x] ^= gmul(s[i + (x + k) % 4], k);
                }
            }
        }

        std::ranges::copy(res, s.begin());
    }

    static constexpr void inv_mix_columns(block_vw s) noexcept
    {
        block res{};

        for (int i : std::views::iota(0, 16) | std::views::stride(4))
        {
            for (int x : iota_word)
            {
                for (int k : iota_word)
                {
                    res[i + x] ^= gmul(s[i + (x + k) % 4], k + 4);
                }
            }
        }

        std::ranges::copy(res, s.begin());
    }

    static constexpr void bytes_xor(block_vw to, block_rvw other) noexcept
    {
        for (auto&& [x, y] : std::views::zip(to, other))
        {
            x ^= y;
        }
    }

    static constexpr block bytes_xor_ret(block_rvw x, block_rvw y) noexcept
    {
        block res{};
        for (int i : std::views::iota(0, 16))
        {
            res[i] = x[i] ^ y[i];
        }
        return res;
    }

    constexpr void add_round_key(block_vw block, size_t index)
    {
        bytes_xor(block, rnd_keys[index]);
    }

    static constexpr auto iota_word = std::views::iota(0, 4);
    static constexpr auto iota_byte = std::views::iota(0, 256) | std::views::transform([](int x) {return byte(x);});

    rnd_key_t rnd_keys;
};

template<>
struct std::formatter<::byte> //F**king ADL...
{
    constexpr auto parse(std::format_parse_context& fpc)
    {
        return fpc.begin();
    }

    auto format(::byte b, std::format_context& fc) const
    {
        return std::format_to(fc.out(), "{:02x}", b.value);
    }
};


template<>
struct std::formatter<block_rvw>
{
    constexpr static auto hexify = [](::byte x) {return std::format("{:02x}", x.value);};

    constexpr auto parse(std::format_parse_context& fpc)
    {
        return fpc.begin();
    }

    auto format(block_rvw v, std::format_context& fc) const
    {
        return std::ranges::copy(v | std::views::transform(hexify) | std::views::join, fc.out()).out;
    }
};

template<>
struct std::formatter<block> : std::formatter<block_rvw> {};

constexpr std::expected<block,std::string> block_fromhex(std::string_view sv)
{
    auto refloc = sv.data();
    block res{};    
    for (auto& bt : res)
    {
        uc temp{};
        if (sv.empty()) //Early terminated, consider returning std::unexpected?
        {
            return std::unexpected{ "Insufficient text to generate a block" };
        }

        if (auto val = std::from_chars(sv.data(), sv.data() + 2, temp, 16);val.ptr != sv.data() + 2)
        {
            return std::unexpected{ std::format("Non-hex character at position {}",val.ptr - refloc) };
        }

        bt = byte{ temp };
        auto fn = [](char ch) -> bool {return std::isblank(ch);};
        sv.remove_prefix(2);
        sv = sv.substr(std::ranges::find_if_not(sv, fn) - sv.begin());
    }    

    return res;
}
