/* This file is part of libhutznohmd.
 * Copyright (C) 2013-2015 Stefan Weiser

 * The libhutznohmd project is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation; either version 3.0 of the
 * License, or (at your option) any later version.

 * The libhutznohmd project is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser
 * General Public License for more details.

 * You should have received a copy of the GNU Lesser General Public License
 * along with the libhutznohmd project; if not, see
 * <http://www.gnu.org/licenses/>.
 */

#include <cassert>
#include <cmath>

#include <utility/common.hpp>

#include "md5.hpp"

namespace hutzn
{

namespace
{

static const size_t block_size = 64;
static const size_t processed_width_per_step = 16;
static const size_t number_of_steps = block_size / processed_width_per_step;
using digest_type = std::array<uint32_t, number_of_steps>;
using table_k_type = std::array<uint32_t, block_size>;

static const size_t b_idx_offset = 1;
static const size_t c_idx_offset = 2;
static const size_t d_idx_offset = 3;

using set_functor = uint32_t (*)(const uint32_t& a, const uint32_t& b,
                                 const uint32_t& c, const uint32_t& d,
                                 const uint32_t* x, const size_t& k,
                                 const size_t& j, const uint8_t& s);

struct dword_setting
{
    std::array<size_t, processed_width_per_step> indices;
    std::array<uint8_t, number_of_steps> shifts;
    set_functor set_fn;
};

static std::array<size_t, processed_width_per_step> get_indice_array(
    const size_t seed, const size_t inc, const size_t mod)
{
    std::array<size_t, processed_width_per_step> result;
    size_t index = seed;
    for (size_t i = 0; i < result.size(); i++) {
        result[i] = index;
        index = (index + inc) % mod;
    }
    assert(seed == index);
    return result;
}

inline const table_k_type make_table_k()
{
    static const uint64_t power =
        static_cast<uint64_t>(1)
        << (BITS_PER_BYTE * sizeof(table_k_type::value_type));

    table_k_type result;
    for (size_t i = 0; i < block_size; i++) {
        result[i] = static_cast<uint32_t>(
            std::floor(std::abs(std::sin(i + 1)) * power));
    }
    return result;
}

inline uint32_t get_value_of_k(const size_t i)
{
    static const table_k_type table_k = make_table_k();
    return table_k[i];
}

inline uint32_t rotate_left(const uint32_t& x, const uint8_t bits)
{
    return (x << bits) | (x >> ((sizeof(x) * BITS_PER_BYTE) - bits));
}

inline uint32_t set1(const uint32_t& a, const uint32_t& b, const uint32_t& c,
                     const uint32_t& d, const uint32_t* x, const size_t& k,
                     const size_t& j, const uint8_t& s)
{
    const uint32_t f = (b & c) | ((~b) & d);
    return rotate_left(a + f + x[k] + get_value_of_k(j), s) + b;
}

inline uint32_t set2(const uint32_t& a, const uint32_t& b, const uint32_t& c,
                     const uint32_t& d, const uint32_t* x, const size_t& k,
                     const size_t& j, const uint8_t& s)
{
    const uint32_t g = (b & d) | (c & (~d));
    return rotate_left(a + g + x[k] + get_value_of_k(j), s) + b;
}

inline uint32_t set3(const uint32_t& a, const uint32_t& b, const uint32_t& c,
                     const uint32_t& d, const uint32_t* x, const size_t& k,
                     const size_t& j, const uint8_t& s)
{
    const uint32_t h = b ^ c ^ d;
    return rotate_left(a + h + x[k] + get_value_of_k(j), s) + b;
}

inline uint32_t set4(const uint32_t& a, const uint32_t& b, const uint32_t& c,
                     const uint32_t& d, const uint32_t* x, const size_t& k,
                     const size_t& j, const uint8_t& s)
{
    const uint32_t i = c ^ (b | (~d));
    return rotate_left(a + i + x[k] + get_value_of_k(j), s) + b;
}

static void step_fn(digest_type& digest, const uint32_t* const x,
                    const size_t beginning_index, const dword_setting& settings)
{
    for (size_t i = 0; i < processed_width_per_step; i += number_of_steps) {
        for (size_t j = 0; j < number_of_steps; j++) {
            const size_t idx = (number_of_steps - j) % number_of_steps;
            uint32_t& a = digest[idx];
            uint32_t& b = digest[(idx + b_idx_offset) % number_of_steps];
            uint32_t& c = digest[(idx + c_idx_offset) % number_of_steps];
            uint32_t& d = digest[(idx + d_idx_offset) % number_of_steps];

            a = settings.set_fn(a, b, c, d, x, settings.indices[i + j],
                                beginning_index + i + j, settings.shifts[j]);
        }
    }
}

//! @brief Processes one block.
//! @param data The data block to process.
//! @param digest The current digest result.
//! @warning Calling this function makes only sense inside of the md5 algorithm.
static void process(const char_t data[block_size], digest_type& digest)
{
    static const std::array<dword_setting, number_of_steps> settings = {
        {dword_setting{get_indice_array(0, 1, processed_width_per_step),
                       {{7, 12, 17, 22}},
                       &set1},
         dword_setting{get_indice_array(1, 5, processed_width_per_step),
                       {{5, 9, 14, 20}},
                       &set2},
         dword_setting{get_indice_array(5, 3, processed_width_per_step),
                       {{4, 11, 16, 23}},
                       &set3},
         dword_setting{get_indice_array(0, 7, processed_width_per_step),
                       {{6, 10, 15, 21}},
                       &set4}}};

    digest_type copy = digest;
    const uint32_t* x = reinterpret_cast<const uint32_t*>(data);

    for (size_t i = 0; i < settings.size(); i++) {
        step_fn(copy, x, processed_width_per_step * i, settings[i]);
    }

    // Add this to the digest.
    digest[0] += copy[0];
    digest[b_idx_offset] += copy[b_idx_offset];
    digest[c_idx_offset] += copy[c_idx_offset];
    digest[d_idx_offset] += copy[d_idx_offset];
}

} // namespace

md5_array calculate_md5(const std::vector<char_t>& data)
{
    static const size_t size_of_size = sizeof(uint64_t);
    static const size_t max_size_minus_size = block_size - size_of_size;

    digest_type digest{{0x67452301U, 0xEFCDAB89U, 0x98BADCFEU, 0x10325476U}};
    const char_t* pointer = data.data();
    size_t remaining = data.size();

    // Process.
    while (remaining >= block_size) {
        process(pointer, digest);
        pointer += block_size;
        remaining -= block_size;
    }

    // Copy the rest.
    static const uint8_t invalid_last_bit = 0x80U;
    std::array<char_t, block_size> data_buffer;
    uint8_t last_bit = invalid_last_bit;
    std::copy(data.end() - static_cast<ssize_t>(remaining), data.end(),
              data_buffer.begin());

    // If there is not enough space to append the size, we have to fill it into
    // another block.
    if (remaining > (max_size_minus_size - 1)) {
        std::fill(data_buffer.begin() + remaining, data_buffer.end(), 0);
        data_buffer[remaining] = static_cast<char_t>(last_bit);
        process(data_buffer.data(), digest);
        last_bit = 0;
        remaining = 0;
    }

    // Fill up the block till there are 8 bytes left for the size in bits.
    std::fill(data_buffer.begin() + remaining,
              data_buffer.begin() + max_size_minus_size, 0);
    data_buffer[remaining] = static_cast<char_t>(last_bit);

    // Fill up the number of bits.
    const uint64_t processed_bits = data.size() * BITS_PER_BYTE;
    for (size_t i = 0; i < size_of_size; ++i) {
        const size_t index = max_size_minus_size + i;
        data_buffer[index] =
            static_cast<char_t>(processed_bits >> (i * BITS_PER_BYTE));
    }

    // Process last block.
    process(data_buffer.data(), digest);

    // Return result.
    md5_array result;
    for (size_t i = 0; i < result.size(); ++i) {
        const size_t index = i / digest.size();
        const size_t bits_to_shift = (i % digest.size()) * BITS_PER_BYTE;
        result[i] = static_cast<uint8_t>(digest[index] >> bits_to_shift);
    }
    return std::move(result);
}

} // namespace hutzn
