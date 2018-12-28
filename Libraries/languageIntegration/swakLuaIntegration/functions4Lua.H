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
    Functions that extend Lua to handle swak4Foam-stuff "in-place"

Contributors/Copyright:
    2018 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>

 SWAK Revision: $Id:  $
\*---------------------------------------------------------------------------*/

#ifndef FUNCTIONS4LUA_H
#define FUNCTIONS4LUA_H

extern "C" {
        # include "lua.h"
        # include "lauxlib.h"
        # include "lualib.h"
}

#include "Field.H"

namespace Foam {

    // add functions and data types to the Lua interpreter
    void addOpenFOAMFunctions(lua_State *luaState);

    // add a field to the Lua dataspace. It references the real data
    template<class T>
    void addFieldToLua(lua_State *luaState,const word &name, Field<T> *data,label index=0);

    // add a field to the Lua dataspace. Creates a copy of the data
    template<class T>
    void addFieldToLua(lua_State *luaState,const word &name, const List<T> &data,label index=0);

    template<class T>
    bool isLuaFieldWrap(lua_State *luaState,int index);

    template<class T>
    Field<T> &getLuaField(lua_State *luaState,int index);
}
#endif
