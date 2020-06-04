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

#include "RawFoamDictionaryParserDriver.H"
#include "RawFoamDictionaryParser.tab.hh"

#include "dictionary.H"

namespace Foam {

#ifdef FOAM_PREFERS_ENUM_TO_NAMED_ENUM
    const Enum<RawFoamDictionaryParserDriver::ErrorMode>
    RawFoamDictionaryParserDriver::errorModeNames
    ({
        {ErrorMode::SilentError,"silent"},
        {ErrorMode::FailError,"fail"},
        {ErrorMode::WarnError,"warn"}
    });
#else
    template<>
    const char* NamedEnum
    <
        RawFoamDictionaryParserDriver::ErrorMode,
        3
        >::names[] =
    {
        "silent",
        "fail",
        "warn"
    };

    const NamedEnum<RawFoamDictionaryParserDriver::ErrorMode, 3>
        RawFoamDictionaryParserDriver::errorModeNames;
#endif

    RawFoamDictionaryParserDriver::RawFoamDictionaryParserDriver(
        ErrorMode mode
    )
        :
        is_(NULL),
        debugLevel_(0),
        rawMode_(false),
        errorMode_(mode)
    {
        startLabelList();
        startScalarList();
        startBoolList();
        startWordList();
        startStringList();

        startLabelListList();
        startScalarListList();
        startBoolListList();
        startWordListList();
        startStringListList();
    }

    RawFoamDictionaryParserDriver::~RawFoamDictionaryParserDriver()
    {}

    Istream &RawFoamDictionaryParserDriver::is() {
        if(is_==NULL) {
            FatalErrorIn("RawFoamDictionaryParserDriver::is()")
                << "The input stream was not set"
                    << endl
                    << exit(FatalError);
        }
        return *is_;
    }

    label RawFoamDictionaryParserDriver::parse(Istream &is,int debugLevel) {
        is_=&is;

        debugLevel_=debugLevel;

        parserRawDict::RawFoamDictionaryParser parser (*this);
        parser.set_debug_level (debugLevel);
        int res = parser.parse ();

        is_=NULL;

        return res;
    }

    label RawFoamDictionaryParserDriver::parse(const dictionary &dict,int debugLevel) {
        const wordList &toc=dict.toc();

        int res=0;

        forAll(toc,i) {
            const word &name=toc[i];

            if(dict.isDict(name)) {
                newTopDict();
                res+=parse(dict.subDict(name),debugLevel);
                addTopDictAs(name);
            } else {
                tokenList inputTokens(1,token(name));

                inputTokens.append(dict.lookup(name));
                inputTokens.append(tokenList(1,token(token::END_STATEMENT)));
                ITstream inStream(
                    dict.name(),
                    inputTokens
                );

                res+=parse(inStream,debugLevel);
            }
        }
        return res;
    }

    void RawFoamDictionaryParserDriver::startLabelList() {
        labelList_.clear();
    }
    void RawFoamDictionaryParserDriver::addToList(label val) {
        labelList_.append(val);
    }
    const labelList &RawFoamDictionaryParserDriver::getLabelList() {
        return labelList_;
    }

    void RawFoamDictionaryParserDriver::moveLabelListToScalar() {
        startScalarList();
        forAll(labelList_,i) {
            addToList(scalar(labelList_[i]));
        }
        startLabelList();
    }

    void RawFoamDictionaryParserDriver::startScalarList() {
        scalarList_.clear();
    }
    void RawFoamDictionaryParserDriver::addToList(scalar val) {
        scalarList_.append(val);
    }
    const scalarList &RawFoamDictionaryParserDriver::getScalarList() {
        return scalarList_;
    }

    void RawFoamDictionaryParserDriver::startBoolList() {
        boolList_.clear();
    }
    void RawFoamDictionaryParserDriver::addToList(bool val) {
        boolList_.append(val);
    }
    const boolList &RawFoamDictionaryParserDriver::getBoolList() {
        return boolList_;
    }

    void RawFoamDictionaryParserDriver::startWordList() {
        wordList_.clear();
    }
    void RawFoamDictionaryParserDriver::addToList(const word &val) {
        wordList_.append(val);
    }
    const wordList &RawFoamDictionaryParserDriver::getWordList() {
        return wordList_;
    }

    void RawFoamDictionaryParserDriver::startStringList() {
        stringList_.clear();
    }
    void RawFoamDictionaryParserDriver::addToList(const string &val) {
        stringList_.append(val);
    }
    const stringList &RawFoamDictionaryParserDriver::getStringList() {
        return stringList_;
    }

    void RawFoamDictionaryParserDriver::startLabelListList() {
        labelListList_.clear();
    }
    void RawFoamDictionaryParserDriver::addToList(const labelList &val) {
        labelListList_.append(val);
    }
    const List<labelList> &RawFoamDictionaryParserDriver::getLabelListList() {
        return labelListList_;
    }

    void RawFoamDictionaryParserDriver::moveLabelListListToScalar() {
        startScalarListList();
        forAll(labelListList_,i) {
            scalarList val(labelListList_[i].size());
            forAll(val,j) {
                val[j]=labelListList_[i][j];
            }
            addToList(val);
        }
        startLabelListList();
    }

    void RawFoamDictionaryParserDriver::startScalarListList() {
        scalarListList_.clear();
    }
    void RawFoamDictionaryParserDriver::addToList(const scalarList &val) {
        scalarListList_.append(val);
    }
    const List<scalarList> &RawFoamDictionaryParserDriver::getScalarListList() {
        return scalarListList_;
    }

    void RawFoamDictionaryParserDriver::startBoolListList() {
        boolListList_.clear();
    }
    void RawFoamDictionaryParserDriver::addToList(const boolList &val) {
        boolListList_.append(val);
    }
    const List<boolList> &RawFoamDictionaryParserDriver::getBoolListList() {
        return boolListList_;
    }

    void RawFoamDictionaryParserDriver::startWordListList() {
        wordListList_.clear();
    }
    void RawFoamDictionaryParserDriver::addToList(const wordList &val) {
        wordListList_.append(val);
    }
    const List<wordList> &RawFoamDictionaryParserDriver::getWordListList() {
        return wordListList_;
    }

    void RawFoamDictionaryParserDriver::startStringListList() {
        stringListList_.clear();
    }
    void RawFoamDictionaryParserDriver::addToList(const stringList &val) {
        stringListList_.append(val);
    }
    const List<stringList> &RawFoamDictionaryParserDriver::getStringListList() {
        return stringListList_;
    }


}
