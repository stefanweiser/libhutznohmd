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

#include <array>

#include "base64.hpp"

namespace rest
{

namespace http
{

static const std::string base64_encoder_map = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz"
        "0123456789+/";

static const std::array<int8_t, 256> base64_decoder_map = {{
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 62, -1, -1, -1, 63,
        52, 53, 54, 55, 56, 57, 58, 59, 60, 61, -1, -1, -1, -1, -1, -1,
        -1, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14,
        15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, -1, -1, -1, -1, -1,
        -1, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
        41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, -1, -1, -1, -1, -1,
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
        -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1
    }
};

static inline bool is_base64(uint8_t c)
{
    return (std::isalnum(c) || (c == '+') || (c == '/'));
}

std::string encode_base64(const std::vector<uint8_t> & data)
{
    std::string result;

    const uint8_t * p = data.data();
    size_t remaining = data.size();
    uint32_t bit_backlog = 0;
    uint8_t bytes_in_backlog = 0;
    while (remaining > 0) {
        bit_backlog = (bit_backlog << 8) | (*p);
        bytes_in_backlog++;
        p++;
        remaining--;

        if (3 == bytes_in_backlog) {
            for (size_t i = 0; i < 4; i++) {
                const size_t bits_to_shift = 6 * (3 - i);
                const uint8_t token = (bit_backlog >> bits_to_shift) & 0x3F;
                result.push_back(base64_encoder_map[token]);
            }
            bytes_in_backlog = 0;
        }
    }

    if (bytes_in_backlog != 0) {
        const uint8_t original_bytes_in_backlog = bytes_in_backlog;
        while (bytes_in_backlog != 3) {
            bit_backlog = bit_backlog << 8;
            bytes_in_backlog++;
        }

        result.push_back(base64_encoder_map[(bit_backlog >> 18) & 0x3F]);
        result.push_back(base64_encoder_map[(bit_backlog >> 12) & 0x3F]);
        if (2 == original_bytes_in_backlog) {
            result.push_back(base64_encoder_map[(bit_backlog >> 6) & 0x3F]);
        } else {
            result.push_back('=');
        }
        result.push_back('=');
    }

    return result;
}

std::vector<uint8_t> decode_base64(const std::string & encoded_string)
{
    std::vector<uint8_t> result;

    uint32_t bit_backlog = 0;
    uint8_t sextets_in_backlog = 0;
    for (const uint8_t & b : encoded_string) {
        const int8_t token = base64_decoder_map[b];
        if (token >= 0) {
            bit_backlog = (bit_backlog << 6) | token;
            sextets_in_backlog++;
        }
        // Invalid tokens will be discarded.

        if (4 == sextets_in_backlog) {
            result.push_back((bit_backlog >> 16) & 0xFF);
            result.push_back((bit_backlog >> 8) & 0xFF);
            result.push_back(bit_backlog & 0xFF);
            sextets_in_backlog = 0;
        }
    }

    if (sextets_in_backlog != 0) {
        if (sextets_in_backlog == 1) {
            return std::vector<uint8_t>();
        }

        const uint8_t original_sextets_in_backlog = sextets_in_backlog;
        while (sextets_in_backlog < 4) {
            bit_backlog = (bit_backlog << 6) | 0;
            sextets_in_backlog++;
        }

        result.push_back((bit_backlog >> 16) & 0xFF);
        if (original_sextets_in_backlog == 3) {
            result.push_back((bit_backlog >> 8) & 0xFF);
        }
    }

    return result;
}

} // namespace http

} // namespace rest
