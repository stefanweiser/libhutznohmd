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

#include "httpmediatypeparser.h"

extern "C"
{

    int httpmediatypeparse(httpmediatypescan_t * scanner);

}

namespace rest
{

namespace http
{

http_media_type_parser::http_media_type_parser(const std::string & buffer)
    : buffer_(buffer)
    , index_(0)
    , error_(false)
    , scan_data_({this})
{
    httpmediatypeparse(&scan_data_);
}

int http_media_type_parser::get()
{
    if (index_ < buffer_.size()) {
        return buffer_[index_++];
    }
    return -1;
}

void http_media_type_parser::set_error()
{
    error_ = true;
}

bool http_media_type_parser::valid() const
{
    return (false == error_);
}

} // namespace http

} // namespace rest



int httpmediatypelex(int * httpmediatypelval, httpmediatypescan_t * scanner)
{
    *httpmediatypelval = scanner->parser->get();
    return *httpmediatypelval;
}

void httpmediatypeerror(httpmediatypescan_t * scanner, const char * /*string*/)
{
    scanner->parser->set_error();
}