/* This file is part of librestsrv.
 * Copyright (C) 2013-2014 Stefan Weiser

 * The librestsrv project is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 3.0 of the License, or (at your
 * option) any later version.

 * The librestsrv project is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public
 * License for more details.

 * You should have received a copy of the GNU Lesser General Public License
 * along with the librestsrv project; if not, see <http://www.gnu.org/licenses/>.
 */

#error "Do not include this pure documentation file."

/**
@page development Development

@todo [DOC] Fill developer informations.
@todo [DOC] Fill concepts.
@todo [DOC] Fill links to todo and bug list.

@section sec_principles Development Principles

To aim a homogenous code base, it is indispensable to have some principles on how
to work.

I spent much time on porting code of software dinosaurs and therefor
reengineering the meaning of it. Often I have chosen to rewrite it component by
component, because the original code is somewhat bloated. Sometimes I got tired
of fixing or removing the one-thousand-and-first idea of slightly improving the
performance at the expense of a massively higher risk of raising maintenance
costs, where another improvement would do the same, but more reliable.

However, there are some principles, that are essential:

@li \b KISS: The code has to be as simple as possible. Any non-intuitive solution
for a problem should be documented.
@li \b SOLID: This should keep the architecture extensible.
@li \b AAA: The tests should be arranged in that pattern, but there are good
reasons that not all tests may fulfill this. A triple-A test is better than one
that is arranged in another way.
@li \b YAGNI: Keep this in mind. If some code is not needed anymore, it shall be
removed.

In any cases it will be difficult where to draw the line. Don't be religious with
them, but keep them in mind and improve the code.

@section sec_who Who can contribute?

Anyone who is passionated in developing software and shared the principles of
this project is welcome to contribute to it.

*/

