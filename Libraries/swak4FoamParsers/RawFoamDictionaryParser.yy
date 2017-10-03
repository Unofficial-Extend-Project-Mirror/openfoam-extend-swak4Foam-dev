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
    Parser that is the framework for transforming any OpenFOAM-dictionary
    into another dictionary-like data structure

    Won't be able to read all possible OF-dictionaries

Contributors/Copyright:
    2017 Bernhard F.W. Gschaider <bgschaid@hfd-research.com>

 SWAK Revision: $Id:  $
\*---------------------------------------------------------------------------*/

%skeleton "lalr1.cc" /* -*- C++ -*- */
%require "3.0.4"
%defines
%define parser_class_name {RawFoamDictionaryParser}

%define api.token.constructor
%define api.value.type variant
%define parse.assert
// %define api.prefix {parserRawDict}
%define api.namespace {parserRawDict}

%code requires
{
    #include "string.H"
    #include "scalar.H"
    #include "word.H"
    #include "label.H"
    #include <sstream>

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
| "word" labelListList ";"       {}
| "word" scalarListList ";"      {}
| "word" wordListList ";"        {}
| "word" boolListList ";"        {}
| "word" error ";"           { driver.add($1,driver.getError()); yyerrok; }
| regexp {} error ";"           { yyerrok; }
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
| "(" ")"                    {}
| "label" "(" scalarSeq ")"   {}
| "label" "{" "scalar" "}"    { for(int i=0;i<$1;i++) { driver.addToList($3); } }
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
| "label" "{" "word" "}"    { for(int i=0;i<$1;i++) { driver.addToList($3); } }
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
| "label" "{" "string" "}"    { for(int i=0;i<$1;i++) { driver.addToList($3); } }
;

stringSeq:
  "string"                      { driver.addToList($1); }
| stringSeq "string"             { driver.addToList($2); }
;

labelListList:
  "(" labelListSeq ")"           {}
;

labelListSeq:
  labelList                      {}
| labelListSeq labelList             {}

scalarListList:
  "(" scalarListSeq ")"          {}
;

scalarListSeq:
  scalarList                      {}
| scalarListSeq scalarList            {}
| labelListSeq scalarList             {}
| scalarListSeq labelList            {}
;

wordListList:
  "(" wordListSeq ")"           {}
;

wordListSeq:
  wordList                      {}
| wordListSeq wordList             {}
;

boolListList:
  "(" boolListSeq ")"           {}
;

boolListSeq:
  boolList                      {}
| boolListSeq boolList             {}
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

    WarningIn("RawFoamDictionaryParser")
        << o.str()
            << endl;
}

parserRawDict::RawFoamDictionaryParser::symbol_type parserRawDict::yylex(
    Foam::RawFoamDictionaryParserDriver &driver
) {
    using namespace Foam;

    RawFoamDictionaryParser::location_type loc(
        &(driver.is().name()),
        driver.is().lineNumber(),
        0
    );

    // Info << nl << driver.is().eof() << " " << driver.is().good()
    //     << driver.is().bad() << endl;

    if(driver.is().eof()) {
        return RawFoamDictionaryParser::make_END_OF_FILE(loc);
    }

    token nextToken(driver.is());

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
        case token::LONG_DOUBLE_SCALAR:
            return RawFoamDictionaryParser::make_SCALAR(
                scalar(nextToken.longDoubleScalarToken()),
                loc
            );
            break;
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
            }
            break;
        case token::FATALERROR:
            return RawFoamDictionaryParser::make_END_OF_FILE(loc);
            break;
        case token::COMPOUND:
        case token::UNDEFINED:
            break;
    }
}
