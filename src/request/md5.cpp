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

#include <utility/common.hpp>

#include "md5.hpp"

namespace hutzn
{

namespace
{

static const size_t block_size = 64;

inline uint32_t rotate_left(const uint32_t& x, const uint8_t bits)
{
    return (x << bits) | (x >> ((sizeof(x) * BITS_PER_BYTE) - bits));
}

inline uint32_t f(const uint32_t& x, const uint32_t& y, const uint32_t& z)
{
    return (x & y) | ((~x) & z);
}

inline uint32_t g(const uint32_t& x, const uint32_t& y, const uint32_t& z)
{
    return (x & z) | (y & (~z));
}

inline uint32_t h(const uint32_t& x, const uint32_t& y, const uint32_t& z)
{
    return x ^ y ^ z;
}

inline uint32_t i(const uint32_t& x, const uint32_t& y, const uint32_t& z)
{
    return y ^ (x | (~z));
}

static const std::array<uint32_t, block_size> t{
    {0xD76AA478U, 0xE8C7B756U, 0x242070DBU, 0xC1BDCEEEU, 0xF57C0FAFU,
     0x4787C62AU, 0xA8304613U, 0xFD469501U, 0x698098D8U, 0x8B44F7AFU,
     0xFFFF5BB1U, 0x895CD7BEU, 0x6B901122U, 0xFD987193U, 0xA679438EU,
     0x49B40821U, 0xF61E2562U, 0xC040B340U, 0x265E5A51U, 0xE9B6C7AAU,
     0xD62F105DU, 0x02441453U, 0xD8A1E681U, 0xE7D3FBC8U, 0x21E1CDE6U,
     0xC33707D6U, 0xF4D50D87U, 0x455A14EDU, 0xA9E3E905U, 0xFCEFA3F8U,
     0x676F02D9U, 0x8D2A4C8AU, 0xFFFA3942U, 0x8771F681U, 0x6D9D6122U,
     0xFDE5380CU, 0xA4BEEA44U, 0x4BDECFA9U, 0xF6BB4B60U, 0xBEBFBC70U,
     0x289B7EC6U, 0xEAA127FAU, 0xD4EF3085U, 0x04881D05U, 0xD9D4D039U,
     0xE6DB99E5U, 0x1FA27CF8U, 0xC4AC5665U, 0xF4292244U, 0x432AFF97U,
     0xAB9423A7U, 0xFC93A039U, 0x655B59C3U, 0x8F0CCC92U, 0xFFEFF47DU,
     0x85845DD1U, 0x6FA87E4FU, 0xFE2CE6E0U, 0xA3014314U, 0x4E0811A1U,
     0xF7537E82U, 0xBD3AF235U, 0x2AD7D2BBU, 0xEB86D391U}};

inline uint32_t set1(const uint32_t& a, const uint32_t& b, const uint32_t& c,
                     const uint32_t& d, const uint32_t* x, const size_t& k,
                     const uint8_t& s, const size_t& j)
{
    return rotate_left(a + f(b, c, d) + x[k] + t[j], s) + b;
}

inline uint32_t set2(const uint32_t& a, const uint32_t& b, const uint32_t& c,
                     const uint32_t& d, const uint32_t* x, const size_t& k,
                     const uint8_t& s, const size_t& j)
{
    return rotate_left(a + g(b, c, d) + x[k] + t[j], s) + b;
}

inline uint32_t set3(const uint32_t& a, const uint32_t& b, const uint32_t& c,
                     const uint32_t& d, const uint32_t* x, const size_t& k,
                     const uint8_t& s, const size_t& j)
{
    return rotate_left(a + h(b, c, d) + x[k] + t[j], s) + b;
}

inline uint32_t set4(const uint32_t& a, const uint32_t& b, const uint32_t& c,
                     const uint32_t& d, const uint32_t* x, const size_t& k,
                     const uint8_t& s, const size_t& j)
{
    return rotate_left(a + i(b, c, d) + x[k] + t[j], s) + b;
}

template <const size_t shift1, const size_t shift2, const size_t shift3,
          const size_t shift4, typename functor>
void step_fn(functor fn, uint32_t& a, uint32_t& b, uint32_t& c, uint32_t& d,
             const uint32_t* const x, const size_t beginning_index,
             std::array<size_t, 16> indices)
{
    static const size_t index0 = 0;
    static const size_t index1 = 1;
    static const size_t index2 = 2;
    static const size_t index3 = 3;

    for (size_t i = 0; i < 16; i += 4) {
        a = fn(a, b, c, d, x, indices[i + 0], shift1,
               beginning_index + i + index0);
        d = fn(d, a, b, c, x, indices[i + 1], shift2,
               beginning_index + i + index1);
        c = fn(c, d, a, b, x, indices[i + 2], shift3,
               beginning_index + i + index2);
        b = fn(b, c, d, a, x, indices[i + 3], shift4,
               beginning_index + i + index3);
    }
}

static std::array<size_t, 16> get_indice_array(const size_t seed,
                                               const size_t inc,
                                               const size_t mod)
{
    std::array<size_t, 16> result;
    size_t index = seed;
    for (size_t i = 0; i < result.size(); i++) {
        result[i] = index;
        index = (index + inc) % mod;
    }
    assert(seed == index);
    return result;
}

//! @brief Processes one block.
//! @param data The data block to process.
//! @param digest The current digest result.
//! @warning Calling this function makes only sense inside of the md5 algorithm.
void process(const char_t data[block_size], std::array<uint32_t, 4>& digest)
{
    uint32_t a = digest[0];
    uint32_t b = digest[1];
    uint32_t c = digest[2];
    uint32_t d = digest[3];
    const uint32_t* x = reinterpret_cast<const uint32_t*>(data);

    static const std::array<size_t, 16> indices1 = get_indice_array(0, 1, 16);
    step_fn<7, 12, 17, 22>(&set1, a, b, c, d, x, 0, indices1);

    static const std::array<size_t, 16> indices2 = get_indice_array(1, 5, 16);
    step_fn<5, 9, 14, 20>(&set2, a, b, c, d, x, 16, indices2);

    static const std::array<size_t, 16> indices3 = get_indice_array(5, 3, 16);
    step_fn<4, 11, 16, 23>(&set3, a, b, c, d, x, 32, indices3);

    static const std::array<size_t, 16> indices4 = get_indice_array(0, 7, 16);
    step_fn<6, 10, 15, 21>(&set4, a, b, c, d, x, 48, indices4);

    // Add this to the digest.
    digest[0] += a;
    digest[1] += b;
    digest[2] += c;
    digest[3] += d;
}

} // namespace

md5_array calculate_md5(const std::vector<char_t>& data)
{
    static const size_t size_of_size = sizeof(uint64_t);
    static const size_t max_size_minus_size = block_size - size_of_size;

    std::array<uint32_t, 4> digest{
        {0x67452301U, 0xEFCDAB89U, 0x98BADCFEU, 0x10325476U}};
    const char_t* pointer = data.data();
    size_t remaining = data.size();

    // Process.
    while (remaining >= block_size) {
        process(pointer, digest);
        pointer += block_size;
        remaining -= block_size;
    }

    // Copy the rest.
    std::array<char_t, block_size> data_buffer;
    uint8_t last_bit = 0x80U;
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
