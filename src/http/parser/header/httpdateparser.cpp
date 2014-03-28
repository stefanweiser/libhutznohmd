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

#include "httpdateparser.h"

extern "C"
{

    int httpdateparse(httpdatescan_t * scanner);

}

namespace rest
{

namespace http
{

http_date_parser::http_date_parser(const std::string & buffer)
    : buffer_(buffer)
    , index_(0)
    , error_(false)
    , timestamp_(-1)
    , scan_data_({this})
{
    httpdateparse(&scan_data_);
}

int64_t http_date_parser::get()
{
    if (index_ < buffer_.size()) {
        return buffer_[index_++];
    }
    return -1;
}

void http_date_parser::set_error()
{
    error_ = true;
}

void http_date_parser::set_date(const time_t & t)
{
    timestamp_ = t;
}

bool http_date_parser::valid() const
{
    return (false == error_);
}

time_t http_date_parser::timestamp() const
{
    return timestamp_;
}

} // namespace http

} // namespace rest



int64_t httpdatelex(int64_t * httpdatelval, httpdatescan_t * scanner)
{
    int64_t result = scanner->parser->get();
    *httpdatelval = result;
    return result;
}

void httpdateerror(httpdatescan_t * scanner, const char * /*string*/)
{
    scanner->parser->set_error();
}

void httpdate_set_date(httpdatescan_t * scanner, const int64_t seconds_since_epoch)
{
    scanner->parser->set_date(seconds_since_epoch);
}
