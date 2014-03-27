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

#ifndef __LIBREST_HTTP_PARSER_LEXER_H__
#define __LIBREST_HTTP_PARSER_LEXER_H__

#include <http/parser/httpscan.h>

#ifdef __cplusplus
#include <functional>

extern "C"
{
#endif

int httplex(int * httplval, httpscan_t * scanner);
void httperror(httpscan_t * scanner, const char * string);
void http_finish(httpscan_t * scanner);

#ifdef __cplusplus
}

namespace rest
{

namespace http
{

class lexer
{
public:
    explicit lexer(const std::function<int()> & get_functor,
                   const std::function<int()> & peek_functor);
    void finish();
    bool finished() const;
    void error(const char * error_string);
    int get();

private:
    std::function<int()> get_functor_;
    std::function<int()> peek_functor_;
    char last_character_;
    bool finished_;
};

} // namespace http

} // namespace rest
#endif

#endif /* __LIBREST_HTTP_PARSER_LEXER_H__ */
