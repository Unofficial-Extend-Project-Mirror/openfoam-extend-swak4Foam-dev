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
    Parser that is the framework for transforming any swak4Foam-dictionary
    into another dictionary-like data structure

    Won't be able to read all possible OF-dictionaries

Contributors/Copyright:
    2017-2018, 2020 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>

 SWAK Revision: $Id:  $
\*---------------------------------------------------------------------------*/

#ifndef RAWFOAMDICTIONARYPARSERDRIVER_H
#define RAWFOAMDICTIONARYPARSERDRIVER_H

#include "Istream.H"
#include "DynamicList.H"
#include "labelList.H"
#include "scalarList.H"
#include "boolList.H"
#include "stringList.H"

#include "swak.H"
#ifdef FOAM_PREFERS_ENUM_TO_NAMED_ENUM
#include "Enum.H"
#else
#include "NamedEnum.H"
#endif

namespace parserRawDict {
    class RawFoamDictionaryParser;
}

namespace Foam {
    class RawFoamDictionaryParserDriver {

    public:
        enum  ErrorMode {
            SilentError,
            FailError,
            WarnError
        };

#ifdef FOAM_PREFERS_ENUM_TO_NAMED_ENUM
        static const Enum<ErrorMode> errorModeNames;
#else
        static const NamedEnum<ErrorMode, 3> errorModeNames;
#endif

    private:

        Istream *is_;

        RawFoamDictionaryParserDriver(const RawFoamDictionaryParserDriver &);
        void operator=(const RawFoamDictionaryParserDriver&);

        int debugLevel_;

        bool rawMode_;

        string error_;

        ErrorMode errorMode_;

        DynamicList<label> labelList_;
        DynamicList<scalar> scalarList_;
        DynamicList<bool> boolList_;
        DynamicList<word> wordList_;
        DynamicList<string> stringList_;

        DynamicList<labelList> labelListList_;
        DynamicList<scalarList> scalarListList_;
        DynamicList<boolList> boolListList_;
        DynamicList<wordList> wordListList_;
        DynamicList<stringList> stringListList_;

    protected:

        void startLabelList();
        void addToList(label val);
        const labelList &getLabelList();
        void moveLabelListToScalar();

        void startScalarList();
        void addToList(scalar val);
        const scalarList &getScalarList();

        void startBoolList();
        void addToList(bool val);
        const boolList &getBoolList();

        void startWordList();
        void addToList(const word &val);
        const wordList &getWordList();

        void startStringList();
        void addToList(const string &val);
        const stringList &getStringList();

        void startLabelListList();
        void addToList(const labelList &val);
        const List<labelList> &getLabelListList();
        void moveLabelListListToScalar();

        void startScalarListList();
        void addToList(const scalarList &val);
        const List<scalarList> &getScalarListList();

        void startBoolListList();
        void addToList(const boolList &val);
        const List<boolList> &getBoolListList();

        void startWordListList();
        void addToList(const wordList &val);
        const List<wordList> &getWordListList();

        void startStringListList();
        void addToList(const stringList &val);
        const List<stringList> &getStringListList();

        void setError(const std::string &e) {
            error_=e;
        }

        const string &getError() const {
            return error_;
        }

        ErrorMode errorMode() const {
            return errorMode_;
        }
    public:

        RawFoamDictionaryParserDriver(ErrorMode mode=WarnError);

        virtual ~RawFoamDictionaryParserDriver();

        Istream &is();

        label parse(Istream &is,int debugLevel=0);

        label parse(const dictionary &dict,int debugLevel=0);

        int debug() {
            return debugLevel_;
        }

        bool rawMode()
            { return rawMode_; }

        void setRawMode() {
            rawMode_=true;
        }
        void resetRawMode() {
            rawMode_=false;
        }

        friend class parserRawDict::RawFoamDictionaryParser;

        virtual void add(const word& name,scalar value) = 0;

        virtual void add(const word& name,label value) = 0;

        virtual void add(const word& name,bool value) = 0;

        virtual void add(const word& name,const word &value) = 0;

        virtual void add(const word& name,const string &value) = 0;

        virtual void add(const word& name,const labelList &value) = 0;

        virtual void add(const word& name,const scalarList &value) = 0;

        virtual void add(const word& name,const boolList &value) = 0;

        virtual void add(const word& name,const wordList &value) = 0;

        virtual void add(const word& name,const stringList &value) = 0;

        virtual void add(const word& name,const List<labelList> &value) = 0;

        virtual void add(const word& name,const List<scalarList> &value) = 0;

        virtual void add(const word& name,const List<boolList> &value) = 0;

        virtual void add(const word& name,const List<wordList> &value) = 0;

        virtual void add(const word& name,const List<stringList> &value) = 0;

        virtual void addTopDictAs(const word &name) = 0;

        virtual void newTopDict() = 0;
    };
}

#endif /* RAWFOAMDICTIONARYPARSERDRIVER_H */
