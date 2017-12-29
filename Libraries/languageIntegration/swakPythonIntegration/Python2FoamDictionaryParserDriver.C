/*----------------------- -*- C++ -*- ---------------------------------------*\
|                       _    _  _     ___                       | The         |
|     _____      ____ _| | _| || |   / __\__   __ _ _ __ ___    | Swiss       |
|    / __\ \ /\ / / _` | |/ / || |_ / _\/ _ \ / _` | '_ ` _ \   | Army        |
|    \__ \\ V  V / (_| |   <|__   _/ / | (_) | (_| | | | | | |  | Knife       |
|    |___/ \_/\_/ \__,_|_|\_\  |_| \/   \___/ \__,_|_| |_| |_|  | For         |
|                                                               | OpenFOAM    |
-----------------------------------------------------------------------------*

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
    Parse to Python2

Contributors/Copyright:
    2017 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>

 SWAK Revision: $Id:  $
\*---------------------------------------------------------------------------*/

#include "Python2FoamDictionaryParserDriver.H"
#include <cassert>

namespace Foam {
    Python2FoamDictionaryParserDriver::Python2FoamDictionaryParserDriver(
        pythonInterpreterWrapper &parent,
        ErrorMode mode
    )
        :
        RawFoamDictionaryParserDriver(mode),
        parent_(parent)
    {
    }

    Python2FoamDictionaryParserDriver::~Python2FoamDictionaryParserDriver()
    {
    }

    void Python2FoamDictionaryParserDriver::add(const word& name,scalar value)
    {
        PyDict_SetItemString(
            parent_.currentDictionary(),
            name.c_str(),
            PyFloat_FromDouble(value)
        );
    }

    void Python2FoamDictionaryParserDriver::add(const word& name,label value)
    {
        PyDict_SetItemString(
            parent_.currentDictionary(),
            name.c_str(),
            PyInt_FromLong(value)
        );
    }

    void Python2FoamDictionaryParserDriver::add(const word& name,bool value)
    {
        PyDict_SetItemString(
            parent_.currentDictionary(),
            name.c_str(),
            PyBool_FromLong(value)
        );
    }

    void Python2FoamDictionaryParserDriver::add(const word& name,const word &value)
    {
        PyDict_SetItemString(
            parent_.currentDictionary(),
            name.c_str(),
            PyString_FromString(value.c_str())
        );
    }

    void Python2FoamDictionaryParserDriver::add(const word& name,const string &value)
    {
        PyDict_SetItemString(
            parent_.currentDictionary(),
            name.c_str(),
            PyString_FromString(value.c_str())
        );
    }

    void Python2FoamDictionaryParserDriver::add(const word& name,const labelList &value)
    {
    }

    void Python2FoamDictionaryParserDriver::add(const word& name,const scalarList &value)
    {
    }

    void Python2FoamDictionaryParserDriver::add(const word& name,const boolList &value)
    {
    }

    void Python2FoamDictionaryParserDriver::add(const word& name,const wordList &value)
    {
    }

    void Python2FoamDictionaryParserDriver::add(const word& name,const stringList &value)
    {
    }

    void Python2FoamDictionaryParserDriver::add(const word& name,const List<labelList> &value)
    {
    }

    void Python2FoamDictionaryParserDriver::add(const word& name,const List<scalarList> &value)
    {
    }

    void Python2FoamDictionaryParserDriver::add(const word& name,const List<boolList> &value)
    {
    }

    void Python2FoamDictionaryParserDriver::add(const word& name,const List<wordList> &value)
    {
    }

    void Python2FoamDictionaryParserDriver::add(const word& name,const List<stringList> &value)
    {
    }

    void Python2FoamDictionaryParserDriver::addTopDictAs(const word &name) {
        parent_.wrapUpDictionary(name);
    }

    void Python2FoamDictionaryParserDriver::newTopDict() {
        parent_.startDictionary();
    }

}
