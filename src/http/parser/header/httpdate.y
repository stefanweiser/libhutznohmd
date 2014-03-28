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

%{
#include <stdint.h>

#include <http/parser/header/httpdateparser.h>
#include "httpdate.h"
%}

%pure-parser
%define api.prefix "httpdate"
%lex-param {httpdatescan_t * scanner}
%parse-param {httpdatescan_t * scanner}

%%

httpdate:       whitespace specific-date whitespace

specific-date:  rfc1123-date | rfc850-date | asctime-date

rfc1123-date:   wkday whitespace
                ',' whitespace
                integer whitespace
                month whitespace
                integer whitespace-char whitespace
                time whitespace
                gmt

rfc850-date:    weekday
                ',' whitespace
                integer whitespace
                '-' whitespace
                month whitespace
                '-' whitespace
                integer whitespace-char whitespace
                time whitespace
                gmt

asctime-date:   wkday whitespace-char whitespace
                month whitespace
                integer whitespace-char whitespace
                time whitespace-char whitespace
                integer

time:           integer ':' integer ':' integer

gmt: g m t

wkday: mon | tue | wed | thu | fri | sat | sun
weekday: monday | tuesday | wednesday | thursday | friday | saturday | sunday
month: jan | feb | mar | apr | may | jun | jul | aug | sep | oct | nov | dec

monday: mon d a y
tuesday: tue s d a y
wednesday: wed n e s d a y
thursday: thu r s d a y
friday: fri d a y
saturday: sat u r d a y
sunday: sun d a y

mon: m o n
tue: t u e
wed: w e d
thu: t h u
fri: f r i
sat: s a t
sun: s u n

jan: j a n
feb: f e b
mar: m a r
apr: a p r
may: m a y
jun: j u n
jul: j u l
aug: a u g
sep: s e p
oct: o c t
nov: n o v
dec: d e c

whitespace: whitespace whitespace-char |
whitespace-char: '\t' | ' '

integer: integer digit | digit
digit: '0' | '1' | '2' | '3' | '4' | '5' | '6' | '7' | '8' | '9'

a: 'A' | 'a'
b: 'B' | 'b'
c: 'C' | 'c'
d: 'D' | 'd'
e: 'E' | 'e'
f: 'F' | 'f'
g: 'G' | 'g'
h: 'H' | 'h'
i: 'I' | 'i'
j: 'J' | 'j'
l: 'L' | 'l'
m: 'M' | 'm'
n: 'N' | 'n'
o: 'O' | 'o'
p: 'P' | 'p'
r: 'R' | 'r'
s: 'S' | 's'
t: 'T' | 't'
u: 'U' | 'u'
v: 'V' | 'v'
w: 'W' | 'w'
y: 'Y' | 'y'

%%

