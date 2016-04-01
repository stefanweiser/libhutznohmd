/* This file is part of libhutznohmd.
 * Copyright (C) 2013-2016 Stefan Weiser

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

#include "accept_parser.hpp"

namespace hutzn
{

accept_parser::accept_parser(void)
    : data_(NULL)
{
}

bool accept_parser::parse(char_t* const /*value_string*/,
                          const size_t /*value_length*/)
{
    return true;
}

bool accept_parser::next_value(void*& /*handle*/, mime& /*type*/) const
{
    return false;
}

} // namespace hutzn
