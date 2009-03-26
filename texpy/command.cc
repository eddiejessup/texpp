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

    /*
    bool parseArgs(Parser& p, Node::ptr n) {
        if(override f = this->get_override("parseArgs"))
            return f(p, n);
        return Cmd::parseArgs(p, n);
    }

    bool default_parseArgs(Parser& p, Node::ptr n) {
        return Cmd::parseArgs(p, n);
    }

    bool execute(Parser& p, Node::ptr n) {
        if(override f = this->get_override("execute"))
            return f(p, n);
        return Cmd::execute(p, n);
    }

    bool default_execute(Parser& p, Node::ptr n) {
        return Cmd::execute(p, n);
    }*/
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
        ;

    export_derived_command<TokenCommand, bases<Command>,
                init<Token::ptr> >("TokenCommand");

}

