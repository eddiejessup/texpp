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

#include <iostream>
#include <fstream>
#include <string>

#include <texpp/parser.h>

const char banner[] = "This is TeXpp, Version 0.0";

int main(int argc, char** argv)
{
    std::string fileName;
    std::istream *file;
    bool interactive;

    if(argc >= 2) {
        interactive = false;
        fileName = argv[1];
        file = new std::ifstream(argv[1], std::fstream::in);
        if(file->fail()) {
            std::cerr << "Can not open file " << argv[1] << std::endl;
            delete file;
            return 255;
        }
    } else {
        interactive = true;
        std::cout << banner << std::endl;
        file = &std::cin;
    }

    texpp::Parser parser(fileName, file, interactive);
    texpp::Node::ptr document = parser.parse();

    std::cout << "Parsed document: " << std::endl;
    std::cout << document->treeRepr();
    
    if(file != &std::cin) {
        static_cast<std::ifstream*>(file)->close();
        delete file;
    }

    return 0;
}

