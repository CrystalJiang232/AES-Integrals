#pragma once
#include "atkinterface.h"
#include <thread>

namespace atk4_2
{
    static constexpr auto gen_keyrng(uc first) noexcept
    {
        auto ib = Attack_Interface::iota_byte;
        return std::views::cartesian_product(ib, ib, ib) | std::views::transform([first](auto&& a) {auto&& [x, y, z] = a;return std::array<byte,4>{ byte(first),x,y,z };});
    }

    using keyrng_t = decltype(gen_keyrng(0));

    static constexpr byte gdb_fn(byte b) noexcept
    {
        return byte((b & 0x80) ? (((b << 1) ^ 0x1B) & 0xFF) : (b << 1));
    }

    static consteval std::array<byte, 256> gdb_gen() noexcept
    {
        std::array<byte, 256> ret{};
        for (byte i : Attack_Interface::iota_byte)
        {
            ret[i] = gdb_fn(i);
        }
        return ret;
    }

    static constexpr std::array<byte, 256> gdb = gdb_gen();

	class Attack
	{
    public:
        using ciphertexts = std::vector<block>;
        using ciphertext_rvw = std::span<const block>;
        using keyls = std::array<std::unordered_set<byte>, 16>;

        using word = std::array<byte, 4>;
        using word_vw = std::span<byte, 4>;
        using word_rvw = std::span<const byte, 4>;

        using cipher_group = std::vector<word>; //Ciphertext with identical color's archive
        using cipher_group_rvw = std::span<const word>;

        class EncKey
        {
        public:
            constexpr EncKey() = default;
            constexpr EncKey(const EncKey&) = delete;
            constexpr EncKey& operator=(const EncKey&) = delete;

            constexpr std::optional<block> exp_key() const noexcept
            {
                if (!ready())
                {
                    return std::nullopt;
                }

                block result;

                for (int i : std::views::iota(0, 4))
                {
                    for (int j : std::views::iota(0, 4))
                    {
                        result[Attack_Interface::inv_shift_rows_idx[i * 4 + j]] = (*status[0])[j];
                    }
                }

                return result;
            }

            constexpr bool ready() const noexcept
            {
                return std::ranges::all_of(status, &std::optional<word>::has_value);
            }

            constexpr bool idx_complete(size_t idx) const noexcept
            {
                return status[idx].has_value();
            }

        private:
            std::array<std::optional<word>,4> status;

            friend Attack;
        };
        
        constexpr Attack() :copy{}
        {

        }

        constexpr Attack(const Attack&) = delete;
        constexpr Attack& operator=(const Attack&) = delete;

        constexpr void assign_ciphertext(ciphertext_rvw vw)
        {
            copy.clear();
            //copy.append_range(vw);
            copy.resize(vw.size(),{});
            for(auto i : std::views::iota(0ull,vw.size()))
            {
                std::ranges::copy(vw[i],copy[i].begin());
            }
        }

        static constexpr int mix[] =
        {
            0,13,10,7,
            4,1,14,11,
            8,5,2,15,
            12,9,6,3
        };

        static void single_thread(cipher_group_rvw,keyrng_t,size_t); //Single thread execution
        void solve();
        void printkey();
    private:

        static constexpr auto iota_word = std::views::iota(0, 4);

        static constexpr void bytes_xor(word_vw to, word_rvw from) noexcept
        {
            for (auto i : iota_word)
            {
                to[i] ^= from[i];
            }
        }

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

        static constexpr void inv_sub_bytes(word_vw w) noexcept
        {
            for (auto& i : w)
            {
                i = inv_s_box[i];
            }
        }
        static constexpr void inv_mix_columns(word_vw buf) noexcept
        {
            word res;
            for(int i : std::views::iota(0,4))
            {
                for(int j : std::views::iota(0,4))
                {
                    res[i] ^= gmul(buf[(i+j) % 4],j+4);
                }
            }
            std::ranges::copy(res,buf.begin());
        }
        
        static constexpr bool verify(word partial_key,cipher_group_rvw vws) noexcept //vws is the pre-processed(shift-row completed) ciphertext "identical-color" 4s
        {
            using check_map = std::array<bool, 1024>;
            check_map mp{};

            for (word vw : vws)
            {
                bytes_xor(vw, partial_key);
                inv_sub_bytes(vw);
                inv_mix_columns(vw);
                for (auto idx : iota_word)
                {
                    if (auto& position = mp[idx * 256 + vw[idx]];position)
                    {
                        return false;
                    }
                    else
                    {
                        position = true;
                    }
                }
            }
            return true;
        }

        public:
        static constexpr bool debug_assert() noexcept
        {
            block key{};
            auto aes = AES<4>{ key };
            cipher_group temp{};

            auto k4 = aes.output_rk4();
            auto translate = word{ k4[0],k4[13],k4[10],k4[7] };

            for (auto i : Attack_Interface::iota_byte | std::views::take(63))
            {
                auto sc = aes.encrypt(block{ i });
                temp.push_back({ sc[0],sc[13],sc[10],sc[7] });
            }

            return verify(translate, temp);

        }
        ciphertexts copy;
        static inline EncKey result_key;
	};
	
    void test_atk4_2();
}

template<>
struct std::formatter<atk4_2::Attack::word_rvw>
{
    constexpr static auto hexify = [](::byte x) {return std::format("{:02x}", x.value);};

    constexpr auto parse(std::format_parse_context& fpc)
    {
        return fpc.begin();
    }

    auto format(atk4_2::Attack::word_rvw v, std::format_context& fc) const
    {
        return std::ranges::copy(v | std::views::transform(hexify) | std::views::join, fc.out()).out;
    }
};

template<>
struct std::formatter<atk4_2::Attack::word> : std::formatter<atk4_2::Attack::word_rvw>{};