/*----------------------- -*- C++ -*- ---------------------------------------*\
|                       _    _  _     ___                       | The         |
|     _____      ____ _| | _| || |   / __\__   __ _ _ __ ___    | Swiss       |
|    / __\ \ /\ / / _` | |/ / || |_ / _\/ _ \ / _` | '_ ` _ \   | Army        |
|    \__ \\ V  V / (_| |   <|__   _/ / | (_) | (_| | | | | | |  | Knife       |
|    |___/ \_/\_/ \__,_|_|\_\  |_| \/   \___/ \__,_|_| |_| |_|  | For         |
|                                                               | OpenFOAM    |
-------------------------------------------------------------------------------
License
    This file is part of swak4Foam.

    swak4Foam is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    swak4Foam is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with swak4Foam.  If not, see <http://www.gnu.org/licenses/>.

Description
    Parse to a Lua

Contributors/Copyright:
    2017-2018 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>

 SWAK Revision: $Id:  $
\*---------------------------------------------------------------------------*/

#ifndef LUAFOAMDICTIONARYPARSERDRIVER_H
#define LUAFOAMDICTIONARYPARSERDRIVER_H

#include "RawFoamDictionaryParserDriver.H"

#include "luaInterpreterWrapper.H"

namespace Foam {
    class LuaFoamDictionaryParserDriver
    :
        public RawFoamDictionaryParserDriver
    {

        LuaFoamDictionaryParserDriver(const LuaFoamDictionaryParserDriver &);
        void operator=(const LuaFoamDictionaryParserDriver&);

        luaInterpreterWrapper &parent_;

        lua_State *lua() {
           return parent_.luaState();
        }

    public:

        LuaFoamDictionaryParserDriver(
            luaInterpreterWrapper &parent,
            ErrorMode mode
        );

        virtual ~LuaFoamDictionaryParserDriver();

        const dictionary &getResult() const;

        void add(const word& name,scalar value);
        void add(const word& name,label value);
        void add(const word& name,bool value);
        void add(const word& name,const word &value);
        void add(const word& name,const string &value);
        void add(const word& name,const labelList &value);
        void add(const word& name,const scalarList &value);
        void add(const word& name,const boolList &value);
        void add(const word& name,const wordList &value);
        void add(const word& name,const stringList &value);
        void add(const word& name,const List<labelList> &value);
        void add(const word& name,const List<scalarList> &value);
        void add(const word& name,const List<boolList> &value);
        void add(const word& name,const List<wordList> &value);
        void add(const word& name,const List<stringList> &value);


        void addTopDictAs(const word &name);
        void newTopDict();

    };
}

#endif /* LUAFOAMDICTIONARYPARSERDRIVER_H */
