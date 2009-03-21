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

#ifndef __TEXPP_BASE_FONT_H
#define __TEXPP_BASE_FONT_H

#include <texpp/common.h>
#include <texpp/command.h>

#include <texpp/base/variable.h>

namespace texpp {
namespace base {

class FontSelector: public Command
{
public:
    FontSelector(const string& name, const string& fontName)
        : Command(name), m_fontName(fontName) {}

    const string& fontName() const { return m_fontName; }
    void setFontName(const string& fontName) { m_fontName = fontName; }

    bool parseArgs(Parser&, shared_ptr<Node>) { return true; }
    bool execute(Parser&, shared_ptr<Node>);

    string texRepr(char escape = '\\') const;

protected:
    string m_fontName;
};

/*
class Font: public FixedCommandGroup<FontSelector>
{
public:
    Font(const string& name): FixedCommandGroup<FontSelector>(name, 1) {}

    virtual Command::ptr item(size_t n) { return Command::ptr(); }
    Command::ptr parseCommand(Parser& parser, shared_ptr<Node> node) {
        return item(0);
    }
    //bool parseArgs(Parser& parser, shared_ptr<Node> node);
};
*/

/*
class FontSelector: public FontBase
{

};
*/
} // namespace base
} // namespace texpp

#endif

