/* This file is part of librestsrv.
 * Copyright (C) 2013-2014 Stefan Weiser

 * The librestsrv project is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 3.0 of the License, or (at your
 * option) any later version.

 * The librestsrv project is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public
 * License for more details.

 * You should have received a copy of the GNU Lesser General Public License
 * along with the librestsrv project; if not, see <http://www.gnu.org/licenses/>.
 */

#include "md5.hpp"

namespace rest
{

namespace http
{

static constexpr size_t block_size = 64;

inline uint32_t rotate_left(const uint32_t & x, const uint8_t bits)
{
    return (x << bits) | (x >> (32 - bits));
}

inline uint32_t f(const uint32_t & x,
                  const uint32_t & y,
                  const uint32_t & z)
{
    return (x & y) | ((~x) & z);
}

inline uint32_t g(const uint32_t & x,
                  const uint32_t & y,
                  const uint32_t & z)
{
    return (x & z) | (y & (~z));
}
inline uint32_t h(const uint32_t & x,
                  const uint32_t & y,
                  const uint32_t & z)
{
    return x ^ y ^ z;
}

inline uint32_t i(const uint32_t & x,
                  const uint32_t & y,
                  const uint32_t & z)
{
    return y ^ (x | (~z));
}

static const std::array<uint32_t, block_size> t {{
        0xD76AA478, 0xE8C7B756, 0x242070DB, 0xC1BDCEEE, 0xF57C0FAF, 0x4787C62A, 0xA8304613,
        0xFD469501, 0x698098D8, 0x8B44F7AF, 0xFFFF5BB1, 0x895CD7BE, 0x6B901122, 0xFD987193,
        0xA679438E, 0x49B40821, 0xF61E2562, 0xC040B340, 0x265E5A51, 0xE9B6C7AA, 0xD62F105D,
        0x02441453, 0xD8A1E681, 0xE7D3FBC8, 0x21E1CDE6, 0xC33707D6, 0xF4D50D87, 0x455A14ED,
        0xA9E3E905, 0xFCEFA3F8, 0x676F02D9, 0x8D2A4C8A, 0xFFFA3942, 0x8771F681, 0x6D9D6122,
        0xFDE5380C, 0xA4BEEA44, 0x4BDECFA9, 0xF6BB4B60, 0xBEBFBC70, 0x289B7EC6, 0xEAA127FA,
        0xD4EF3085, 0x04881D05, 0xD9D4D039, 0xE6DB99E5, 0x1FA27CF8, 0xC4AC5665, 0xF4292244,
        0x432AFF97, 0xAB9423A7, 0xFC93A039, 0x655B59C3, 0x8F0CCC92, 0xFFEFF47D, 0x85845DD1,
        0x6FA87E4F, 0xFE2CE6E0, 0xA3014314, 0x4E0811A1, 0xF7537E82, 0xBD3AF235, 0x2AD7D2BB,
        0xEB86D391
    }
};

inline uint32_t set1(const uint32_t & a, const uint32_t & b, const uint32_t & c,
                     const uint32_t & d, const uint32_t * x, const size_t & k,
                     const uint8_t & s, const size_t & j)
{
    return rotate_left(a + f(b, c, d) + x[k] + t[j], s) + b;
}

inline uint32_t set2(const uint32_t & a, const uint32_t & b, const uint32_t & c,
                     const uint32_t & d, const uint32_t * x, const size_t & k,
                     const uint8_t & s, const size_t & j)
{
    return rotate_left(a + g(b, c, d) + x[k] + t[j], s) + b;
}

inline uint32_t set3(const uint32_t & a, const uint32_t & b, const uint32_t & c,
                     const uint32_t & d, const uint32_t * x, const size_t & k,
                     const uint8_t & s, const size_t & j)
{
    return rotate_left(a + h(b, c, d) + x[k] + t[j], s) + b;
}

inline uint32_t set4(const uint32_t & a, const uint32_t & b, const uint32_t & c,
                     const uint32_t & d, const uint32_t * x, const size_t & k,
                     const uint8_t & s, const size_t & j)
{
    return rotate_left(a + i(b, c, d) + x[k] + t[j], s) + b;
}

void process(const uint8_t data[block_size], std::array<uint32_t, 4> & digest)
{
    uint32_t a = digest[0];
    uint32_t b = digest[1];
    uint32_t c = digest[2];
    uint32_t d = digest[3];
    const uint32_t * x = reinterpret_cast<const uint32_t *>(data);

    a = set1(a, b, c, d, x,  0,  7, 0);
    d = set1(d, a, b, c, x,  1, 12, 1);
    c = set1(c, d, a, b, x,  2, 17, 2);
    b = set1(b, c, d, a, x,  3, 22, 3);
    a = set1(a, b, c, d, x,  4,  7, 4);
    d = set1(d, a, b, c, x,  5, 12, 5);
    c = set1(c, d, a, b, x,  6, 17, 6);
    b = set1(b, c, d, a, x,  7, 22, 7);
    a = set1(a, b, c, d, x,  8,  7, 8);
    d = set1(d, a, b, c, x,  9, 12, 9);
    c = set1(c, d, a, b, x, 10, 17, 10);
    b = set1(b, c, d, a, x, 11, 22, 11);
    a = set1(a, b, c, d, x, 12,  7, 12);
    d = set1(d, a, b, c, x, 13, 12, 13);
    c = set1(c, d, a, b, x, 14, 17, 14);
    b = set1(b, c, d, a, x, 15, 22, 15);
    a = set2(a, b, c, d, x,  1,  5, 16);
    d = set2(d, a, b, c, x,  6,  9, 17);
    c = set2(c, d, a, b, x, 11, 14, 18);
    b = set2(b, c, d, a, x,  0, 20, 19);
    a = set2(a, b, c, d, x,  5,  5, 20);
    d = set2(d, a, b, c, x, 10,  9, 21);
    c = set2(c, d, a, b, x, 15, 14, 22);
    b = set2(b, c, d, a, x,  4, 20, 23);
    a = set2(a, b, c, d, x,  9,  5, 24);
    d = set2(d, a, b, c, x, 14,  9, 25);
    c = set2(c, d, a, b, x,  3, 14, 26);
    b = set2(b, c, d, a, x,  8, 20, 27);
    a = set2(a, b, c, d, x, 13,  5, 28);
    d = set2(d, a, b, c, x,  2,  9, 29);
    c = set2(c, d, a, b, x,  7, 14, 30);
    b = set2(b, c, d, a, x, 12, 20, 31);
    a = set3(a, b, c, d, x,  5,  4, 32);
    d = set3(d, a, b, c, x,  8, 11, 33);
    c = set3(c, d, a, b, x, 11, 16, 34);
    b = set3(b, c, d, a, x, 14, 23, 35);
    a = set3(a, b, c, d, x,  1,  4, 36);
    d = set3(d, a, b, c, x,  4, 11, 37);
    c = set3(c, d, a, b, x,  7, 16, 38);
    b = set3(b, c, d, a, x, 10, 23, 39);
    a = set3(a, b, c, d, x, 13,  4, 40);
    d = set3(d, a, b, c, x,  0, 11, 41);
    c = set3(c, d, a, b, x,  3, 16, 42);
    b = set3(b, c, d, a, x,  6, 23, 43);
    a = set3(a, b, c, d, x,  9,  4, 44);
    d = set3(d, a, b, c, x, 12, 11, 45);
    c = set3(c, d, a, b, x, 15, 16, 46);
    b = set3(b, c, d, a, x,  2, 23, 47);
    a = set4(a, b, c, d, x,  0,  6, 48);
    d = set4(d, a, b, c, x,  7, 10, 49);
    c = set4(c, d, a, b, x, 14, 15, 50);
    b = set4(b, c, d, a, x,  5, 21, 51);
    a = set4(a, b, c, d, x, 12,  6, 52);
    d = set4(d, a, b, c, x,  3, 10, 53);
    c = set4(c, d, a, b, x, 10, 15, 54);
    b = set4(b, c, d, a, x,  1, 21, 55);
    a = set4(a, b, c, d, x,  8,  6, 56);
    d = set4(d, a, b, c, x, 15, 10, 57);
    c = set4(c, d, a, b, x,  6, 15, 58);
    b = set4(b, c, d, a, x, 13, 21, 59);
    a = set4(a, b, c, d, x,  4,  6, 60);
    d = set4(d, a, b, c, x, 11, 10, 61);
    c = set4(c, d, a, b, x,  2, 15, 62);
    b = set4(b, c, d, a, x,  9, 21, 63);

    // Add this to the digest.
    digest[0] += a;
    digest[1] += b;
    digest[2] += c;
    digest[3] += d;
}

std::array<uint8_t, 16> calculate_md5(const std::vector<uint8_t> & data)
{
    static constexpr size_t bits_per_byte = 8;
    static constexpr size_t size_of_size = sizeof(uint64_t);
    static constexpr size_t max_size_minus_size = block_size - size_of_size;

    std::array<uint32_t, 4> digest {{0x67452301, 0xEFCDAB89, 0x98BADCFE, 0x10325476}};
    const uint8_t * pointer = data.data();
    size_t remaining = data.size();

    // Process.
    while (remaining >= block_size) {
        process(pointer, digest);
        pointer += block_size;
        remaining -= block_size;
    }

    // Copy the rest.
    std::array<uint8_t, block_size> buffer;
    uint8_t last_bit = 0x80;
    std::copy(data.end() - remaining, data.end(), buffer.begin());

    // If there is not enough space to append the size, we have to fill it into another block.
    if (remaining > (max_size_minus_size - 1)) {
        std::fill(buffer.begin() + remaining, buffer.end(), 0);
        buffer[remaining] = last_bit;
        process(buffer.data(), digest);
        last_bit = 0;
        remaining = 0;
    }

    // Fill up the block till there are 8 bytes left for the size in bits.
    std::fill(buffer.begin() + remaining, buffer.begin() + max_size_minus_size, 0);
    buffer[remaining] = last_bit;

    // Fill up the number of bits.
    const size_t processed_bits = data.size() * bits_per_byte;
    for (size_t i = 0; i < size_of_size; ++i) {
        const size_t index = max_size_minus_size + i;
        buffer[index] = static_cast<uint8_t>(processed_bits >>(i * bits_per_byte));
    }

    // Process last block.
    process(buffer.data(), digest);

    // Return result.
    std::array<uint8_t, 16> result;
    for (size_t i = 0; i < result.size(); ++i) {
        const size_t index = i / digest.size();
        const size_t bits_to_shift = (i % digest.size()) * bits_per_byte;
        result[i] = static_cast<uint8_t>(digest[index] >> bits_to_shift);
    }
    return std::move(result);
}

} // namespace http

} // namespace rest
