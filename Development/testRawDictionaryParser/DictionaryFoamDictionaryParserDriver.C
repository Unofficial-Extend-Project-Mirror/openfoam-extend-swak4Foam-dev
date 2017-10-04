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
    Parse to dictionary

Contributors/Copyright:
    2017 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>

 SWAK Revision: $Id:  $
\*---------------------------------------------------------------------------*/

#include "DictionaryFoamDictionaryParserDriver.H"
#include <cassert>

namespace Foam {
    DictionaryFoamDictionaryParserDriver::DictionaryFoamDictionaryParserDriver(
        ErrorMode mode
    )
        :
        RawFoamDictionaryParserDriver(mode)
    {
        addDict();
    }

    DictionaryFoamDictionaryParserDriver::~DictionaryFoamDictionaryParserDriver()
    {
        while(dicts_.size()>0) {
            delete dicts_.pop();
        }
    }

    const dictionary &DictionaryFoamDictionaryParserDriver::getResult() const
    {
        return getTop();
    }

    dictionary &DictionaryFoamDictionaryParserDriver::getTop() const
    {
        assert(dicts_.size()>0);

        return *(dicts_.top());
    }

    void DictionaryFoamDictionaryParserDriver::addDict()
    {
        dicts_.push(
            new dictionary()
        );
    }

    void DictionaryFoamDictionaryParserDriver::add(const word& name,scalar value)
    {
        getTop().add(name,value);
    }

    void DictionaryFoamDictionaryParserDriver::add(const word& name,label value)
    {
        getTop().add(name,value);
    }

    void DictionaryFoamDictionaryParserDriver::add(const word& name,bool value)
    {
        getTop().add(name,value);
    }

    void DictionaryFoamDictionaryParserDriver::add(const word& name,const word &value)
    {
        getTop().add(name,value);
    }

    void DictionaryFoamDictionaryParserDriver::add(const word& name,const string &value)
    {
        getTop().add(name,value);
    }

    void DictionaryFoamDictionaryParserDriver::add(const word& name,const labelList &value)
    {
        getTop().add(name,value);
    }

    void DictionaryFoamDictionaryParserDriver::add(const word& name,const scalarList &value)
    {
        getTop().add(name,value);
    }

    void DictionaryFoamDictionaryParserDriver::add(const word& name,const boolList &value)
    {
        getTop().add(name,value);
    }

    void DictionaryFoamDictionaryParserDriver::add(const word& name,const wordList &value)
    {
        getTop().add(name,value);
    }

    void DictionaryFoamDictionaryParserDriver::add(const word& name,const stringList &value)
    {
        getTop().add(name,value);
    }

    void DictionaryFoamDictionaryParserDriver::add(const word& name,const List<labelList> &value)
    {
        getTop().add(name,value);
    }

    void DictionaryFoamDictionaryParserDriver::add(const word& name,const List<scalarList> &value)
    {
        getTop().add(name,value);
    }

    void DictionaryFoamDictionaryParserDriver::add(const word& name,const List<boolList> &value)
    {
        getTop().add(name,value);
    }

    void DictionaryFoamDictionaryParserDriver::add(const word& name,const List<wordList> &value)
    {
        getTop().add(name,value);
    }

    void DictionaryFoamDictionaryParserDriver::add(const word& name,const List<stringList> &value)
    {
        getTop().add(name,value);
    }

    void DictionaryFoamDictionaryParserDriver::addTopDictAs(const word &name) {
        dictionary *dPtr=dicts_.pop();
        getTop().add(name,*dPtr);
        delete dPtr;
    }

    void DictionaryFoamDictionaryParserDriver::newTopDict() {
        addDict();
    }

}
