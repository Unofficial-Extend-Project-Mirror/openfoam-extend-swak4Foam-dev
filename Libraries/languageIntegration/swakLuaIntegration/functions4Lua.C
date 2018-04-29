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
    Functions that extend Lua to handle OpenFOAM-stuff "in-place"

Contributors/Copyright:
    2018 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>

 SWAK Revision: $Id:  $
\*---------------------------------------------------------------------------*/

#include "functions4Lua.H"

#include <cmath>

void* operator new(size_t size, lua_State* L, const char* metatableName)
{
    void* ptr = lua_newuserdata(L, size);
    luaL_getmetatable(L, metatableName);
    // assert(lua_istable(L, -1)) if you're paranoid
    lua_setmetatable(L, -2);
    return ptr;
}

namespace Foam {

    template<class T>
    class FieldWrap {
        Field<T> *otherData_;
        autoPtr<Field<T> > ownData_;
    public:
        FieldWrap(Field<T> *data)
            : otherData_(data)
            {}
        FieldWrap(label size)
            : otherData_(NULL),
              ownData_(
                  new Field<T>(
                      size,
                      pTraits<T>::zero
                  )
              )
            {}
        ~FieldWrap() {}

        Field<T> &operator()() {
            if(otherData_!=NULL) {
                return *otherData_;
            } else {
                return ownData_();
            }
        }
        static const char *metaTable;
    };

    template<>
    const char *FieldWrap<scalar>::metaTable=
        "swak4Foam.scalarArray";

    template<class T>
    static FieldWrap<T> *checkField(lua_State *L, int index)
    {
        FieldWrap<T> *data;
        luaL_checktype(L, index, LUA_TUSERDATA);
        data = (FieldWrap<T> *)luaL_checkudata(L, index, FieldWrap<T>::metaTable);
        return data;
    }

    template<class T>
    static int newField(lua_State *L) {
        int n = label(luaL_checkinteger(L, 1));
        FieldWrap<T> *fw=new(L,FieldWrap<T>::metaTable) FieldWrap<T>(n);

        return 1;
    }

    template<class T>
    static int delField(lua_State *L) {
        FieldWrap<T> *fw=checkField<T>(L,1);
        fw->~FieldWrap<T>();
        return 0;
    }

    template<class T>
    static int fieldSize(lua_State *L) {
        FieldWrap<T> *fw=checkField<T>(L,1);
        lua_pushinteger(L, (*fw)().size());
        return 1;
    }

    template<class T>
    static int getFieldElement(lua_State *L) {
        FieldWrap<T> *fw=checkField<T>(L,1);
        int index = (int)luaL_checkinteger(L, 2) - 1;
        luaL_argcheck(
            L, 0 <= index && index < (*fw)().size(), 2,
            "index out of range");

        lua_pushnumber(L, (*fw)()[index]);

        return 1;
    }

    template<class T>
    static int setFieldElement(lua_State *L) {
        FieldWrap<T> *fw=checkField<T>(L,1);
        int index = (int)luaL_checkinteger(L, 2) - 1;
        luaL_argcheck(
            L, 0 <= index && index < (*fw)().size(), 2,
            "index out of range");

        luaL_checkany(L,3);
        T v=luaL_checknumber(L,3);
        (*fw)()[index]=v;

        return 1;
    }

    template<class T>
    static int fieldToString(lua_State *L) {
        FieldWrap<T> *fw=checkField<T>(L,1);
        lua_pushfstring(
            L,
            "swak4Foam.%sField with %d elements",
            pTraits<T>::typeName,
            (*fw)().size()
        );
        return 1;
    }

    static const struct luaL_Reg swaklib [] = {
        {"newScalarField", newField<scalar>},
        {NULL, NULL}  /* sentinel */
    };

    static const struct luaL_Reg scalarArray_table [] = {
        {"__gc"        , delField<scalar> },
        {"__len"       , fieldSize<scalar> },
        {"__index"     , getFieldElement<scalar> },
        {"__newindex"  , setFieldElement<scalar> },
        {"__tostring"  , fieldToString<scalar> },
        {NULL, NULL}  /* sentinel */
    };

    void addOpenFOAMFunctions(lua_State *luaState) {
        luaL_newmetatable(luaState, FieldWrap<scalar>::metaTable);
        luaL_setfuncs(luaState, scalarArray_table, 0);
        lua_pop(luaState, 1);

        luaL_newlib(luaState,swaklib);

        lua_setglobal(luaState, "swak4foam");

        // static Field<scalar> sField(10);
        // addFieldToLua<scalar>(luaState, "testScalar", &sField);
    }

    template<class T>
    void addFieldToLua(lua_State *luaState,const word &name, Field<T> *data) {
        FieldWrap<T> *fw=new(luaState,FieldWrap<T>::metaTable) FieldWrap<T>(data);

        lua_setglobal(luaState, name.c_str());
    }
}
