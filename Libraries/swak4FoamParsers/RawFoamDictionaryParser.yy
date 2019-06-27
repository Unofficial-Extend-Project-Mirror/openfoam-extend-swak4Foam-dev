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
    2017-2018 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>

 SWAK Revision: $Id:  $
\*---------------------------------------------------------------------------*/

%skeleton "lalr1.cc" /* -*- C++ -*- */
%require "3.0.4"
%defines
%define api.parser.class {RawFoamDictionaryParser}

%define api.token.constructor
%define api.value.type variant
%define parse.assert
// %define api.prefix {parserRawDict}
%define api.namespace {parserRawDict}

%code requires
{
    #include "swak.H"
    #include "swakString.H"
    #include "scalar.H"
    #include "word.H"
    #include "label.H"
    #include <sstream>
    #include "OStringStream.H"

    namespace Foam {
        class RawFoamDictionaryParserDriver;
    }

}

%param { Foam::RawFoamDictionaryParserDriver& driver }

%locations
%initial-action
{
    // Initialize the initial location.
    @$.begin.filename = @$.end.filename = &(driver.is().name());
};

%define parse.trace
%define parse.error verbose

%code
{
# include "RawFoamDictionaryParserDriver.H"

    namespace parserRawDict {
        RawFoamDictionaryParser::symbol_type yylex(
            Foam::RawFoamDictionaryParserDriver &driver
        );
    }
}

%define api.token.prefix {TOKEN_}

%token
    END_OF_FILE  0   "end of file"
    END_STATEMENT    ";"
    BEGIN_LIST       "("
    END_LIST         ")"
    BEGIN_SQR        "["
    END_SQR          "]"
    BEGIN_BLOCK      "{"
    END_BLOCK        "}"
    COLON            ":"
    COMMA            ","
     ;

%token <Foam::word> WORD "word"
%token <Foam::word> REDIRECTION "redirection"
%token <Foam::scalar> SCALAR "scalar"
%token <Foam::label> LABEL "label"
%token <Foam::string> STRING "string"
%token <Foam::string> UNKNOWN "unknown"
%token <Foam::string> RAW "raw"
%token <bool> BOOL "bool"

// %type  <int> exp

%printer { yyoutput << $$; } <*>;

%%

%start assignments;

assignments:
  %empty                     {}
| assignments assignment     {};

assignment:
  "word" "word" ";"          { driver.add($1,$2); }
| "word" "redirection" ";"   { error(@2,"Redirection "+$2+" can't be parsed"); driver.add($1,driver.getError()); }
| "word" "scalar" ";"        { driver.add($1,$2); }
| "word" "label" ";"         { driver.add($1,$2); }
| "word" "bool" ";"          { driver.add($1,$2); }
| "word" "string" ";"        { driver.add($1,$2); }
| "word" dictionary          { driver.addTopDictAs($1); }
| "word" labelList ";"       { driver.add($1,driver.getLabelList()); driver.startLabelList(); }
| "word" scalarList ";"      { driver.add($1,driver.getScalarList()); driver.startScalarList(); }
| "word" wordList ";"        { driver.add($1,driver.getWordList()); driver.startWordList(); }
| "word" boolList ";"        { driver.add($1,driver.getBoolList()); driver.startBoolList(); }
| "word" stringList ";"      { driver.add($1,driver.getStringList()); driver.startStringList(); }
| "word" labelListList ";"       {driver.add($1,driver.getLabelListList()); driver.startLabelListList();}
| "word" scalarListList ";"      {driver.add($1,driver.getScalarListList()); driver.startScalarListList();}
| "word" wordListList ";"        {driver.add($1,driver.getWordListList()); driver.startWordListList();}
| "word" boolListList ";"        {driver.add($1,driver.getBoolListList()); driver.startBoolListList();}
| "word" stringListList ";"        {driver.add($1,driver.getStringListList()); driver.startStringListList();}
| "word" error ";"           { driver.add($1,driver.getError()); yyerrok; }
| "word" "word" { yyclearin; driver.setRawMode();} "raw" ";"    {
    driver.add($1,Foam::string(Foam::string($2)+" "+Foam::string($4)));
                                       }
| regexp {} error ";"           { yyerrok; }
| ";"                        {}
;

regexp:
  "string" {
      error(@1,"Can't parse regular expression key "+$1);
      YYERROR;
      // throw syntax_error(@1,"Can't parse regular expression key "+$1);
  }
;

labelList:
  "(" labelSeq ")"           {}
| "label" "(" labelSeq ")"   {}
| "label" "{" "label" "}"    { for(int i=0;i<$1;i++) { driver.addToList($3); } }
;

labelSeq:
  "label"                      { driver.addToList($1); }
| labelSeq "label"             { driver.addToList($2); }

scalarList:
  "(" scalarSeq ")"          {}
| emptyList                   {}
| "label" "(" scalarSeq ")"   {}
| "label" "{" "scalar" "}"    { for(int i=0;i<$1;i++) { driver.addToList($3); } }
;

emptyList:
  "(" ")"
;

scalarSeq:
"scalar"                      { driver.addToList($1); }
| scalarSeq "scalar"            { driver.addToList($2); }
| labelSeq "scalar"             { driver.moveLabelListToScalar(); driver.addToList($2); }
| scalarSeq "label"            { driver.addToList(Foam::scalar($2)); }
;

wordList:
  "(" wordSeq ")"           {}
| "label" "(" wordSeq ")"   {}
;

wordSeq:
"word"                      { driver.addToList($1); }
| wordSeq "word"             { driver.addToList($2); }
;

boolList:
  "(" boolSeq ")"           {}
| "label" "(" boolSeq ")"   {}
| "label" "{" "bool" "}"    { for(int i=0;i<$1;i++) { driver.addToList($3); } }
;

boolSeq:
"bool"                      { driver.addToList($1); }
| boolSeq "bool"             { driver.addToList($2); }
;

stringList:
  "(" stringSeq ")"           {}
| "label" "(" stringSeq ")"   {}
;

stringSeq:
  "string"                      { driver.addToList($1); }
| stringSeq "string"             { driver.addToList($2); }
;

labelListList:
  "(" labelListSeq ")"           {}
;

labelListSeq:
  labelList                      { driver.addToList(driver.getLabelList()); driver.startLabelList();}
| labelListSeq labelList             {driver.addToList(driver.getLabelList()); driver.startLabelList();}
;

scalarListList:
  "(" scalarListSeq ")"          {}
;

scalarListSeq:
  scalarList                      {driver.addToList(driver.getScalarList()); driver.startScalarList();}
| scalarListSeq scalarList            { driver.addToList(driver.getScalarList()); driver.startScalarList(); }
| labelListSeq scalarList             { driver.moveLabelListListToScalar(); driver.addToList(driver.getScalarList()); driver.startScalarList(); }
| scalarListSeq labelList            { driver.moveLabelListToScalar(); driver.addToList(driver.getScalarList()); driver.startScalarList(); }
;

wordListList:
  "(" wordListSeq ")"           {}
;

wordListSeq:
  wordList                      {driver.addToList(driver.getWordList()); driver.startWordList();}
| wordListSeq wordList             {driver.addToList(driver.getWordList()); driver.startWordList();}
| wordListSeq emptyList             {driver.addToList(driver.getWordList()); driver.startWordList();}
;

stringListList:
"(" stringListSeq ")"           {}
;

stringListSeq:
  stringList                      {driver.addToList(driver.getStringList()); driver.startStringList();}
| stringListSeq stringList             { driver.addToList(driver.getStringList()); driver.startStringList(); }
| stringListSeq emptyList             { driver.addToList(driver.getStringList()); driver.startStringList(); }
;

boolListList:
  "(" boolListSeq ")"           {}
;

boolListSeq:
  boolList                      { driver.addToList(driver.getBoolList()); driver.startBoolList();}
| boolListSeq boolList             { driver.addToList(driver.getBoolList()); driver.startBoolList();}
| boolListSeq emptyList             { driver.addToList(driver.getBoolList()); driver.startBoolList();}
;


dictionary:
  "{" { driver.newTopDict(); } assignments "}"        {}

%%

void
parserRawDict::RawFoamDictionaryParser::error(
    const location_type& l,
    const std::string& m
)
{
    using namespace Foam;

    std::ostringstream o;
    o << "Parser error " << m << " at " << l;

    driver.setError(o.str());

    switch(driver.errorMode()) {
        case RawFoamDictionaryParserDriver::WarnError:
            WarningIn("RawFoamDictionaryParser")
                << o.str()
                    << endl;
            break;
        case RawFoamDictionaryParserDriver::FailError:
            //            Foam::Info << "Fail" << endl;
            FatalErrorIn("RawFoamDictionaryParser")
                << o.str()
                    << endl
                    << exit(FatalError);
            break;
        case RawFoamDictionaryParserDriver::SilentError:
            break;
    }
}

parserRawDict::RawFoamDictionaryParser::symbol_type parserRawDict::yylex(
    Foam::RawFoamDictionaryParserDriver &driver
) {
    using namespace Foam;

    static token lastToken;
    static bool pushed=false;  // work around a EOF-problem

    RawFoamDictionaryParser::location_type loc(
        &(driver.is().name()),
        driver.is().lineNumber(),
        0
    );

    // Info << nl << driver.is().eof() << " " << driver.is().good()
    //     << driver.is().bad() << endl;
    if(driver.debug()>2 ) {
        Info << "Stream: Good " << driver.is().good() << " EOF "
            << driver.is().eof() << endl;
    }

    if(driver.is().eof()) {
        if(!pushed) {
            if(driver.debug()>2 ) {
                Info << "Detected EOF" << endl;
            }
            return RawFoamDictionaryParser::make_END_OF_FILE(loc);
        }
    }
    pushed=false;

    if(driver.rawMode()) {
        if(driver.debug()>2 ) {
            Info << "Starting raw mode " << endl;
        }

        Foam::OStringStream result;
        result << lastToken;

        while(driver.is().good()) {
            token tok(driver.is());
            if(
                tok.isPunctuation()
                &&
                tok.pToken()==token::END_STATEMENT
            ) {
                if(driver.debug()>2 ) {
                    Info << "Found ';'" << endl;
                    Info << "Stream: Good " << driver.is().good() << " EOF "
                        << driver.is().eof() << endl;
                }
                driver.is().putBack(tok);
                pushed=true;
                break;
            }
            result << " " << tok;
        }
        driver.resetRawMode();
        return RawFoamDictionaryParser::make_RAW(result.str(),loc);
    }
    token nextToken(driver.is());
    lastToken=nextToken;

    if(driver.debug()>2 ) {
        Info << "Next token: " << nextToken.info() << endl;
    }
    switch(nextToken.type()) {
        case token::WORD:
            if(
                nextToken.wordToken()=="yes"
                ||
                nextToken.wordToken()=="true"
                ||
                nextToken.wordToken()=="on"
            ) {
                return RawFoamDictionaryParser::make_BOOL(
                    true,
                    loc
                );
            }
            if(
                nextToken.wordToken()=="no"
                ||
                nextToken.wordToken()=="false"
                ||
                nextToken.wordToken()=="off"
            ) {
                return RawFoamDictionaryParser::make_BOOL(
                    false,
                    loc
                );
            }
            if(nextToken.wordToken()[0]=='$') {
                return RawFoamDictionaryParser::make_REDIRECTION(
                    nextToken.wordToken(),
                    loc
                );
            }
            return RawFoamDictionaryParser::make_WORD(
                nextToken.wordToken(),
                loc
            );
            break;
        case token::STRING:
            return RawFoamDictionaryParser::make_STRING(
                nextToken.stringToken(),
                loc
            );
            break;
        case token::FLOAT_SCALAR:
            return RawFoamDictionaryParser::make_SCALAR(
                scalar(nextToken.floatScalarToken()),
                loc
            );
            break;
        case token::DOUBLE_SCALAR:
            return RawFoamDictionaryParser::make_SCALAR(
                scalar(nextToken.doubleScalarToken()),
                loc
            );
            break;
#ifdef FOAM_HAS_LONG_DOUBLE_SCALAR
        case token::LONG_DOUBLE_SCALAR:
            return RawFoamDictionaryParser::make_SCALAR(
                scalar(nextToken.longDoubleScalarToken()),
                loc
            );
            break;
#endif
        case token::LABEL:
            return RawFoamDictionaryParser::make_LABEL(
                nextToken.labelToken(),
                loc
            );
            break;
        case token::PUNCTUATION:
            switch(nextToken.pToken()) {
                case token::BEGIN_BLOCK:
                    return RawFoamDictionaryParser::make_BEGIN_BLOCK(loc);
                case token::END_BLOCK:
                    return RawFoamDictionaryParser::make_END_BLOCK(loc);
                case token::BEGIN_LIST:
                    return RawFoamDictionaryParser::make_BEGIN_LIST(loc);
                case token::END_LIST:
                    return RawFoamDictionaryParser::make_END_LIST(loc);
                case token::END_STATEMENT:
                    return RawFoamDictionaryParser::make_END_STATEMENT(loc);
                default:
                    return RawFoamDictionaryParser::make_UNKNOWN(string(nextToken.pToken()),loc);
            }
            break;
#ifdef FOAM_TOKEN_CLASS_HAS_FATAL_ERROR
        case token::FATALERROR:
#else
        case token::ERROR:
#endif
            return RawFoamDictionaryParser::make_END_OF_FILE(loc);
            break;
        case token::COMPOUND:
        case token::UNDEFINED:
        default:
            return RawFoamDictionaryParser::make_UNKNOWN(string("undefined token"),loc);
            break;
    }
}
