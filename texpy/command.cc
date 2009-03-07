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

#include <boost/python.hpp>
#include <texpp/parser.h>

namespace texpp { namespace {

using boost::python::wrapper;
using boost::python::override;

template<class Cmd>
class CommandWrap: public Cmd, public wrapper<Cmd>
{
public:
    string texRepr(char escape) const {
        if(override f = this->get_override("texRepr"))
            return f(escape);
        return Cmd::texRepr();
    }

    string default_texRepr(char escape) const {
        return Cmd::texRepr(escape);
    }

    Node::ptr parse(Parser& p) {
        if(override f = this->get_override("parse"))
            return f(p);
        return Cmd::parse(p);
    }

    Node::ptr default_parse(Parser& p) {
        return Cmd::parse(p);
    }

    bool execute(Parser& p, Node::ptr n) {
        if(override f = this->get_override("execute"))
            return f(p, n);
        return Cmd::execute(p, n);
    }

    bool default_execute(Parser& p, Node::ptr n) {
        return Cmd::execute(p, n);
    }
};

}}

template<class Cmd, class _bases, class _init>
inline void export_derived_command(const char* name)
{
    using namespace boost::python;
    using namespace texpp;

    class_<CommandWrap<Cmd>, shared_ptr<Cmd>, _bases >(
           name, _init())
        .def("texRepr", &Cmd::texRepr,
                    &CommandWrap<Cmd>::default_texRepr)
        .def("parse", &Cmd::parse,
                    &CommandWrap<Cmd>::default_parse)
        .def("execute", &Cmd::execute,
                    &CommandWrap<Cmd>::default_execute)
        ;
}

void export_command()
{
    using namespace boost::python;
    using namespace texpp;

    class_<CommandWrap<Command>, shared_ptr<Command> >(
           "Command", init<std::string>())
        .def("name", &Command::name,
            return_value_policy<copy_const_reference>())
        .def("texRepr", &Command::texRepr,
                    &CommandWrap<Command>::default_texRepr)
        .def("parse", &Command::parse,
                    &CommandWrap<Command>::default_parse)
        .def("execute", &Command::execute,
                    &CommandWrap<Command>::default_execute)
        ;

    export_derived_command<TokenCommand, bases<Command>,
                init<Token::ptr> >("TokenCommand");

}

