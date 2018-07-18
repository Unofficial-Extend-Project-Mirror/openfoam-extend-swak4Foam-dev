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

#include "functions4Lua.H"

#include <cmath>

#include "vector.H"
#include "tensor.H"
#include "symmTensor.H"
#include "sphericalTensor.H"

#include "OStringStream.H"
#include "word.H"

#include "swak.H"

#ifdef FOAM_DEV_ADDITIONAL_TENSOR_TYPES
#include "diagTensor.H"
#include "symmTensor4thOrder.H"
#endif

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
    class VectorSpaceWrap {
        T* ptr_;
    public:
        VectorSpaceWrap(T* ptr)
            : ptr_(ptr)
            {}
        T& operator()() { return *ptr_; }
        static const char *metaTable;
    };

    template<class T>
    static T &checkVectorSpace(lua_State *L, int index)
    {
        luaL_checktype(L, index, LUA_TUSERDATA);
        VectorSpaceWrap<T>* data=static_cast<VectorSpaceWrap<T>*>(
            luaL_checkudata(L, index, VectorSpaceWrap<T>::metaTable)
        );
        return (*data)();
    }

    template<class T>
    static int vectorSpaceToString(lua_State *L) {
        T& vs=checkVectorSpace<T>(L, 1);
        OStringStream os;
        os << pTraits<T>::typeName << vs;
        lua_pushstring(L,os.str().c_str());
        return 1;
    }

    template<class T>
    static int getVectorSpaceElement(lua_State *L) {
        T& vs=checkVectorSpace<T>(L, 1);
        const char *cname = luaL_checkstring(L, 2);
        word name(cname);
        int index=-1;
        for(label i=0;i<pTraits<T>::nComponents;i++) {
            if(word(T::componentNames[i])==name) {
                index=i;
                break;
            }
        }
        luaL_argcheck(
            L,
            index>=0 && index<pTraits<T>::nComponents,
            2,
            "unknown component name");

        lua_pushnumber(L, vs[index]);

        return 1;
    }

    template<class T>
    static int setVectorSpaceElement(lua_State *L) {
        T& vs=checkVectorSpace<T>(L, 1);
        const char *cname = luaL_checkstring(L, 2);
        word name(cname);
        int index=-1;
        for(label i=0;i<pTraits<T>::nComponents;i++) {
            if(word(T::componentNames[i])==name) {
                index=i;
                break;
            }
        }
        luaL_argcheck(
            L,
            index>=0 && index<pTraits<T>::nComponents,
            2,
            "unknown component name");

        luaL_checkany(L,3);
        scalar v=luaL_checknumber(L,3);
        vs[index]=v;

        return 1;
    }

#define VectorSpaceInstance(T)                        \
    static const struct luaL_Reg T ## _table[] = {            \
        {"__tostring"   , vectorSpaceToString<T> },           \
        {"__index"      , getVectorSpaceElement<T> },         \
        {"__newindex"   , setVectorSpaceElement<T> },         \
        {NULL, NULL}  /* sentinel */                          \
    };                                                        \
                                                              \
    template<>                                                          \
    const char *VectorSpaceWrap<T>::metaTable="swak4foam." #T;

    VectorSpaceInstance(vector)
    VectorSpaceInstance(tensor)
    VectorSpaceInstance(sphericalTensor)
    VectorSpaceInstance(symmTensor)
#ifdef FOAM_DEV_ADDITIONAL_TENSOR_TYPES
    VectorSpaceInstance(diagTensor)
    VectorSpaceInstance(symmTensor4thOrder)
#endif

    template<class T>
    void newVectorspaceToStack(lua_State *luaState,T *data) {
        VectorSpaceWrap<T>* ptr = static_cast<VectorSpaceWrap<T>*>(
            lua_newuserdata(
                luaState,
                sizeof(VectorSpaceWrap<T>)
            )
        );
        (*ptr)=data;
        luaL_setmetatable(luaState, VectorSpaceWrap<T>::metaTable);
    }

    template<class T>
    void addVectorspaceToLua(lua_State *luaState,const word &name,T *data) {
        newVectorspaceToStack(luaState, data);
        lua_setglobal(luaState, name.c_str());
    }

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

    template<class T>
    static FieldWrap<T> *checkField(lua_State *L, int index)
    {
        FieldWrap<T> *data;
        luaL_checktype(L, index, LUA_TUSERDATA);
        data = static_cast<FieldWrap<T> *>(
            luaL_checkudata(L, index, FieldWrap<T>::metaTable)
        );
        return data;
    }

    template<class T>
    static int newField(lua_State *L) {
        int n = label(luaL_checkinteger(L, 1));

        new(L,FieldWrap<T>::metaTable) FieldWrap<T>(n);

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
        int index = luaL_checkinteger(L, 2) - 1;
        luaL_argcheck(
            L, 0 <= index && index < (*fw)().size(), 2,
            "index out of range");

        newVectorspaceToStack(L, &((*fw)()[index]));

        return 1;
    }

    template<>
    int getFieldElement<scalar>(lua_State *L) {
        FieldWrap<scalar> *fw=checkField<scalar>(L,1);
        int index = luaL_checkinteger(L, 2) - 1;
        luaL_argcheck(
            L, 0 <= index && index < (*fw)().size(), 2,
            "index out of range");

        lua_pushnumber(L, (*fw)()[index]);

        return 1;
    }

    template<class T>
    static int setFieldElement(lua_State *L) {
        FieldWrap<T> *fw=checkField<T>(L,1);
        int index = luaL_checkinteger(L, 2) - 1;
        luaL_argcheck(
            L, 0 <= index && index < (*fw)().size(), 2,
            "index out of range");

        luaL_checkany(L,3);
        T& v=checkVectorSpace<T>(L,3);
        (*fw)()[index]=v;

        return 1;
    }

    template<>
    int setFieldElement<scalar>(lua_State *L) {
        FieldWrap<scalar> *fw=checkField<scalar>(L,1);
        int index = luaL_checkinteger(L, 2) - 1;
        luaL_argcheck(
            L, 0 <= index && index < (*fw)().size(), 2,
            "index out of range");

        luaL_checkany(L,3);
        scalar v=luaL_checknumber(L,3);
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
        {"newVectorField", newField<vector>},
        {"newTensorField", newField<tensor>},
        {"newSymmTensorField", newField<symmTensor>},
        {"newSphericalTensorField", newField<sphericalTensor>},
#ifdef FOAM_DEV_ADDITIONAL_TENSOR_TYPES
        {"newDiagTensorField", newField<diagTensor>},
        {"newSymmTensor4thOrderField", newField<symmTensor4thOrder>},
#endif
        {NULL, NULL}  /* sentinel */
    };

#define declareArrayMetaTable(T) \
    static const struct luaL_Reg T ## Array_table [] = { \
        {"__gc"        , delField<T> },              \
        {"__len"       , fieldSize<T> },             \
        {"__index"     , getFieldElement<T> },       \
        {"__newindex"  , setFieldElement<T> },       \
        {"__tostring"  , fieldToString<T> },         \
        {NULL, NULL}  /* sentinel */                      \
    };                                                    \
    template<>                                            \
    const char *FieldWrap<T>::metaTable=             \
        "swak4Foam." #T "Array";

#define addArrayMetaTable(T)    \
    luaL_newmetatable(luaState, FieldWrap<T>::metaTable); \
    luaL_setfuncs(luaState, T ## Array_table, 0);             \
    lua_pop(luaState, 1);

    declareArrayMetaTable(scalar);
    declareArrayMetaTable(vector);
    declareArrayMetaTable(tensor);
    declareArrayMetaTable(symmTensor);
    declareArrayMetaTable(sphericalTensor);
#ifdef FOAM_DEV_ADDITIONAL_TENSOR_TYPES
    declareArrayMetaTable(diagTensor);
    declareArrayMetaTable(symmTensor4thOrder);
#endif

#define addVectorSpaceMeta(T)   \
    luaL_newmetatable(luaState, VectorSpaceWrap<T>::metaTable); \
    luaL_setfuncs(luaState, T ## _table, 0);                   \
    lua_pop(luaState, 1);

    void addOpenFOAMFunctions(lua_State *luaState) {
        addArrayMetaTable(scalar);
        addArrayMetaTable(vector);
        addArrayMetaTable(tensor);
        addArrayMetaTable(symmTensor);
        addArrayMetaTable(sphericalTensor);
#ifdef FOAM_DEV_ADDITIONAL_TENSOR_TYPES
        addArrayMetaTable(diagTensor);
        addArrayMetaTable(symmTensor4thOrder);
#endif

        addVectorSpaceMeta(vector);
        addVectorSpaceMeta(tensor);
        addVectorSpaceMeta(symmTensor);
        addVectorSpaceMeta(sphericalTensor);
#ifdef FOAM_DEV_ADDITIONAL_TENSOR_TYPES
        addVectorSpaceMeta(diagTensor);
        addVectorSpaceMeta(symmTensor4thOrder);
#endif

        luaL_newlib(luaState,swaklib);

        lua_setglobal(luaState, "swak4foam");

        // static Field<scalar> sField(10);
        // addFieldToLua<scalar>(luaState, "testScalar", &sField);

        // static tensor nix;
        // addVectorspaceToLua(luaState,"nix",&nix);
    }

    template<class T>
    void addFieldToLua(lua_State *luaState,const word &name, Field<T> *data,label index) {
        FieldWrap<T> *fw=new(luaState,FieldWrap<T>::metaTable) FieldWrap<T>(data);

        if(index==0) {
            lua_setglobal(luaState, name.c_str());
        } else {
            lua_setfield(luaState, index, name.c_str());
        }
    }
    template<class T>
    void addFieldToLua(lua_State *luaState,const word &name, const List<T> &data,label index) {
        FieldWrap<T> *fw=new(luaState,FieldWrap<T>::metaTable) FieldWrap<T>(data.size());
        (*fw)()=data;
        if(index==0) {
            lua_setglobal(luaState, name.c_str());
        } else {
            lua_setfield(luaState, index, name.c_str());
        }
    }

    template<class T>
    bool isLuaFieldWrap(lua_State *luaState,int index) {
        luaL_checktype(luaState, index, LUA_TUSERDATA);
        void *fw=luaL_testudata(luaState, index, FieldWrap<T>::metaTable);
        return fw!=NULL;
    }

    template<class T>
    Field<T> &getLuaField(lua_State *luaState,int index) {
        FieldWrap<T> *fw=checkField<T>(luaState,index);
        return (*fw)();
    }

#define makeFunctionInstances(T) \
    template                                                            \
    void addFieldToLua<T>(                                              \
        lua_State *luaState,const word &name, Field<T> *data, label index \
    );                                                                  \
    template                                                            \
    void addFieldToLua<T>(                                              \
        lua_State *luaState,const word &name, const List<T> &data, label index \
    );                                                                  \
    template                                                            \
    bool isLuaFieldWrap<T>(lua_State *luaState,int index);              \
    template                                                            \
    Field<T> &getLuaField<T>(lua_State *luaState,int index);


    makeFunctionInstances(scalar);
    makeFunctionInstances(vector);
    makeFunctionInstances(tensor);
    makeFunctionInstances(symmTensor);
    makeFunctionInstances(sphericalTensor);
#ifdef FOAM_DEV_ADDITIONAL_TENSOR_TYPES
    makeFunctionInstances(diagTensor);
    makeFunctionInstances(symmTensor4thOrder);
#endif

}
