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

#include "base64.hpp"

#include <array>

#include "utility/common.hpp"

namespace hutzn
{

namespace
{

static const size_t evaluation_chunk_size = 3;
static const size_t sextets_per_evaluation_chunk = 4;
static const size_t sextets_size = 6;
static const size_t hexade_bitmask = 0x3FU;

static const std::string base64_encoder_map =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "abcdefghijklmnopqrstuvwxyz"
    "0123456789+/";

//! @brief Completes a base64 string.
//!
//! Whenever the number of bytes is not divisible by 3, the remaining bytes get
//! filled with padding.
//! @param[out] result           Bytes get appended to that string.
//! @param[in]  bytes_in_backlog Bytes stored in a bit backlog.
//! @param[in]  bit_backlog      Backlog of read bits to get converted.
void append_base64_last_bytes(std::string& result, uint8_t bytes_in_backlog,
                              uint32_t bit_backlog)
{
    // finish string only when there is data to convert
    if (bytes_in_backlog != 0) {

        // store original number of bytes in backlog
        const uint8_t original_bytes_in_backlog = bytes_in_backlog;

        // fill the backlog with 0 till enough data is in it
        while (bytes_in_backlog != evaluation_chunk_size) {
            bit_backlog = bit_backlog << bits_per_byte;
            bytes_in_backlog++;
        }

        static const size_t second_hexade = sextets_size;
        static const size_t third_hexade = second_hexade + sextets_size;
        static const size_t fourth_hexade = third_hexade + sextets_size;

        // append all original hexades
        result.push_back(base64_encoder_map[(bit_backlog >> fourth_hexade) &
                                            hexade_bitmask]);
        result.push_back(
            base64_encoder_map[(bit_backlog >> third_hexade) & hexade_bitmask]);
        if ((evaluation_chunk_size - 1) == original_bytes_in_backlog) {
            result.push_back(base64_encoder_map[(bit_backlog >> second_hexade) &
                                                hexade_bitmask]);
        } else {
            // when there was no hexade, fill with padding
            result.push_back('=');
        }

        // at least one byte was empty, so fill at least one padding byte
        result.push_back('=');
    }
}

} // namespace

std::string encode_base64(const std::vector<uint8_t>& data)
{
    std::string result;
    const uint8_t* p = data.data();
    size_t remaining = data.size();
    uint32_t bit_backlog = 0;
    uint8_t bytes_in_backlog = 0;

    // loop till all bytes were read
    while (remaining > 0) {

        // put these bytes into a backlog
        bit_backlog = (bit_backlog << bits_per_byte) | (*p);
        bytes_in_backlog++;
        p++;
        remaining--;

        // everytime the limit is reached, encoding of 4 hexades has to be done
        if (evaluation_chunk_size == bytes_in_backlog) {
            for (ssize_t bits_to_shift =
                     ((sextets_per_evaluation_chunk - 1) * sextets_size);
                 bits_to_shift >= 0; bits_to_shift -= sextets_size) {
                const uint8_t token =
                    (bit_backlog >> bits_to_shift) & hexade_bitmask;
                result.push_back(base64_encoder_map[token]);
            }
            bytes_in_backlog = 0;
        }
    }

    // append the remaining rest
    append_base64_last_bytes(result, bytes_in_backlog, bit_backlog);

    return result;
}

std::vector<uint8_t> decode_base64(const std::string& encoded_string)
{
    static const std::array<int8_t, 1 << bits_per_byte> base64_decoder_map = {
        {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
         -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
         -1, -1, -1, -1, -1, -1, -1, 62, -1, 62, -1, 63, 52, 53, 54, 55, 56, 57,
         58, 59, 60, 61, -1, -1, -1, -1, -1, -1, -1, 0, 1, 2, 3, 4, 5, 6, 7, 8,
         9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, -1,
         -1, -1, -1, 63, -1, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38,
         39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, -1, -1, -1, -1, -1,
         -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
         -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
         -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
         -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
         -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
         -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
         -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
         -1, -1}};

    std::vector<uint8_t> result;

    static const size_t second_byte_offset = bits_per_byte;
    static const size_t third_byte_offset = second_byte_offset + bits_per_byte;
    static const size_t byte_bitmask = 0xFFU;

    uint32_t bit_backlog = 0;
    uint8_t sextets_in_backlog = 0;

    // loop over every byte in the base64 string
    for (const int8_t& b : encoded_string) {
        const uint8_t& ub = static_cast<const uint8_t&>(b);

        // every byte has a token represensation
        const int8_t token = base64_decoder_map[ub];

        // invalid tokens will be discarded
        if (token >= 0) {

            // add that 6 bit token to the bit backlog
            const uint8_t utoken = static_cast<const uint8_t>(token);
            bit_backlog = (bit_backlog << sextets_size) | utoken;
            sextets_in_backlog++;
        }

        // whenever the number of sextets per chunk is reached the conversion
        // starts
        if (sextets_per_evaluation_chunk == sextets_in_backlog) {
            result.push_back((bit_backlog >> third_byte_offset) & byte_bitmask);
            result.push_back((bit_backlog >> second_byte_offset) &
                             byte_bitmask);
            result.push_back(bit_backlog & byte_bitmask);
            sextets_in_backlog = 0;
        }
    }

    // there may be remaining sextets in the bit backlog if the string was not
    // correctly terminated by padding
    if (sextets_in_backlog != 0) {

        // it is not possible to convert only 6 bits into at least one byte
        if (sextets_in_backlog == 1) {

            // this is an error
            return std::vector<uint8_t>();
        }

        // fill up with zero bits as padding
        const uint8_t original_sextets_in_backlog = sextets_in_backlog;
        while (sextets_in_backlog < sextets_per_evaluation_chunk) {
            bit_backlog = (bit_backlog << sextets_size) | 0;
            sextets_in_backlog++;
        }

        // convert the chunk
        result.push_back((bit_backlog >> third_byte_offset) & byte_bitmask);
        if (original_sextets_in_backlog == evaluation_chunk_size) {
            result.push_back((bit_backlog >> second_byte_offset) &
                             byte_bitmask);
        }
    }

    return result;
}

} // namespace hutzn
