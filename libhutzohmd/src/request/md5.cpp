/* This file is part of libhutznohmd.
 * Copyright (C) 2013-2025 Stefan Weiser

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

#include "md5.hpp"

#include <cassert>
#include <cmath>

#include "utility/common.hpp"

namespace hutzn
{

namespace
{

static const size_t block_size = 64;
static const size_t processed_width_per_step = 16;
static const size_t number_of_steps = block_size / processed_width_per_step;
static const size_t a_idx_offset = 0;
static const size_t b_idx_offset = 1;
static const size_t c_idx_offset = 2;
static const size_t d_idx_offset = 3;

//! Stores digest type, which is an intermediate result.
using digest_type = std::array<uint32_t, number_of_steps>;

//! Stores a table k.
using table_k_type = std::array<uint32_t, block_size>;

//! A function pointer to f, g, h or i.
using set_functor = uint32_t (*)(const uint32_t& a, const uint32_t& b,
                                 const uint32_t& c, const uint32_t& d,
                                 const uint32_t* x, const size_t& k,
                                 const size_t& j, const uint8_t& s);

//! @brief Defines a setting for a dword.
struct dword_setting {
    //! A number of indices, which are used to select an input data index for
    //! a calculation.
    std::array<size_t, processed_width_per_step> indices;

    //! Contains bit shift numbers, which are used by each step when rotating
    //! the result of f, g, h and i.
    std::array<uint8_t, number_of_steps> shifts;

    //! Stores a function to f, g, h or i.
    set_functor set_fn;
};

//! @brief Calculates an array of indices for use in dword_settings.
//!
//! These index arrays define a progression in a residue class ring.
//! @param[in] seed Used initial index.
//! @param[in] inc  Incrementation step.
//! @param[in] mod  Modulus of incrementation.
//! @return Array of indices.
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

//! @brief Calculates a table k.
//!
//! @return Table k.
inline const table_k_type make_table_k()
{
    static const uint64_t power =
        static_cast<uint64_t>(1)
        << (bits_per_byte * sizeof(table_k_type::value_type));

    table_k_type result;
    for (size_t i = 0; i < block_size; i++) {
        result[i] = static_cast<uint32_t>(
            std::floor(std::abs(std::sin(i + 1)) * power));
    }
    return result;
}

//! @brief Returns a specific value of table k.
//!
//! The table k is lazy initialized, which means, that the first call will
//! initialize it.
//! @param[in] i Index to the value of table k.
//! @return      Value of table k at index i.
inline uint32_t get_value_of_k(const size_t i)
{
    static const table_k_type table_k = make_table_k();
    return table_k[i];
}

//! @brief Rotates a value some bits left.
//!
//! The shifted out bits get inserted on the right.
//! @param[in] x    Value to rotate.
//! @param[in] bits Number of bits to shift.
//! @return         Rotated value.
inline uint32_t rotate_left(const uint32_t& x, const uint8_t bits)
{
    return (x << bits) | (x >> ((sizeof(x) * bits_per_byte) - bits));
}

//! @brief Conforms to function f of md5 algorithm.
//!
//! @param[in] a Value a of function f.
//! @param[in] b Value b of function f.
//! @param[in] c Value c of function f.
//! @param[in] d Value d of function f.
//! @param[in] x Input data.
//! @param[in] k Input data index.
//! @param[in] j Index of table k.
//! @param[in] s Number of bits to rotate left.
//! @return      Result of function f.
inline uint32_t set1(const uint32_t& a, const uint32_t& b, const uint32_t& c,
                     const uint32_t& d, const uint32_t* x, const size_t& k,
                     const size_t& j, const uint8_t& s)
{
    const uint32_t f = (b & c) | ((~b) & d);
    return rotate_left(a + f + x[k] + get_value_of_k(j), s) + b;
}

//! @brief Conforms to function g of md5 algorithm.
//!
//! @param[in] a Value a of function g.
//! @param[in] b Value b of function g.
//! @param[in] c Value c of function g.
//! @param[in] d Value d of function g.
//! @param[in] x Input data.
//! @param[in] k Input data index.
//! @param[in] j Index of table k.
//! @param[in] s Number of bits to rotate left.
//! @return      Result of function g.
inline uint32_t set2(const uint32_t& a, const uint32_t& b, const uint32_t& c,
                     const uint32_t& d, const uint32_t* x, const size_t& k,
                     const size_t& j, const uint8_t& s)
{
    const uint32_t g = (b & d) | (c & (~d));
    return rotate_left(a + g + x[k] + get_value_of_k(j), s) + b;
}

//! @brief Conforms to function h of md5 algorithm.
//!
//! @param[in] a Value a of function h.
//! @param[in] b Value b of function h.
//! @param[in] c Value c of function h.
//! @param[in] d Value d of function h.
//! @param[in] x Input data.
//! @param[in] k Input data index.
//! @param[in] j Index of table k.
//! @param[in] s Number of bits to rotate left.
//! @return      Result of function h.
inline uint32_t set3(const uint32_t& a, const uint32_t& b, const uint32_t& c,
                     const uint32_t& d, const uint32_t* x, const size_t& k,
                     const size_t& j, const uint8_t& s)
{
    const uint32_t h = b ^ c ^ d;
    return rotate_left(a + h + x[k] + get_value_of_k(j), s) + b;
}

//! @brief Conforms to function i of md5 algorithm.
//!
//! @param[in] a Value a of function i.
//! @param[in] b Value b of function i.
//! @param[in] c Value c of function i.
//! @param[in] d Value d of function i.
//! @param[in] x Input data.
//! @param[in] k Input data index.
//! @param[in] j Index of table k.
//! @param[in] s Number of bits to rotate left.
//! @return      Result of function i.
inline uint32_t set4(const uint32_t& a, const uint32_t& b, const uint32_t& c,
                     const uint32_t& d, const uint32_t* x, const size_t& k,
                     const size_t& j, const uint8_t& s)
{
    const uint32_t i = c ^ (b | (~d));
    return rotate_left(a + i + x[k] + get_value_of_k(j), s) + b;
}

//! @brief Processes one step.
//!
//! @param[out] digest          The current digest result.
//! @param[in]  x               Input data block.
//! @param[in]  beginning_index Start index of this step.
//! @param[in]  settings        Settings to use for the step.
void step_fn(digest_type& digest, const uint32_t* const x,
             const size_t beginning_index, const dword_setting& settings)
{
    for (size_t i = 0; i < processed_width_per_step; i += number_of_steps) {
        for (size_t j = 0; j < number_of_steps; j++) {
            const size_t idx = (number_of_steps - j) % number_of_steps;
            uint32_t& a = digest[(idx + a_idx_offset) % number_of_steps];
            uint32_t& b = digest[(idx + b_idx_offset) % number_of_steps];
            uint32_t& c = digest[(idx + c_idx_offset) % number_of_steps];
            uint32_t& d = digest[(idx + d_idx_offset) % number_of_steps];

            a = settings.set_fn(a, b, c, d, x, settings.indices[i + j],
                                beginning_index + i + j, settings.shifts[j]);
        }
    }
}

//! @brief Processes one block.
//!
//! @param[in,out] data   The data block to process.
//! @param[out]    digest The current digest result.
void process(const char_t* data, digest_type& digest)
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

    // one process round consists of for steps, that are described in data
    // above.
    for (size_t i = 0; i < settings.size(); i++) {
        step_fn(copy, x, processed_width_per_step * i, settings[i]);
    }

    // add this to the digest
    digest[a_idx_offset] += copy[a_idx_offset];
    digest[b_idx_offset] += copy[b_idx_offset];
    digest[c_idx_offset] += copy[c_idx_offset];
    digest[d_idx_offset] += copy[d_idx_offset];
}

//! @brief Returns, whether the platform the code runs on is little endian.
//!
//! @return True, when the platform the code runs on is little endian and false
//!         if not.
bool is_little_endian()
{
    // endianess could be checked, by proving what gets written into the first
    // byte of an 16 bit variable
    static const uint16_t endianness_test_value = 1;
    return (*reinterpret_cast<const uint8_t*>(&endianness_test_value)) == 1;
}

} // namespace

md5_array calculate_md5(const char_t* const data, const size_t size)
{
    assert(is_little_endian());

    static const size_t size_of_size = sizeof(uint64_t);
    static const size_t max_size_minus_size = block_size - size_of_size;

    digest_type digest{{0x67452301U, 0xEFCDAB89U, 0x98BADCFEU, 0x10325476U}};
    const char_t* pointer = data;
    size_t remaining = size;

    // process
    while (remaining >= block_size) {
        process(pointer, digest);
        pointer += block_size;
        remaining -= block_size;
    }

    // copy the rest
    static const uint8_t invalid_last_bit = 0x80U;
    std::array<char_t, block_size> data_buffer;
    uint8_t last_bit = invalid_last_bit;
    std::copy(data + size - static_cast<ssize_t>(remaining), data + size,
              data_buffer.begin());

    // if there is not enough space to append the size, we have to fill it into
    // another block
    if (remaining > (max_size_minus_size - 1)) {
        std::fill(data_buffer.begin() + remaining, data_buffer.end(), 0);
        data_buffer[remaining] = static_cast<char_t>(last_bit);
        process(data_buffer.data(), digest);
        last_bit = 0;
        remaining = 0;
    }

    // fill up the block till there are 8 bytes left for the size in bits
    std::fill(data_buffer.begin() + remaining,
              data_buffer.begin() + max_size_minus_size, 0);
    data_buffer[remaining] = static_cast<char_t>(last_bit);

    // fill up the number of bits
    const uint64_t processed_bits = size * bits_per_byte;
    for (size_t i = 0; i < size_of_size; ++i) {
        const size_t index = max_size_minus_size + i;
        data_buffer[index] =
            static_cast<char_t>(processed_bits >> (i * bits_per_byte));
    }

    // process last block
    process(data_buffer.data(), digest);

    // return result
    md5_array result;
    for (size_t i = 0; i < result.size(); ++i) {
        const size_t index = i / digest.size();
        const size_t bits_to_shift = (i % digest.size()) * bits_per_byte;
        result[i] = static_cast<uint8_t>(digest[index] >> bits_to_shift);
    }
    return result;
}

} // namespace hutzn
