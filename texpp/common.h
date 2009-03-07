/* This file is part of texpp.
   Copyright (C) 2009 Vladimir Kuznetsov <ks.vladimir@gmail.com>

   texpp is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 2 of the License, or
   (at your option) any later version.

   texpp is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with texpp.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef __TEXPP_COMMON_H
#define __TEXPP_COMMON_H

#include <string>
#include <vector>

//#include <tr1/memory>
#include <boost/shared_ptr.hpp>
#include <boost/any.hpp>

#include <tr1/unordered_map>

namespace texpp {
    using std::string;
    using std::vector;
    using std::pair;
    using std::tr1::unordered_map;
    //using std::tr1::shared_ptr;
    //using std::tr1::weak_ptr;

    using boost::shared_ptr;
    using boost::weak_ptr;
    using boost::dynamic_pointer_cast;
    using boost::static_pointer_cast;

    using boost::any;

    string reprString(const string& s);
    string reprAny(const any& value);
} // namespace texpp

#endif

