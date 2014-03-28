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

httpdateparser::httpdateparser(const std::string & buffer)
    : buffer_(buffer)
    , index_(0)
    , finished_(false)
    , error_(false)
    , scan_data_({this})
{}

void httpdateparser::parse()
{
    if (false == finished_) {
        httpdateparse(&scan_data_);
        finished_ = true;
    }
}

int httpdateparser::get()
{
    if (index_ < buffer_.size()) {
        return buffer_[index_++];
    }
    return -1;
}

void httpdateparser::set_error()
{
    error_ = true;
}

bool httpdateparser::valid() const
{
    return (false == error_);
}

} // namespace http

} // namespace rest



int httpdatelex(int * httpdatelval, httpdatescan_t * scanner)
{
    int result = scanner->parser->get();
    *httpdatelval = result;
    return result;
}

void httpdateerror(httpdatescan_t * scanner, const char * /*string*/)
{
    scanner->parser->set_error();
}
