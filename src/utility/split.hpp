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

#ifndef __LIBREST_UTILITY_SPLIT_HPP__
#define __LIBREST_UTILITY_SPLIT_HPP__

#include <sstream>
#include <string>
#include <vector>

namespace rest
{

inline std::vector<std::string> split(const std::string& s, const char delemiter)
{
	std::vector<std::string> words;
	std::istringstream iss(s);
	std::string segment;
	while (std::getline(iss, segment, delemiter))
	{
		words.push_back(segment);
	}

	return words;
}

} // namespace rest

#endif // __LIBREST_UTILITY_SPLIT_HPP__
