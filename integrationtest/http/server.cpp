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

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#define private public
#define protected public

#include <http/server.hpp>

using namespace testing;

TEST(Server, NormalUseCase)
{
//  bool called = false;
//  auto transaction = [&called] (
//      const rest::http::RequestInterface & /*request*/,
//      rest::http::ResponseInterface & /*response*/)
//  {
//      called = true;
//  };

//  std::shared_ptr<rest::http::ServerInterface> server;
//  server = rest::http::createServer("localhost", 10000, transaction);

//  std::thread t(std::bind(&rest::http::ServerInterface::run, server.get()));

//  server->stop();
//  t.join();
}
