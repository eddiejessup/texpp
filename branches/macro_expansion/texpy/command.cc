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
    CommandWrap(const string& name = string())
        : Cmd(name) {}

    string texRepr(Parser* parser) const {
        if(override f = this->get_override("texRepr"))
            return f(parser);
        return this->Cmd::texRepr(parser);
    }

    string default_texRepr(Parser* parser) const {
        return this->Cmd::texRepr(parser);
    }

    bool invoke(Parser& p, Node::ptr n) {
        if(override f = this->get_override("invoke"))
            return f(boost::ref(p), n);
        return this->Cmd::invoke(p, n);
    }

    bool default_invoke(Parser& p, Node::ptr n) {
        return this->Cmd::invoke(p, n);
    }

    bool checkPrefixes(Parser& p) {
        if(override f = this->get_override("checkPrefixes"))
            return f(boost::ref(p));
        return Cmd::checkPrefixes(p);
    }

    bool default_checkPrefixes(Parser& p) {
        return Cmd::checkPrefixes(p);
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
        .def("__repr__", &Cmd::repr)
        .def("texRepr", &Cmd::texRepr,
                    &CommandWrap<Cmd>::default_texRepr)
        .def("invoke", &Command::invoke,
                    &CommandWrap<Cmd>::default_invoke)
        .def("checkPrefixes", &Command::checkPrefixes,
                    &CommandWrap<Cmd>::default_checkPrefixes)
        ;
}

void export_command()
{
    using namespace boost::python;
    using namespace texpp;

    class_<CommandWrap<Command>, shared_ptr< CommandWrap<Command> >, boost::noncopyable >(
           "Command", init<std::string>())
        .def("__repr__", &Command::repr)
        .def("name", &Command::name,
            return_value_policy<copy_const_reference>())
        .def("texRepr", &Command::texRepr,
                    &CommandWrap<Command>::default_texRepr)
        .def("invoke", &Command::invoke,
                    &CommandWrap<Command>::default_invoke)
        .def("checkPrefixes", &Command::checkPrefixes,
                    &CommandWrap<Command>::default_checkPrefixes)
        ;

    export_derived_command<TokenCommand, bases<Command>,
                init<Token::ptr> >("TokenCommand");

}

