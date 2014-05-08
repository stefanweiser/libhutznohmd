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
@page deployment Deployment

@section sec_quick_start Quick Start

All deployment steps are done by the @c make script. A help could be printed out
by:

@code
$ ./make --help
@endcode

Normally making @c all in both targets (@c debug and @c release) and making
@c coverage is of interest.

@section sec_deploying Deploying

Deploying a version is done by incrementing the version and making a package:

@code
./make all release
@endcode

These packages are ought to be released. Create a tag on the git repository
afterwards.

*/
