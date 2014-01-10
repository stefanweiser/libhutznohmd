/* This file is part of librest.
 * Copyright (C) 2013 Stefan Weiser

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

#include <iostream>
#include <sstream>
#include <vector>

#include <utility/split.hpp>

#include "mime.hpp"

namespace rest
{

namespace http
{

MimeType::MimeType(const std::string & string)
	: m_string(string)
	, m_type(Type::Wildcard)
	, m_subtype(Subtype::Wildcard)
	, m_quality(10)
	, m_extensions()
{
	std::vector<std::string> words = split(m_string, ';');
	if ( words.empty() )
	{
		return;
	}
}

} // namespace http

} // namespace rest
