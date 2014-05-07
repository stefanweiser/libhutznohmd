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

/*!
@page development Development

@todo [DOC] Fill concepts.

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

@li @b KISS: The code has to be as simple as possible. Any non-intuitive solution
for a problem should be documented.
@li @b SOLID: This should keep the architecture extensible.
@li @b AAA: The tests should be arranged in that pattern, but there are good
reasons that not all tests may fulfill this. A triple-A test is better than one
that is arranged in another way.
@li @b YAGNI: Keep this in mind. If some code is not needed anymore, it shall be
removed.

In any cases it will be difficult where to draw the line. Don't be religious with
them, but keep them in mind and improve the code.

@section sec_who Who can contribute?

Anyone who is passionated in developing software and shared the principles of
this project is welcome to contribute to it. Please use a pull request on GitHub.

@section sec_tools Tools

Generally:
@li @b bash >= 4.2
@li @b git >= 1.7.9
@li @b cmake >= 2.8

For building:
@li @b g++ >= 4.8 with complete C++11 support.
@li @b cppcheck >= 1.60
@li @b astyle >= 2.03

For developing:
@li @b qtcreator >= 2.8

For documentation:
@li @b doxygen >= 1.7.6
@li @b dot >= 2.26
@li @b java >= 1.6

For coverage:
@li @b gcov >= 4.8
@li @b lcov >= 1.9

For valgrind:
@li @b valgrind >= 3.7

For packaging:
@li @b debhelper
@li @b rpmbuild >= 4.9
@li @b tar >= 1.26
@todo Find out how to build deb files in rpm based distributions.

@section sec_concepts Concepts

@li @ref concept_overview

*/

