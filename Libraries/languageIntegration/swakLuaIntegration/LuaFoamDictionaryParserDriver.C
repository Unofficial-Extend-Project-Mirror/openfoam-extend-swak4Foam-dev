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
    Parse to Lua

Contributors/Copyright:
    2017-2018 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>

 SWAK Revision: $Id:  $
\*---------------------------------------------------------------------------*/

#include "LuaFoamDictionaryParserDriver.H"
#include <cassert>

#include "functions4Lua.H"

namespace Foam {
    LuaFoamDictionaryParserDriver::LuaFoamDictionaryParserDriver(
        luaInterpreterWrapper &parent,
        ErrorMode mode
    )
        :
        RawFoamDictionaryParserDriver(mode),
        parent_(parent)
    {
    }

    LuaFoamDictionaryParserDriver::~LuaFoamDictionaryParserDriver()
    {
    }

    void LuaFoamDictionaryParserDriver::add(const word& name,scalar value)
    {
        lua_pushnumber(lua(),value);
        lua_setfield(lua(),-2,name.c_str());
    }

    void LuaFoamDictionaryParserDriver::add(const word& name,label value)
    {
        lua_pushinteger(lua(),value);
        lua_setfield(lua(),-2,name.c_str());
    }

    void LuaFoamDictionaryParserDriver::add(const word& name,bool value)
    {
        lua_pushboolean(lua(),value);
        lua_setfield(lua(),-2,name.c_str());
    }

    void LuaFoamDictionaryParserDriver::add(const word& name,const word &value)
    {
        lua_pushstring(lua(),value.c_str());
        lua_setfield(lua(),-2,name.c_str());
    }

    void LuaFoamDictionaryParserDriver::add(const word& name,const string &value)
    {
        lua_pushstring(lua(),value.c_str());
        lua_setfield(lua(),-2,name.c_str());
    }

    void LuaFoamDictionaryParserDriver::add(const word& name,const labelList &value)
    {
        if(
            parent_.parseToWrappedField()
            &&
            parent_.wrapBoolLabelAsScalar()
        ) {
            scalarList svalue(value.size());
            forAll(svalue,i) {
                svalue[i]=value[i];
            }
            addFieldToLua(lua(),name,svalue,-2);
        } else {
            lua_newtable(lua());
            forAll(value,i) {
                lua_pushinteger(lua(),value[i]);
                lua_seti(lua(),-2,i+1);
            }
            lua_setfield(lua(),-2,name.c_str());
        }
    }

    void LuaFoamDictionaryParserDriver::add(const word& name,const scalarList &value)
    {
        if(parent_.parseToWrappedField()) {
            addFieldToLua(lua(),name,value,-2);
        } else {
            lua_newtable(lua());
            forAll(value,i) {
                lua_pushnumber(lua(),value[i]);
                lua_seti(lua(),-2,i+1);
            }
            lua_setfield(lua(),-2,name.c_str());
        }
    }

    void LuaFoamDictionaryParserDriver::add(const word& name,const boolList &value)
    {
        if(
            parent_.parseToWrappedField()
            &&
            parent_.wrapBoolLabelAsScalar()
        ) {
            scalarList svalue(value.size());
            forAll(svalue,i) {
                svalue[i]=value[i];
            }
            addFieldToLua(lua(),name,svalue,-2);
        } else {
            lua_newtable(lua());
            forAll(value,i) {
                lua_pushboolean(lua(),value[i]);
                lua_seti(lua(),-2,i+1);
            }
            lua_setfield(lua(),-2,name.c_str());
        }
    }

    void LuaFoamDictionaryParserDriver::add(const word& name,const wordList &value)
    {
        lua_newtable(lua());
        forAll(value,i) {
            lua_pushstring(lua(),value[i].c_str());
            lua_seti(lua(),-2,i+1);
        }
        lua_setfield(lua(),-2,name.c_str());
    }

    void LuaFoamDictionaryParserDriver::add(const word& name,const stringList &value)
    {
        lua_newtable(lua());
        forAll(value,i) {
            lua_pushstring(lua(),value[i].c_str());
            lua_seti(lua(),-2,i+1);
        }
        lua_setfield(lua(),-2,name.c_str());
    }

    void LuaFoamDictionaryParserDriver::add(const word& name,const List<labelList> &value)
    {
        lua_newtable(lua());
        forAll(value,i) {
            lua_newtable(lua());
            forAll(value[i],j) {
                lua_pushinteger(lua(),value[i][j]);
                lua_seti(lua(),-2,j+1);
            }
            lua_seti(lua(),-2,i+1);
        }
        lua_setfield(lua(),-2,name.c_str());
    }

    void LuaFoamDictionaryParserDriver::add(const word& name,const List<scalarList> &value)
    {
        lua_newtable(lua());
        forAll(value,i) {
            lua_newtable(lua());
            forAll(value[i],j) {
                lua_pushnumber(lua(),value[i][j]);
                lua_seti(lua(),-2,j+1);
            }
            lua_seti(lua(),-2,i+1);
        }
        lua_setfield(lua(),-2,name.c_str());
    }

    void LuaFoamDictionaryParserDriver::add(const word& name,const List<boolList> &value)
    {
        lua_newtable(lua());
        forAll(value,i) {
            lua_newtable(lua());
            forAll(value[i],j) {
                lua_pushboolean(lua(),value[i][j]);
                lua_seti(lua(),-2,j+1);
            }
            lua_seti(lua(),-2,i+1);
        }
        lua_setfield(lua(),-2,name.c_str());
    }

    void LuaFoamDictionaryParserDriver::add(const word& name,const List<wordList> &value)
    {
        lua_newtable(lua());
        forAll(value,i) {
            lua_newtable(lua());
            forAll(value[i],j) {
                lua_pushstring(lua(),value[i][j].c_str());
                lua_seti(lua(),-2,j+1);
            }
            lua_seti(lua(),-2,i+1);
        }
        lua_setfield(lua(),-2,name.c_str());
    }

    void LuaFoamDictionaryParserDriver::add(const word& name,const List<stringList> &value)
    {
        lua_newtable(lua());
        forAll(value,i) {
            lua_newtable(lua());
            forAll(value[i],j) {
                lua_pushstring(lua(),value[i][j].c_str());
                lua_seti(lua(),-2,j+1);
            }
            lua_seti(lua(),-2,i+1);
        }
        lua_setfield(lua(),-2,name.c_str());
    }

    void LuaFoamDictionaryParserDriver::addTopDictAs(const word &name) {
        lua_setfield(lua(),-2,name.c_str());
    }

    void LuaFoamDictionaryParserDriver::newTopDict() {
        lua_newtable(lua());
    }

}
