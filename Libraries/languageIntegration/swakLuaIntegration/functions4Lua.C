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

    // static int l_sin (lua_State *L) {
    //     double d = lua_tonumber(L, 1);  /* get argument */
    //     lua_pushnumber(L, sin(d));  /* push result */
    //     return 1;  /* number of results */
    // }

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
    static FieldWrap<T> *toField (lua_State *L, int index)
    {
        FieldWrap<T> *data = (FieldWrap<T> *)lua_touserdata(L, index);
        if (data == NULL) {
            luaL_typerror(L, index, FieldWrap<T>::metaTable);
        }
        return data;
    }

    template<class T>
    static FieldWrap<T> *checkField (lua_State *L, int index)
    {
        FieldWrap<T> *data;
        luaL_checktype(L, index, LUA_TUSERDATA);
        data = (FieldWrap<T> *)luaL_checkudata(L, index, FieldWrap<T>::metaTable);
        if (data == NULL) {
            luaL_typerror(L, index, pTraits<T>::typeName+"Array");
        }
        return data;
    }

    template<class T>
    static int newField(lua_State *L) {
        //        FieldWrap<T> *fw=(FieldWrap<T>*)lua_touserdata(L,1);
        //        luaL_argcheck(L, fw != NULL, 1, "'FieldWrap' expected");
        //        lua_pushinteger(L, (*fw)().size());
        int n = label(luaL_checkinteger(L, 1));
        FieldWrap<T> *fw=new(L,FieldWrap<T>::metaTable) FieldWrap<T>(n);

        return 1;
    }

    template<class T>
    static int delField(lua_State *L) {
        //        Info << "Deleting" << endl;
        static_cast<FieldWrap<T>*>(lua_touserdata(L, 1))->~FieldWrap<T>();
        return 0;
    }

    template<class T>
    static int fieldSize(lua_State *L) {
        FieldWrap<T> *fw=(FieldWrap<T>*)lua_touserdata(L,1);
        luaL_argcheck(L, fw != NULL, 1, "'FieldWrap' expected");
        lua_pushinteger(L, (*fw)().size());
        return 1;
    }

    template<class T>
    static int getElement(lua_State *L) {
        FieldWrap<T> *fw=(FieldWrap<T>*)lua_touserdata(L,1);
        luaL_argcheck(L, fw != NULL, 1, "'FieldWrap' expected");
        int index = (int)luaL_checkinteger(L, 2) - 1;
        luaL_argcheck(
            L, 0 <= index && index < (*fw)().size(), 2,
            "index out of range");

        lua_pushnumber(L, (*fw)()[index]);

        return 1;
    }

    template<class T>
    static int setElement(lua_State *L) {
        FieldWrap<T> *fw=(FieldWrap<T>*)lua_touserdata(L,1);
        luaL_argcheck(L, fw != NULL, 1, "'FieldWrap' expected");
        int index = (int)luaL_checkinteger(L, 2) - 1;
        luaL_argcheck(
            L, 0 <= index && index < (*fw)().size(), 2,
            "index out of range");

        luaL_checkany(L,3);
        T v=luaL_checknumber(L,3);
        (*fw)()[index]=v;

        return 1;
    }

    static const struct luaL_Reg swaklib [] = {
        // {"sin",l_sin},
        {"newScalarField", newField<scalar>},
        {NULL, NULL}  /* sentinel */
    };

    static const struct luaL_Reg scalarArray_table [] = {
        {"__gc"        , delField<scalar> },
        {"__len"       , fieldSize<scalar> },
        {"__index"     , getElement<scalar> },
        {"__newindex"  , setElement<scalar> },
        {NULL, NULL}  /* sentinel */
    };

    void addOpenFOAMFunctions(lua_State *luaState) {
        luaL_newmetatable(luaState, FieldWrap<scalar>::metaTable);
        luaL_setfuncs(luaState, scalarArray_table, 0);
        lua_pop(luaState, 1);

        luaL_newlib(luaState,swaklib);

        lua_setglobal(luaState, "swak4foam");
    }

    // void addScalarField(lua_State *luaState,const word &name, Field<scalar> *data) {
    //     FieldWrap<scalar>* s=(FieldWrap<scalar>*)lua_newuserdata(luaState,sizeof(FieldWrap<scalar>));
    //     s->data_=data;

    //     luaL_getmetatable(luaState, FieldWrap<T>::metaTable);
    //     lua_setmetatable(luaState, -2);

    //     lua_setglobal(luaState, name.c_str());
    // }
}
