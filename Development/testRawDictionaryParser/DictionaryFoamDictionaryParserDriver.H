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
    Parse to a Dictionary

Contributors/Copyright:
    2017 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>

 SWAK Revision: $Id:  $
\*---------------------------------------------------------------------------*/

#ifndef DICTIONARYFOAMDICTIONARYPARSERDRIVER_H
#define DICTIONARYFOAMDICTIONARYPARSERDRIVER_H

#include "RawFoamDictionaryParserDriver.H"

#include "LIFOStack.H"
#include "dictionary.H"

namespace Foam {
    class DictionaryFoamDictionaryParserDriver
    :
        public RawFoamDictionaryParserDriver
    {

        DictionaryFoamDictionaryParserDriver(const DictionaryFoamDictionaryParserDriver &);
        void operator=(const DictionaryFoamDictionaryParserDriver&);

        typedef LIFOStack<dictionary*> dictStack;

        dictStack dicts_;

        dictionary &getTop() const;

        void addDict();

    public:

        DictionaryFoamDictionaryParserDriver(ErrorMode mode);

        virtual ~DictionaryFoamDictionaryParserDriver();

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

#endif /* DICTIONARYFOAMDICTIONARYPARSERDRIVER_H */
