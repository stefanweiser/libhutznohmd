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

#ifndef __LIBREST_HTTP_PARSER_HEADER_HTTPDATEPARSER_H__
#define __LIBREST_HTTP_PARSER_HEADER_HTTPDATEPARSER_H__

#ifdef __cplusplus
#include <ctime>
#include <map>
#include <string>

extern "C"
{
#endif

typedef struct httpdatescan httpdatescan_t;

int64_t httpdatelex(int64_t * httpdatelval, httpdatescan_t * scanner);
void httpdateerror(httpdatescan_t * scanner, const char * string);
void httpdate_set_date(httpdatescan_t * scanner, const int64_t seconds_since_epoch);

#ifdef __cplusplus
}

namespace rest
{

namespace http
{

class httpdateparser;

} // namespace http

} // namespace rest

typedef struct httpdatescan {
    rest::http::httpdateparser * parser;
} httpdatescan_t;

namespace rest
{

namespace http
{

class httpdateparser
{
public:
    explicit httpdateparser(const std::string & buffer);

    int64_t get();
    void set_error();
    void set_date(const time_t & t);

    bool valid() const;
    time_t timestamp() const;

private:
    const std::string & buffer_;
    size_t index_;
    bool error_;
    time_t timestamp_;

    httpdatescan_t scan_data_;
};

} // namespace http

} // namespace rest
#endif

#endif /* __LIBREST_HTTP_PARSER_HEADER_HTTPDATEPARSER_H__ */
