/* This file is part of librest.
 * Copyright (C) 2013-2014 Stefan Weiser

 * The librest project is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 3.0 of the License, or (at your
 * option) any later version.

 * The librest project is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public
 * License for more details.

 * You should have received a copy of the GNU Lesser General Public License
 * along with the librest project; if not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __LIBREST_HTTP_PARSER_HEADER_HTTPMEDIATYPEPARSER_H__
#define __LIBREST_HTTP_PARSER_HEADER_HTTPMEDIATYPEPARSER_H__

#ifdef __cplusplus
#include <ctime>
#include <map>
#include <string>

extern "C"
{
#endif

typedef struct httpmediatypescan httpmediatypescan_t;

int httpmediatypelex(int * httpmediatypelval, httpmediatypescan_t * scanner);
void httpmediatypeerror(httpmediatypescan_t * scanner, const char * string);

#ifdef __cplusplus
}

namespace rest
{

namespace http
{

class http_media_type_parser;

} // namespace http

} // namespace rest

typedef struct httpmediatypescan {
    rest::http::http_media_type_parser * parser;
} httpmediatypescan_t;

namespace rest
{

namespace http
{

class http_media_type_parser
{
public:
    explicit http_media_type_parser(const std::string & buffer);

    int get();
    void set_error();

    bool valid() const;

private:
    const std::string & buffer_;
    size_t index_;
    bool error_;

    httpmediatypescan_t scan_data_;
};

} // namespace http

} // namespace rest
#endif

#endif /* __LIBREST_HTTP_PARSER_HEADER_HTTPMEDIATYPEPARSER_H__ */