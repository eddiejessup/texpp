/*  This file is part of texpp library.
    Copyright (C) 2009 Vladimir Kuznetsov <ks.vladimir@gmail.com>

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include <boost/python.hpp>
#include <string>
#include <cstring>

extern "C" {
struct stemmer;

extern struct stemmer * create_stemmer(void);
extern void free_stemmer(struct stemmer * z);

extern int stem(struct stemmer * z, char * b, int k);
}

namespace {

class Stemmer {
public:
    Stemmer() { _stemmer = create_stemmer(); }
    ~Stemmer() { free_stemmer(_stemmer); }

    std::string stem(std::string word)
    {
        /* TODO: remove unnessesary conversion to/from std::string */
        int n = word.size();
        char buf[n];
        std::memcpy(buf, word.data(), n);
        n = ::stem(_stemmer, buf, n-1);
        return std::string(buf, n+1);
    }

protected:
    struct stemmer* _stemmer;
};

} // namespace

BOOST_PYTHON_MODULE(_chrefliterals)
{
    using namespace boost::python;
    class_<Stemmer>("Stemmer", init<>())
        .def("stem", &Stemmer::stem);
}

