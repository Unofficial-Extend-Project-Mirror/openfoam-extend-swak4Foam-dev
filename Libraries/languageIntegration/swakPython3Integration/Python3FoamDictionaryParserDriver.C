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
    Parse to Python3

Contributors/Copyright:
    2017-2018 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>

 SWAK Revision: $Id:  $
\*---------------------------------------------------------------------------*/

#include "Python3FoamDictionaryParserDriver.H"
#include <cassert>

#include "python3HelperTemplates.H"

namespace Foam {
    Python3FoamDictionaryParserDriver::Python3FoamDictionaryParserDriver(
        python3InterpreterWrapper &parent,
        ErrorMode mode
    )
        :
        RawFoamDictionaryParserDriver(mode),
        parent_(parent)
    {
    }

    Python3FoamDictionaryParserDriver::~Python3FoamDictionaryParserDriver()
    {
    }

    template<class T>
    void Python3FoamDictionaryParserDriver::add1DListDirect(
        const word &name,const List<T> &value
    ) {
        PyObject *list=PyList_New(value.size());
        for(label i=0;i<value.size();i++) {
            PyList_SetItem(
                list,
                i,
                makePrimitiveElement(value[i])
            );
        }
        PyDict_SetItemString(
            parent_.currentDictionary(),
            name.c_str(),
            list
        );
    }

    template<class T>
    void Python3FoamDictionaryParserDriver::add1DList(
        const word &name,const List<T> &value
    ) {
        // if(parent_.useNumpy()) {
        //     notImplemented("Adding parsed lists as numpy");
        // } else {
            add1DListDirect(name, value);
        // }
    }

    template<class T>
    void Python3FoamDictionaryParserDriver::add2DListDirect(
        const word &name,
        const List<List<T> > &value
    ) {
        PyObject *outerList=PyList_New(value.size());
        for(label i=0;i<value.size();i++) {
            const List<T> &iValue=value[i];
            PyObject *innerList=PyList_New(iValue.size());
            for(label j=0;j<iValue.size();j++) {
                PyList_SetItem(
                    innerList,
                    j,
                    makePrimitiveElement(iValue[j])
                );
            }
            PyList_SetItem(
                outerList,
                i,
                innerList
            );
        }
        PyDict_SetItemString(
            parent_.currentDictionary(),
            name.c_str(),
            outerList
        );
    }

    template<class T>
    void Python3FoamDictionaryParserDriver::add2DList(
        const word &name,
        const List<List<T> > &value
    ) {
        // if(
        //     parent_.useNumpy()
        //     &&
        //     isUniform2DList(value)
        // ) {
        //     notImplemented("Adding parsed lists of lists as numpy");
        // } else {
            add2DListDirect(name, value);
        // }
    }

    template<class T>
    bool Python3FoamDictionaryParserDriver::isUniform2DList(
        const List<List<T> > &value
    ) {
        if(value.size()==0) {
            return true;
        }
        label size=value[0].size();
        forAll(value,i) {
            if(value[i].size()!=size) {
                return false;
            }
        }
        return true;
    }

    void Python3FoamDictionaryParserDriver::add(const word& name,scalar value)
    {
        PyDict_SetItemString(
            parent_.currentDictionary(),
            name.c_str(),
            PyFloat_FromDouble(value)
        );
    }

    void Python3FoamDictionaryParserDriver::add(const word& name,label value)
    {
        PyDict_SetItemString(
            parent_.currentDictionary(),
            name.c_str(),
            PyLong_FromLong(value)
        );
    }

    void Python3FoamDictionaryParserDriver::add(const word& name,bool value)
    {
        PyDict_SetItemString(
            parent_.currentDictionary(),
            name.c_str(),
            PyBool_FromLong(value)
        );
    }

    void Python3FoamDictionaryParserDriver::add(
        const word& name,const word &value
    ) {
        //        string cmd='"'+value+'"';
        //        PyObject *o=PyRun_String(cmd.c_str(),0,NULL,NULL);
        PyDict_SetItemString(
            parent_.currentDictionary(),
            name.c_str(),
            //            PyBytes_FromString(value.c_str())
            //            PyUnicode_DecodeLocale(value.c_str(),NULL)
            //            PyUnicode_DecodeLocale("nixda",NULL)
            //            o
            PyUnicode_FromString(value.c_str())
        );
    }

    void Python3FoamDictionaryParserDriver::add(
        const word& name,const string &value
    ) {
        PyDict_SetItemString(
            parent_.currentDictionary(),
            name.c_str(),
            PyUnicode_FromString(value.c_str())
        );
    }

    void Python3FoamDictionaryParserDriver::add(
        const word& name,const labelList &value
    ) {
        add1DList(name, value);
    }

    void Python3FoamDictionaryParserDriver::add(
        const word& name,const scalarList &value
    ) {
        add1DList(name, value);
    }

    void Python3FoamDictionaryParserDriver::add(
        const word& name,const boolList &value
    ) {
        add1DList(name, value);
    }

    void Python3FoamDictionaryParserDriver::add(
        const word& name,const wordList &value
    ) {
        add1DListDirect(name, value);
    }

    void Python3FoamDictionaryParserDriver::add(
        const word& name,const stringList &value
    ) {
        add1DListDirect(name, value);
    }

    void Python3FoamDictionaryParserDriver::add(
        const word& name,const List<labelList> &value
    ) {
        add2DList(name, value);
    }

    void Python3FoamDictionaryParserDriver::add(
        const word& name,const List<scalarList> &value
    ) {
        add2DList(name, value);
    }

    void Python3FoamDictionaryParserDriver::add(
        const word& name,const List<boolList> &value
    ) {
        add2DList(name, value);
    }

    void Python3FoamDictionaryParserDriver::add(
        const word& name,const List<wordList> &value
    ) {
        add2DListDirect(name, value);
    }

    void Python3FoamDictionaryParserDriver::add(
        const word& name,const List<stringList> &value
    ) {
        add2DListDirect(name, value);
    }

    void Python3FoamDictionaryParserDriver::addTopDictAs(const word &name) {
        parent_.wrapUpDictionary(name);
    }

    void Python3FoamDictionaryParserDriver::newTopDict() {
        parent_.startDictionary();
    }

}
